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
#include "BHTree.h"
#include "TestManager.h"

string ftos(float in) {
    char *c = new char[9];
    sprintf(c, "%.2f", in);
    string str = string(c);
    delete[]c;
    return str;
}

float furthestBody(std::vector<Body *> &bodies) {
    float max = 0;
    for (auto b : bodies) {
        auto l = b->ld();
        if (l > max) max = l;
    }
    return max;
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
    graphics->centerFull(1024 - 32);
}

void Simulation::run() {
    while (running) {
        currentStep += settings->stepSize;

        simulate();
        handleEvents();
    }
}

void Simulation::simulate() {
    /* Since the bodies tend to fling far a way and return, we can expand the
     * entire main quad to include every body. In some implementations, bodies
     * are removed when they leave the quad. */
    float width = (float) furthestBody(bodies) * 2;
    settings->radius = width;

    /* We recreate the main outer quad with the radius previously determined.
     * The center of the quad is set to (0, 0), meaning a quad of width of
     * 1024 will have a range of -512 to 512. */
    Quad *q = new Quad(0, 0, width);
    BHTree bhTree = BHTree(q);
    bhTree.setConfig(*bhCfg);

    /* Before each computation, we clear the force from the previous
     * iteration. This ensures the bodies do not accelerate into oblivion.
     * Once the bodies are prepared, we add them into the newly sized Tree. */
    for (auto body : bodies) {
        body->clearForce();
        bhTree.insert(*body);
    }

    /* The simulation settings allow for the simulation to run in brute-force
     * n^2 mode. This features is used to demonstrate the necessity of a quad
     * tree. */
    if (settings->useBarnes) {
        /* We loop through each body and update it's force recursively
         * through the tree. */
        for (auto body : bodies)
            bhTree.updateForce(body);

    } else {
        /* In the brute force attempt, we have to calculate the force on each
         * body on every other body, one by one. */
        for (auto body : bodies)
            for (auto body2 : bodies)
                if (body != body2)
                    body->addForce(*body2, bhCfg->dampening, bhCfg->constant);
    }

    /* In the previous steps we only updated the body's force, now that they
     * are all complete, we can apply it. If we apply the force before we
     * complete all of the bodies, the bodies will have effectively been in
     * their own temporal frame. This method is sometimes used to emulate
     * gravity oon bodies at relativistic speeds, but we do not need to worry
     * about that here. Instead we calculate, apply, and render to simulate the
     * normal universe. */
    for (auto body : bodies)
        body->update(currentStep);

    /* Since we are drawing the quads later, we can set the lastTree to the
     * current. We don't actually care about the tree after we render it, so
     * it is okay to let the stack clear it later. */

    /* Finally we render the current iteration. */
    render(bhTree);
}

void Simulation::render(const BHTree &b) {

    /* Clean the window */
    graphics->clear();
    graphics->setRadius(settings->radius);

    /* Render each individual body */
    for (auto body : bodies)
        body->draw(graphics);

    /* Render Quads if specified */
    if (settings->renderQuads) {
        b.draw(graphics);
    }


    graphics->drawCube(-settings->radius / 2, -settings->radius / 2,
                       -settings->radius / 2, settings->radius,
                       settings->radius,
                       settings->radius);

    /* Render the side panel showing the statistics */
    showStats();

    /* Call render from the BHGraphics engine. */
    graphics->render();

}

void Simulation::handleEvents() {

    fps = (float) totalFrames /
          (float) ((float) (SDL_GetTicks() - startTime)
                   / 1000.0);

    if (totalFrames >= 120) {
        totalFrames = 0;
        startTime = SDL_GetTicks();
    }
    totalFrames++;

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
                        break;
                    case 'q':
                        settings->renderQuads = !settings->renderQuads;
                        break;
                    case 'i':
                        settings->showControls = !settings->showControls;
                        break;
                    case ']':
                        settings->stepSize += 1;
                        break;
                    case '[':
                        settings->stepSize -= 1;
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
                    case 'a':
                        settings->rotY += M_PI / 8;
                        break;
                    case 'd':
                        settings->rotY -= M_PI / 8;
                        break;
                    case 'w':
                        settings->rotP -= M_PI / 8;
                        break;
                    case 's':
                        settings->rotP += M_PI / 8;
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
                graphics->rotate(settings->rotY, 0, settings->rotP);
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
    graphics->drawString("N-Bodies", 3, xOff, lnOff);
    graphics->drawString("Statistics", 2, xOff + 10, lnOff += 40);
    graphics->setColor(128, 128, 128);

    graphics->drawString("Step Size: " + ftos(settings->stepSize),
                         1.5, xOff +
                              20,
                         lnOff += 25);
    graphics->drawString("Subjective Time: " + ftos(currentStep), 1.5, xOff +
                                                                       20,
                         lnOff += 20);
    graphics->drawString(
            "Relative Time: " + ftos(currentStep / settings->stepSize), 1.5,
            xOff + 20,
            lnOff += 20);

    graphics->drawString("Remaining Bodies: " + ftos(bodies.size()), 1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->setColor(200, 200, 200);
    graphics->drawString("Rendering", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString("Frame Time: " + ftos((1000.0 / fps)
                         ) + "ms",
                         1.5, xOff +
                              20,
                         lnOff += 25);
    graphics->drawString("FPS: " + ftos(fps) + "fps ",
                         1.5,
                         xOff +
                         20,
                         lnOff += 20);
    graphics->setColor(200, 200, 200);
    graphics->drawString("Computation", 2, xOff + 10, lnOff += 30);
    graphics->setColor(128, 128, 128);
    graphics->drawString(
            "Calculation Type: " + string((!settings->useBarnes) ? "Brute" :
                                          "Barnes-Hut"), 1.5,
            xOff + 20,
            lnOff += 25);
    graphics->drawString("Barnes-Hut", 2, xOff + 10, lnOff += 30);
    graphics->drawString("Outer Quad: " + ftos(cam.radius), 1.5, xOff + 20,
                         lnOff += 25);
    graphics->drawString(
            "Theta: " + ftos(bhCfg->theta), 1.5,
            xOff + 20,
            lnOff += 20);
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

    graphics->drawString("Pitch: " + ftos(cam.pitch * 180 / M_PI) + " deg",
                         1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
                        fmap(abs(cam.pitch) * 180 / M_PI, 0,
                             360, 0,
                             1));

    graphics->drawString("Yaw: " + ftos(cam.yaw * 180 / M_PI) + " deg", 1.5,
                         xOff + 20,
                         lnOff += 20);
    graphics->drawMeter(xOff + 20, lnOff += 16, 200, 6,
                        fmap(abs(cam.yaw) * 180 / M_PI, 0,
                             360, 0,
                             1));
    graphics->setColor(200, 200, 200);
    graphics->drawString("Tests", 2, xOff + 10, lnOff += 30);
    float ptOff = 0;
    lnOff += 5;
    if(testManager->getTests().size() >= 1){

    for (auto test : testManager->getTests()) {
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
    }else{
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
    graphics->drawString("Made by Braden Nicholson - 2021", 1.5, xOff,
                         1024 - 8);
    graphics->drawString("https://bradenn.com", 1.5, xOff,
                         1024 + 8);

}


