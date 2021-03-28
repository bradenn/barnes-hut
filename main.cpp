#include <iostream>
#include "BHTree.h"
#include "BHGraphics.h"
#include "Quad.h"
#include <SDL.h>
#include <algorithm>
#include <vector>
#include <sys/resource.h>

bool showQuads = false;
float step = 1;
bool brute = false;

float fmap(float value,
           float istart,
           float istop,
           float ostart,
           float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

void updateStatistics(std::vector<Body *> &bodies, BHGraphics *g, float dt,
                      float dF) {
    auto cam = g->getCamera();
    float xPad = 12, yPad = 8;
    float xOff = 1024 + xPad, yOff = 32;
    float lnOff = yOff;
    g->setAlphaColor(32, 32, 32, 128);
    g->fillRect(xOff - xPad * 2, yPad * 2, 340 - xPad, 1024 - yPad * 4);
    g->drawString("N-Bodies", 4, xOff, lnOff);
    g->drawString("Statistics", 3, xOff + 10, lnOff += 50);
    g->drawString("Step Size: " + std::to_string(step), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Subjective Time: " + std::to_string(dt), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Relative Time: " + std::to_string(dt / step), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Outer Quad: " + std::to_string(cam.radius), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Remaining Bodies: " + std::to_string(bodies.size()), 2,
                  xOff + 20,
                  lnOff += 30);
    g->drawString("Rendering", 3, xOff + 10, lnOff += 40);
    g->drawString("Frame Time: " + std::to_string((int) (1000.0 / dF)
                  ) + "ms",
                  2, xOff +
                     20,
                  lnOff += 30);
    g->drawString("FPS: " + std::to_string((int) dF) + "fps ",
                  2,
                  xOff +
                  20,
                  lnOff += 30);
    g->drawString("Computation", 3, xOff + 10, lnOff += 50);
    g->drawString(
            "Calculation Type: " + string((brute) ? "Brute" : "Barnes-Hut"), 2,
            xOff + 20,
            lnOff += 30);
    g->drawString("Resources", 3, xOff + 10, lnOff += 40);
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    g->drawString("MaxRSS: " + std::to_string(r_usage.ru_maxrss / (1024 *
                                                                   1024)) +
                  " Mb", 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Camera", 3, xOff + 10, lnOff += 40);
    g->drawString("Scale: " + std::to_string(cam.scale), 2, xOff + 20, lnOff
            += 40);
    g->drawMeter(xOff + 20, lnOff += 20, 256, 16, fmap(cam.scale, 0, 5, 0,
                                                       1));
    g->drawString("Pitch: " + std::to_string(cam.pitch * 180 / M_PI) + " deg",
                  2,
                  xOff + 20,
                  lnOff += 40);
    g->drawMeter(xOff + 20, lnOff += 20, 256, 16,
                 fmap(abs(cam.pitch) * 180 / M_PI, 0,
                      360, 0,
                      1));
    g->drawString("Yaw: " + std::to_string(cam.yaw * 180 / M_PI) + " deg", 2,
                  xOff + 20,
                  lnOff += 40);
    g->drawMeter(xOff + 20, lnOff += 20, 256, 16,
                 fmap(abs(cam.yaw) * 180 / M_PI, 0,
                      360, 0,
                      1));
    g->drawString("Controls", 3, xOff + 10, lnOff += 40);
    g->drawString("Computation Type: C", 2, xOff +
                                            20,
                  lnOff += 30);
    g->drawString("Toggle Quads: Q", 2, xOff +
                                        20,
                  lnOff += 30);
    g->drawString("Decrease Step: [", 2, xOff +
                                         20,
                  lnOff += 30);
    g->drawString("Increase Step: ]", 2, xOff +
                                         20,
                  lnOff += 30);
    g->drawString("Pitch: W / S", 2, xOff +
                                     20,
                  lnOff += 30);
    g->drawString("Yaw: A / D", 2, xOff +
                                   20,
                  lnOff += 30);
    g->drawString("Zoom: Scroll", 2, xOff +
                                     20,
                  lnOff += 30);
    g->drawString("Reset View: Z", 2, xOff +
                                      20,
                  lnOff += 30);


}

float furthestBody(std::vector<Body *> &bodies) {
    float max = 0;
    for (auto b : bodies) {
        auto l = b->ld();
        if (l > max) max = l;
    }
    return max;
}

BHTree simulate(std::vector<Body *> &bodies, BHGraphics *g, float dt) {

    float inc = 16;
    float radius = (float) furthestBody(bodies) * 2;
    g->setRadius(radius);
    Quad *q = new Quad(0, 0, radius);


    BHTree bhTree = BHTree(q);

    for (auto body : bodies) {
        body->clearForce();
        bhTree.insert(*body);
    }
    if (brute) {
        for (auto body : bodies) {
            for (auto body2 : bodies) {
                if (body != body2) {
                    body->addForce(*body2);
                }
            }
        }

    } else {
        for (auto body : bodies) {
            bhTree.updateForce(body);
        }
    }


    for (auto body : bodies) {
        body->update(dt);
        body->draw(g);
    }

    g->drawCube(-radius / 2, -radius / 2, -radius / 2, radius, radius, radius);

    return bhTree;
}

int main() {
    std::vector<Body *> bodies;
    while (!std::cin.eof()) {
        double px, py, pz, vx, vy, vz, mass;
        std::cin >> px;
        std::cin >> py;
        std::cin >> pz;
        std::cin >> vx;
        std::cin >> vy;
        std::cin >> vz;
        std::cin >> mass;
        Body *b = new Body(px, py, pz, vx, vy, vz, 0, 0, 0, mass);
        bodies.push_back(b);
    }

    auto g = BHGraphics(1024, 1024);

    bool running = true;


    float dt = 0;

    float rotY = 0, rotP = 0, scale = 0.7;

    /* Count Frames Per second */
    Uint32 totalFrames = 0;
    Uint32 startTime = SDL_GetTicks();
    float lastFps = 0.0;

    while (running) {
        dt += step;


        if (totalFrames >= 120) {
            totalFrames = 0;
            startTime = SDL_GetTicks();
        }
        totalFrames++;

        g.clear();


        auto bhTree = simulate(bodies, &g, dt);

        if (showQuads) bhTree.draw(&g);

        float fps = (float) totalFrames / (float) ((float) (SDL_GetTicks() -
                                                            startTime) /
                                                   1000.0);
        updateStatistics(bodies, &g, dt, fps);
        g.render();
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEWHEEL:
                    scale = g.getCamera().scale;
                    scale += e.wheel.y * 0.01;
                    if (scale <= 0.001) scale = 0.001;
                    g.scaleCamera(scale);
                    break;
                case SDL_KEYDOWN:
                    //do whatever you want to do after a mouse button was pressed,
                    // e.g.:
                    switch (e.key.keysym.sym) {
                        case 'q':
                            showQuads = !showQuads;
                            break;
                        case ']':
                            step += 1;
                            break;
                        case '[':
                            step -= 1;
                            break;
                        case 'c':
                            brute = !brute;
                            break;
                        case 'a':
                            rotY += M_PI / 8;
                            break;
                        case 'd':
                            rotY -= M_PI / 8;
                            break;
                        case 'w':
                            rotP -= M_PI / 8;
                            break;
                        case 's':
                            rotP += M_PI / 8;
                            break;
                        case 'z':
                            rotY = 0;
                            rotP = 0;
                            scale = 1;
                            g.scaleCamera(scale);
                            break;
                        default:
                            break;
                    }
                    g.rotate(rotY, 0, rotP);
                    break;

            }
        }
    }

}


