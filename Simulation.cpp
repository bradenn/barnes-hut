/* *
 * @project barnesHut
 * @file simulation.cpp.cc
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/28/21
 */

#include "Simulation.h"

#include <utility>
#include <unistd.h>
#include <math.h>
#include "BHTree.h"
#include "TestManager.h"
#include "omp.h"

//double getTime() {
//    struct timespec s;
//    clock_gettime(CLOCK_MONOTONIC_RAW, &s);
//    double seconds = s.tv_sec * 1000.0 * 1000.0; // Seconds to Microseconds
//    double nanoseconds = s.tv_nsec / 1000.0; // Nanoseconds to Microseconds
//    return (seconds + nanoseconds) / 1000.0;
//}

double getTime() {
    return omp_get_wtime() * 1000.0;
}


string ftos(float in) {
    char *c = new char[9];
    sprintf(c, "%.2f", in);
    string str = string(c);
    delete[]c;
    return str;
}

float furthestBody(std::vector<Body *> &bodies) {
    float sum = 0;
    float largest = 0;
    auto ref = Body(0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    for (auto b: bodies) {
        ref.plus(*b);
        largest = (b->ld() > largest) ? b->ld() : largest;
    }
    int candidates = 0;
    for (auto b: bodies) {
        auto dist = b->distance(ref);
        sum += dist;
        candidates++;

    }

    return sum / (float) candidates;
}

float fmap(float value,
           float istart,
           float istop,
           float ostart,
           float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

Simulation::Simulation() {
    graphics = new BHGraphics(1024 + 256 + 16, 1024 + 48);
    testManager = new TestManager(&bodies);

    settings->maxThreads = omp_get_max_threads();
    settings->numThreads = settings->maxThreads > 2 ? 3 : 1;

    graphics->centerFull(1024 - 32);
}


void Simulation::run() {
    while (running) {
        currentStep += settings->stepSize;

        double tickDelta = 0.0;
        for (int i = 0; i < 10; i++) {
            tickBegin = getTime();
            simulate();
            tickDelta = (getTime() - tickBegin);
        }


        ups = (float) (1000 / tickDelta);
        settings->numThreads = settings->maxThreads;

        double renderBegin = (getTime());

        /* Finally we render the current iteration. */
        render();

        double renderDelta = getTime() - renderBegin;

        fps = (float) (1000.0 / renderDelta);

        handleEvents();
    }
}

void Simulation::simulate() {


    /* Since the bodies tend to fling far a way and return, we can expand the
     * entire main quad to include every body. In some implementations, bodies
     * are removed when they leave the quad. h(1024 - 32) * 2 */
    float width = (float) furthestBody(bodies) * 2;
    settings->radius = width;


    /* The simulation settings allow for the simulation to run in brute-force
     * n^2 mode. This features is used to demonstrate the necessity of a quad
     * tree. */
#pragma omp parallel for default(none) shared(bodies, bhCfg) num_threads(settings->maxThreads) if(settings->useParallel)
    for (auto to: bodies)
        for (auto from: bodies)
            if (to != from) {
                to->handleCollision(*from);
                to->addForce(*from, bhCfg->dampening, bhCfg->constant);
            }

    /* In the previous steps we only updated the body's force, now that they
     * are all complete, we can apply it. If we apply the force before we
     * complete all of the bodies, the bodies will have effectively been in
     * their own temporal frame. This method is sometimes used to emulate
     * gravity oon bodies at relativistic speeds, but we do not need to worry
     * about that here. Instead we calculate, apply, and render to simulate the
     * normal universe. */
#pragma omp parallel for default(none) shared(bodies, currentStep) num_threads(settings->maxThreads) if(settings->useParallel)
    for (auto body: bodies) {
        body->update(currentStep);
    }


    for (auto body: bodies) {
        body->clearForce();
    }
}

struct MinMax {
    float max;
    float min;
};


void Simulation::render() {

    /* Clean the window */
    graphics->clear();
    graphics->setRadius(settings->radius);

    float maxMass = 0;


    for (auto body: bodies) {
//        mm.max = mm.max < body->avgForce() ? body->avgForce() : mm.max;
//        mm.min = mm.min > body->avgForce() ? body->avgForce() : mm.min;
        maxMass = body->getMass() > maxMass ? body->getMass() : maxMass;
    }

    graphics->beginSimulationFrame();
    /* Render each individual body */
    for (auto body: bodies) {
        body->draw(graphics, 0, 0, maxMass);
    }

    graphics->setAlphaColor(128, 128, 128, 128);
    graphics->drawOrigin(0, 0, 0, settings->radius, settings->radius, settings->radius);
//    graphics->drawCube(-settings->radius / 2, -settings->radius / 2,
//                       -settings->radius / 2, settings->radius,
//                       settings->radius,
//                       settings->radius);

    graphics->endSimulationFrame();



    /* Render the side panel showing the statistics */
    showStats();

    /* Call render from the BHGraphics engine. */
    graphics->render();

}

void Simulation::handleEvents() {

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEWHEEL:
                settings->scale = graphics->getCamera().scale;
                settings->scale += e.wheel.y * (float) 0.01;
                if (settings->scale <= 0.001) settings->scale = 0.001;
                graphics->scaleCamera(settings->scale);
                break;
            case SDL_KEYDOWN:
                //do whatever you want to do after a mouse button was pressed,
                // e.g.:
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        testManager->lastTest();
                        break;
                    case SDLK_DOWN:
                        testManager->nextTest();
                        break;
                    case SDLK_RETURN:
                        testManager->selectTest();
                        currentStep = 0;

                        break;
                    case 'q':
                        settings->renderQuads = !settings->renderQuads;
                        break;
                    case 'i':
                        settings->showControls = !settings->showControls;
                        break;
                    case ']':
                        settings->stepSize += 0.25;
                        break;
                    case '[':
                        settings->stepSize -= 0.25;
                        break;
                    case ',':
                        bhCfg->theta += 0.1;
                        break;
                    case '.':
                        bhCfg->theta -= 0.1;
                        break;
                    case 'c':
                        settings->useBarnes = !settings->useBarnes;
                        break;
                    case 'p':
                        settings->useParallel = !settings->useParallel;
                        break;
                    case 'a':
                        settings->rotY += 22.5;
                        break;
                    case 'd':
                        settings->rotY -= 22.5;
                        break;
                    case 'w':
                        settings->rotP -= 22.5;
                        break;
                    case 's':
                        settings->rotP += 22.5;
                        break;
                    case 'z':
                        settings->rotY = 0;
                        settings->rotP = 0;
                        settings->scale = 1;
                        graphics->centerFull(1024 - 32);
                        break;
                    default:
                        break;
                }
                graphics->rotate(settings->rotY, settings->rotP,0);
                break;
        }
    }
}

