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

#include<iostream>
#include "Body.h"
#include "Quad.h"
#include "BHGraphics.h"

class BHTree {
private:
    BHTree *NW = nullptr, *NE = nullptr, *SW = nullptr, *SE = nullptr;
    Quad *q = nullptr;
    bool internal = false;
    Body b = Body();

    void rawInsert(Body body);


public:
    ~BHTree() {
        delete NW;
        delete NE;
        delete SW;
        delete SE;
        delete q;
    }
    BHTree() = default;
    explicit BHTree(Quad *q) : q(q) {}

    void updateForce(Body *body);

    void draw(BHGraphics *bh) const;

    void insert(Body body);

};


#endif //BARNESHUT_BHTREE_H
