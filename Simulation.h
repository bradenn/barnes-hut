/* *
 * @project barnesHut
 * @file simulation.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/28/21
 */

#ifndef BARNESHUT_SIMULATION_H
#define BARNESHUT_SIMULATION_H

#include "Body.h"
#include "BHTree.h"
#include "TestManager.h"
#include <utility>
#include <vector>

using std::vector;

struct Settings {
    bool useBarnes = true;
    bool renderQuads = false;
    bool useParallel = true;
    int targetFps = 120;
    int numThreads = 1;
    int maxThreads = 1;
    float radius = 0;
    float scale = 1;
    float stepSize = 0.25;
    bool showControls = false;
    float rotP{}, rotY{};
};


class Simulation {
private:
    vector<Body *> bodies;
    BHGraphics *graphics;
    Settings *settings = new Settings;
    BHConfig *bhCfg = new BHConfig;
    TestManager *testManager;

    float currentStep = 0.0;
    bool running = true;

    Uint32 totalFrames = 0;
    double startTime = SDL_GetTicks();
    float fps = 0.0;

    void simulate();

    void showStats();

    void handleEvents();

    void render(const BHTree &b);

public:
    Simulation();

    void run();

};


#endif //BARNESHUT_SIMULATION_H
