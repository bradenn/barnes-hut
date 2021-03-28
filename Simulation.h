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
#include <utility>
#include <vector>
using std::vector;

struct Settings {
    bool useBarnes = true;
    bool renderQuads = false;
    float stepSize = 1;
    float theta = 0.9;
    float dampening = 2E-2;
};

class Simulation {
private:
    vector<Body*> bodies;
    BHGraphics *graphics;
    BHTree lastTree;
public:
    Simulation(vector<Body*> b): bodies(std::move(b)) {}
    Settings *settings;
    void run();
    void render();

};


#endif //BARNESHUT_SIMULATION_H
