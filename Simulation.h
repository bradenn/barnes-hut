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
    bool useBarnes = false;
    bool renderQuads = false;
    bool useParallel = true;
    int targetFps = 60;
    int numThreads = 1;
    int maxThreads = 1;
    float radius = 0;
    float scale = 1;
    float stepSize = 1;
    bool showControls = false;
    float rotP = 45, rotY = 45;
};


class Simulation {
private:
    vector<Body *> bodies;
    BHGraphics *graphics;
    Settings *settings = new Settings;
    BHConfig *bhCfg = new BHConfig;
    TestManager *testManager;

    double tickBegin = 0.0;
    float currentStep = 0.0;
    bool running = true;
    float fps = 0.0;
    float ups = 0.0;

    void simulate();

    void showStats();

    void handleEvents();

    static float fmap(float value,
                      float istart,
                      float istop,
                      float ostart,
                      float ostop) {
        if (value < istart) return 0;
        if (value > istop) return istop / 2;
        return ostart +
               (ostop - ostart) * ((value - istart) / (istop - istart));
    }
public:
    Simulation();

    void run();

    void render();
};


#endif //BARNESHUT_SIMULATION_H
