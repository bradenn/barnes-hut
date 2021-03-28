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
#include "BHTree.h"

float furthestBody(std::vector<Body *> &bodies) {
    float max = 0;
    for (auto b : bodies) {
        auto l = b->ld();
        if (l > max) max = l;
    }
    return max;
}

void Simulation::run() {
    /* Since the bodies tend to fling far a way and return, we can expand the
     * entire main quad to include every body. In some implementations, bodies
     * are removed when they leave the quad. */
    float width = (float) furthestBody(bodies) * 2;

    /* We recreate the main outer quad with the radius previously determined.
     * The center of the quad is set to (0, 0), meaning a quad of width of
     * 1024 will have a range of -512 to 512. */
    Quad *q = new Quad(0, 0, width);
    BHTree bhTree = BHTree(q);

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
                    body->addForce(*body2);
    }

    /* In the previous steps we only updated the body's force, now that they
     * are all complete, we can apply it. If we apply the force before we
     * complete all of the bodies, the bodies will have effectively been in
     * their own temporal frame. This method is sometimes used to emulate
     * gravity oon bodies at relativistic speeds, but we do not need to worry
     * about that here. Instead we calculate, apply, and render to simulate the
     * normal universe. */
    for (auto body : bodies)
        body->update(settings->stepSize);

    /* Since we are drawing the quads later, we can set the lastTree to the
     * current. We don't actually care about the tree after we render it, so
     * it is okay to let the stack clear it later. */
    lastTree = bhTree;

    /* Finally we render the current iteration. */
    render();
}

void Simulation::render() {

    /* Render each individual body */
    for (auto body : bodies)
        body->draw(graphics);

    /* Render Quads if specified */
    if(settings->renderQuads)
        lastTree.draw(graphics);

}
