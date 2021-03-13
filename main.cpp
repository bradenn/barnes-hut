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
float fmap(float value,
           float istart,
           float istop,
           float ostart,
           float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}
void updateStatistics(std::vector<Body *> &bodies, BHGraphics *g, float dt,
                      float dF) {
    float xPad = 12, yPad = 8;
    float xOff = 1024 + xPad, yOff = 32;
    float lnOff = yOff;
    g->drawString("N-Bodies", 4, xOff, lnOff);
    g->drawString("Statistics", 3, xOff + 10, lnOff += 50);
    g->drawString("Step Size: " + std::to_string(step), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Subjective Time: " + std::to_string(dt), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Relative Time: " + std::to_string(dt / step), 2, xOff + 20,
                  lnOff += 30);
    g->drawString("Remaining Bodies: " + std::to_string(bodies.size()), 2,
                  xOff + 20,
                  lnOff += 30);
    g->drawString("Rendering", 3, xOff + 10, lnOff += 40);
    g->drawString("Frame Time: " + std::to_string((int) dF) + "ms", 2, xOff +
                                                                       20,
                  lnOff += 30);
    g->drawString("FPS: " + std::to_string((int) (1000 / dF)) + "fps", 2, xOff +
                                                                          20,
                  lnOff += 30);
    g->drawString("Resources", 3, xOff + 10, lnOff += 40);
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    g->drawString("MaxRSS: " + std::to_string(r_usage.ru_maxrss / (1024 *
                                                                   1024)) +
                  " Mb", 2, xOff + 20,
                  lnOff += 30);

    g->drawString("Controls", 3, xOff + 10, lnOff += 40);
    g->drawString("Inject Bodies: I", 2, xOff +
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

BHTree simulate(std::vector<Body *> &bodies, BHGraphics *g, float dt) {
    Quad *q = new Quad(512, 512, 1024);

    BHTree bhTree = BHTree(q);
    for (int i = 0; i < bodies.size(); i++) {
        auto body = bodies[i];
        if (!body->in(q)) {
            body->repluse();
//            bodies.erase(bodies.begin() + i);
//            delete body;
//            continue;
        }
        body->dampenInertia();
        bhTree.insert(*body);
    }

    for (auto body : bodies) {
        bhTree.updateForce(body);
        body->update(dt);
    }
//    g->drawCube(0, 0, 0, 1024, 1024, 1024);

    return bhTree;
}

void injectBodies(std::vector<Body *> &bodies) {
    for (int i = 0; i <= 24; i++) {
        float r = (float) ((float) (rand() % 200) / 100) - 1;
        bodies.push_back(new Body(1024, 512, 512, -1e-3, 1e-3 * r, 0, 0,
                                  0, 0,
                                  1000000));
    }
}

void injectBodiesAtPoint(std::vector<Body *> &bodies, float x, float y) {
    bodies.push_back(new Body(x, y, 512, 0, 0, 0, 0,
                              0, 0,
                              1000000));
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
    float oldTime = SDL_GetTicks();
    float newTime = 0;
    float rotY = 0, rotP = 0, scale = 0.7;
    float dF = 0;
    while (running) {
        dt += step;

        g.clear();
        SDL_Event e;
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0) // poll for event
        {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    injectBodiesAtPoint(bodies, e.button.x, e.button.y);
                    break;
                case SDL_MOUSEWHEEL:
                    scale += e.wheel.y * 0.1;
                    if (scale <= 0.5) scale = 0.5;
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
                        case 'a':
                            rotY += M_PI_4/16;
                            break;
                        case 'd':
                            rotY -= M_PI_4/16;
                            break;
                        case 'w':
                            rotP -= M_PI_4/16;
                            break;
                        case 's':
                            rotP += M_PI_4/16;
                            break;
                        case 'z':
                            rotY = 0;
                            rotP = 0;
                            scale = 0.7;
                            g.scaleCamera(scale);
                            break;
                        case 'i':
                            injectBodies(bodies);
                            break;
                        default:
                            break;
                    }
                    g.rotate(rotY, 0, rotP);
                    break;

            }
        }

        oldTime = newTime;
        newTime = SDL_GetTicks();

        auto bhTree = simulate(bodies, &g, dt);
        if ((int) newTime % 10 == 0) {
            dF = (newTime - oldTime);
        }

        g.drawCube(0, 0, 0, 1024, 1024, 1024);
        float xPad = 12, yPad = 8;
        float xOff = 1024 + 48 + xPad, yOff = 1024 - 256;
        float lnOff = yOff;
        g.drawString("Scale: " + std::to_string(scale), 2, xOff, lnOff);
        g.drawMeter(xOff, lnOff+=20, 256, 32, fmap(scale, 0.5, 20, 0, 1));
        g.drawString("Pitch: " + std::to_string(rotP*180/M_PI) + " deg", 2,
                     xOff,
                     lnOff+=50);
        g.drawMeter(xOff, lnOff+=20, 256, 32, fmap(abs(rotP)*180/M_PI, 0,
                                                   360, 0,
                                                   1));
        g.drawString("Yaw: " + std::to_string(rotY*180/M_PI) + " deg", 2,
                     xOff,
                     lnOff+=50);
        g.drawMeter(xOff, lnOff+=20, 256, 32, fmap(abs(rotY)*180/M_PI, 0,
                                                   360, 0,
                                                   1));

        for (auto body : bodies) {
            body->draw(&g);
        }
        if (showQuads) bhTree.draw(&g);
        updateStatistics(bodies, &g, dt, dF);

        g.render();
    }

}


