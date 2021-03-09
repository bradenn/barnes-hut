#include <iostream>
#include "BHTree.h"
#include "BHGraphics.h"
#include "Quad.h"
#include <SDL.h>
#include <algorithm>
#include <vector>


BHTree simulate(std::vector<Body *> &bodies, BHGraphics *g, double dt) {
    Quad *q = new Quad(512, 512, 1024);

    BHTree bhTree = BHTree(q);
    for (int i = 0; i < bodies.size(); i++) {
        auto body = bodies[i];
        if (!body->in(q)) {
            bodies.erase(bodies.begin() + i);
        }
        body->dampenInertia();
        bhTree.insert(*body);
    }

//     Calculate forces, the actual algorithm...
    for (auto body : bodies) {
        bhTree.updateForce(body);
    }

    for (auto body : bodies) {
        body->update(dt);
    }

    for (auto body : bodies) {
        body->draw(g);
    }

//    bhTree.draw(g);

    return bhTree;
}


int main() {
    std::vector<Body *> bodies;
    while (!std::cin.eof()) {
        double px, py, vx, vy, mass;
        std::cin >> px;
        std::cin >> py;
        std::cin >> vx;
        std::cin >> vy;
        std::cin >> mass;
        Body *b = new Body(px, py, vx, vy, 0, 0, mass);
        bodies.push_back(b);
    }

    auto g = BHGraphics(1024, 1048);

    bool running = true;
    double dt = 0;
    while (running) {
        dt += 1;
        g.clear();
        SDL_Event e;
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0) // poll for event
        {
            //User requests quit
            if (e.type == SDL_QUIT) // unless player manually quits
            {
                running = false;
            }
        }

        simulate(bodies, &g, dt);


        g.render();
    }

}
