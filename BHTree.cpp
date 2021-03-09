/* *
 * @project barnesHut
 * @file BHTree.cpp.cc
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/4/21
 */

#include "BHTree.h"

void BHTree::draw(BHGraphics *bh) const {
    if (q != nullptr) {
        q->draw(bh);
    }
    if (NW != nullptr) NW->draw(bh);
    if (NE != nullptr) NE->draw(bh);
    if (SW != nullptr) SW->draw(bh);
    if (SE != nullptr) SE->draw(bh);
}

void BHTree::insert(Body *body) {
    if (b == nullptr) {
        b = body;
    } else {
        b->addForce(*body);
        if (NW == nullptr) NW = new BHTree(q->NW());
        if (NE == nullptr) NE = new BHTree(q->NE());
        if (SW == nullptr) SW = new BHTree(q->SW());
        if (SE == nullptr) SE = new BHTree(q->SE());

        if (body->in(NW->q)) {
            NW->insert(body);
        } else if (body->in(NE->q)) {
            NE->insert(body);
        } else if (body->in(SW->q)) {
            SW->insert(body);
        } else if (body->in(SE->q)) {
            SE->insert(body);
        }
    }
}

void BHTree::updateForce(Body *body) {

}