void Simulation::showStats() {
    auto cam = graphics->getCamera();
    float xPad = 0, yPad = 8;
    float xOff = 1024 + xPad, yOff = 32;
    float lnOff = yOff;
    graphics->setAlphaColor(32, 32, 32, 128);
    graphics->fillRect(xOff - xPad * 2, yPad * 2, 256 - xPad, 1024 + 32 + 16 -
                                                              yPad *
                                                              4);
    graphics->setColor(200, 200, 200);
    xOff += 16;
    graphics->drawString("N-Bodies 2", 3, xOff, lnOff);
    graphics->drawString("Statistics", 2, xOff + 10, lnOff += 40);
    graphics->setColor(128, 128, 128);

    graphics->drawString("Step Size: " + ftos(settings->stepSize),
                         1.5, xOff +
                              20,
                         lnOff += 25);
    graphics->drawString("Relative Step: " + ftos(currentStep), 1.5, xOff +
                                                                     20,
                         lnOff += 20);
    graphics->drawString(
            "Total Steps: " + ftos(currentStep / settings->stepSize), 1.5,
            xOff + 20,
            lnOff += 20);

    graphics->drawString("Remaining Bodies: " + ftos(bodies.size()), 1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->setColor(200, 200, 200);
    graphics->drawString("Rendering", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString("Update Time: " + ftos((float) (((ups > 0) ? 1000.0 / ups : (1000.0 / 60.0)))
                         ) + "ms",
                         1.5, xOff +
                              20,
                         lnOff += 25);
    graphics->drawString("Frame Time: " + ftos((float) (((fps > 0) ? 1000.0 / fps : (1000.0 / 60.0)))
                         ) + "ms",
                         1.5, xOff +
                              20,
                         lnOff += 25);
    graphics->drawString("FPS: " + ftos((fps > 0) ? fps : 60.0) + "fps",
                         1.5,
                         xOff +
                         20,
                         lnOff += 20);
    graphics->drawString("UPS: " + ftos((ups > 0) ? ups : 60.0) + "ups",
                         1.5,
                         xOff +
                         20,
                         lnOff += 20);
    graphics->drawString("RUNS: " + ftos(floor(ups / fps)),
                         1.5,
                         xOff +
                         20,
                         lnOff += 20);


    graphics->setColor(200, 200, 200);
    graphics->drawString("Computation", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString(
            "Parallel: " + string((settings->useParallel) ? "Parallel" :
                                  "Sequential"), 1.5,
            xOff + 20,
            lnOff += 25);
    graphics->drawString(
            "Cores: " + string(std::to_string(settings->numThreads)) + "/" +
            string(std::to_string(settings->maxThreads)), 1.5,
            xOff + 20,
            lnOff += 20);
    lnOff += 16;
    double a = 200.0 / (settings->maxThreads);
    double b = 180.0 / (settings->maxThreads);

    for (int i = 0; i < settings->maxThreads; ++i) {
        if (settings->numThreads > i) {
            graphics->setColor(128, 128, 128);
            graphics->fillRect(xOff + 20 + (float) i * a, lnOff, b, 6);
        } else {
            graphics->setColor(96, 96, 96);
            graphics->strokeRect(xOff + 20 + (float) i * a, lnOff, b, 6);
        }
    }

//    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
//                        fmap(((float) settings->numThreads / (float) settings->maxThreads) * 100, 0,
//                             100, 0,
//                             1));
    graphics->setColor(200, 200, 200);
    graphics->drawString("Barnes-Hut", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString("Outer Container: " + ftos(cam.radius), 1.5, xOff + 20,
                         lnOff += 25);
    graphics->drawString(
            "Dampening: " + ftos(bhCfg->dampening), 1.5,
            xOff + 20,
            lnOff += 20);
    graphics->setColor(200, 200, 200);
//    graphics->drawString("Resources", 2, xOff + 10, lnOff += 40);
//    graphics->setColor(128, 128, 128);
//    struct rusage r_usage;
//    getrusage(RUSAGE_SELF, &r_usage);
//    graphics->drawString("MaxRSS: " + ftos(r_usage.ru_maxrss / (1024 *
//                                                                1024)) +
//                         " Mb", 1.5, xOff + 20,
//                         lnOff += 25);
//    graphics->setColor(200, 200, 200);
    graphics->drawString("Camera", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString("Scale: " + ftos(cam.scale), 1.5, xOff + 20, lnOff
            += 25);
    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
                        fmap(cam.scale, 0, cam.scale * 2, 0,
                             1));

    graphics->drawString("Pitch: " + ftos(cam.pitch) + " deg",
                         1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
                        fmap(cam.pitch, 0,
                             360, 0,
                             1));

    graphics->drawString("Yaw: " + ftos(cam.yaw) + " deg", 1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
                        fmap(cam.yaw, 0,
                             360, 0,
                             1));
    graphics->setColor(200, 200, 200);
    graphics->drawString("Tests", 2, xOff + 10, lnOff += 30);
    float ptOff = 0;
    lnOff += 5;
    if (testManager->getTests().size() >= 1) {
        for (auto test: testManager->getTests()) {
            if (test == testManager->getTest()) {
                graphics->setAlphaColor(255, 200, 64, 255);
                graphics->drawString(test->name, 1.5, xOff + 20, lnOff += 20);
            } else {
                graphics->setColor(128, 128, 128);
                graphics->drawString(test->name, 1.5, xOff + 20, lnOff += 20);
            }
            if (test == testManager->getSelected()) {
                graphics->setAlphaColor(200, 96, 64, 255);
                graphics->drawCircle(xOff + 12, lnOff + 6, 3);

            }
            ptOff++;
        }
    } else {
        graphics->setAlphaColor(255, 64, 64, 255);
        graphics->drawString("'tests'  directory  not  found.", 1.5, xOff + 20,
                             lnOff +=
                                     20);
    }
    graphics->setAlphaColor(32, 32, 32, 128);
    graphics->fillRect(16, 1024, 1024 - 32, 32);
    graphics->setColor(128, 128, 128);
    float hOff = 24;
    float vCenter = 1034;

    hOff += graphics->drawStringGetLength("Toggle Method: C", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Show Quads: Q", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Pitch: W / S", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Yaw: A / D", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Reset View: Z", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Zoom: Scroll", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Cycle Tests: Arrow Up/Down", 1.5,
                                          hOff, vCenter);
    hOff += graphics->drawStringGetLength("Select Test: Enter", 1.5,
                                          hOff, vCenter);


    graphics->setColor(128, 128, 128);
    graphics->drawString("Made by Braden Nicholson - 2021/2022", 1.5, xOff,
                         1024 - 8);
    graphics->drawString("https://bradenn.com", 1.5, xOff,
                         1024 + 8);

}



//double overflow = 0;
//double underflow = 0;
//
//
//
//

//        if (ups < settings->targetFps * 0.925) {
//            // If the FPS is below 60 for more than 2 seconds, add a new core
//            if (underflow == 0) {
//                underflow = getTime();
//            } else if (getTime() - underflow >= 1500) {
//                settings->numThreads =
//                        settings->maxThreads > settings->numThreads ? settings->numThreads + 1 : settings->numThreads;
//                underflow = 0;
//            }
//        } else if (ups > settings->targetFps * 1.5) {
//            // If the FPS is above 90 for more than 2 seconds, remove a core
//            if (overflow == 0 && settings->numThreads > 2) {
//                overflow = getTime();
//            } else if (getTime() - overflow >= 1500) {
//                settings->numThreads = 1 < settings->numThreads - 1 ? settings->numThreads - 1 : settings->numThreads;
//                overflow = 0;
//            }
//        }