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

struct BHConfig {
    float theta = 1.2;
    float dampening = M_PI;
    float constant = 6.67408E-11;
};

class BHTree {
private:
    BHTree *NW = nullptr, *NE = nullptr, *SW = nullptr, *SE = nullptr;
    BHTree *children[4];
    Quad *q = nullptr;
    Body b = Body();
    BHConfig bhCfg = BHConfig{};

    void rawInsert(Body body);

public:
    ~BHTree() {
        delete NW;
        delete NE;
        delete SW;
        delete SE;
        delete q;
    }

    void setConfig(BHConfig bh) {
        bhCfg = bh;
    }

    BHTree() = default;

    explicit BHTree(Quad *q) : q(q) {}

    void updateForce(Body *body);

    void draw(BHGraphics *bh) const;

    void insert(Body body);

    int countQuads();

};


#endif //BARNESHUT_BHTREE_H
