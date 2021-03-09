/* *
 * @project barnesHut
 * @file Quad.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/4/21
 */

#ifndef BARNESHUT_QUAD_H
#define BARNESHUT_QUAD_H


#include "BHGraphics.h"

class Quad {
private:
    double xm, ym, l;
public:
    Quad(double xm, double ym, double l) : xm(xm), ym(ym), l(l) {}

    // This is not the wey
    bool contains(double x, double y) const {
        double xs = xm - l / 2;
        double xe = xm + l / 2;
        double ys = ym - l / 2;
        double ye = ym + l / 2;
        return (x >= xs && x < xe && y >= ys && y < ye);
    }

    void draw(BHGraphics *bh) const{
        double sx = xm - l/2;
        double sy = ym - l/2;
        bh->drawRect(sx, sy, l, l);
    }

    double length() const {
        return l;
    }

    Quad *NW() const {
        return new Quad(xm - l / 4, ym - l / 4, l / 2);
    }

    Quad *NE() const {
        return new Quad(xm + l / 4, ym - l / 4, l / 2);
    }

    Quad *SW() const {
        return new Quad(xm - l / 4, ym + l / 4, l / 2);
    }

    Quad *SE() const {
        return new Quad(xm + l / 4, ym + l / 4, l / 2);
    }
};


#endif //BARNESHUT_QUAD_H
