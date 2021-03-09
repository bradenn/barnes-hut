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
public:
    BHTree *NW = nullptr, *NE = nullptr, *SW = nullptr, *SE = nullptr;
    Quad *q = nullptr;
    Body *b = nullptr;

    explicit BHTree(Quad *q) : q(q) {}

    void updateForce(Body *body);

    void draw(BHGraphics *bh) const;

    void insert(Body *body);
};


#endif //BARNESHUT_BHTREE_H
