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

    for (int i = 0; i < NODES; ++i) {
        if (children[i] != nullptr) {
            children[i]->draw(bh);
        }
    }
}


void BHTree::insert(Body body) {

    if (b.isSetDefault()) {
        b = body;
    } else {
        for (int i = 0; i < NODES; ++i) {
            if (children[i] == nullptr) {
                children[i] = new BHTree(q->SubTree(i));
            }
        }

        if (!b.isSetInternal()) {
            rawInsert(b);
        }
        b = b.plus(body);

        rawInsert(body);
    }
}

void BHTree::rawInsert(Body body) {
    b.setInternal();

    for (int i = 0; i < NODES; ++i) {
        if (children[i] != nullptr) {
            if (body.in(children[i]->q)) {
                children[i]->insert(body);
            }
        }
    }

}

void BHTree::updateForce(Body *body) {
    if (!b.isSetDefault()) {
        if (!b.isSetInternal()) {
            if (b.getHash() != body->getHash()) {
                body->addForce(b, bhCfg.dampening, bhCfg.constant);
            }
        } else {
            double sd = q->length() / (*body).distance(b);
            if (sd < bhCfg.theta) {
                body->addForce(b, bhCfg.dampening, bhCfg.constant);
            } else {

                for (auto &i: children) {
                    if (i != nullptr) {
                        if (body->in(i->q)) {
                            i->updateForce(body);
                        }
                    }
                }

            }
        }
    }
}

int BHTree::countQuads() {
    int total = 1;

    return total;
}

BHTree::BHTree(Quad *q) : q(q) {
    for (auto & i : children) {
        i = nullptr;
    }
}
