/* *
 * @project barnesHut
 * @file BHTree.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/4/21
 */

#ifndef BARNESHUT_BHTREE_H
#define BARNESHUT_BHTREE_H

#include <iostream>
#include "Body.h"
#include "Quad.h"
#include "BHGraphics.h"

#define NODES 8

struct BHConfig {
    float theta = 1.4;
    float dampening = 1;
    float constant = 6.67408E-11;
};

class BHTree {
private:
    BHTree *children[NODES];
    Quad *q = nullptr;
    Body b = Body();
    BHConfig bhCfg = BHConfig{};

    void rawInsert(Body body);

public:
    ~BHTree() {
        for (auto &item: children) {
            delete item;
        }
        delete q;
    }

    void setConfig(BHConfig bh) {
        bhCfg = bh;
    }

    BHTree() = default;

    explicit BHTree(Quad *q);

    void updateForce(Body *body);

    void draw(BHGraphics *bh) const;

    void insert(Body body);

    int countQuads();

};


#endif //BARNESHUT_BHTREE_H
