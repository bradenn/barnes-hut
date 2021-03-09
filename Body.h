/* *
 * @project barnesHut
 * @file Body.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/4/21
 */

#ifndef BARNESHUT_BODY_H
#define BARNESHUT_BODY_H

#include<cmath>
#include "Quad.h"

/*
 * Basic Structure inspired by this:
 * ftp://ftp.cs.princeton.edu/pub/cs126/barnes-hut/Body.java
 */

#include<iostream>
// Stack overflow: https://stackoverflow.com/questions/17134839/how-does-the-map-function-in-processing-work


class Body {
private:
    double px = 0, py = 0, vx = 0, vy = 0, fx = 0, fy = 0;
    double mass = 0;
    bool isDefault = true;
    bool internal = true;

public:
    void update(double dt) {
        vx += dt * fx / mass;
        vy += dt * fy / mass;
        px += dt * vx;
        py += dt * vy;
    }

    void draw(BHGraphics *bh) const{
        bh->drawPixel(px, py);
    }

    double distance(Body b) const {
        double dx = px - b.px;
        double dy = py - b.py;
        return sqrt(dx * dx + dy * dy);
    }

    void print() {
        std::cout << "_____________" << std::endl;
        std::cout << px << ", " << py << std::endl;
        std::cout << vx << ", " << vy << std::endl;
        std::cout << fx << ", " << fy << std::endl;
        std::cout << "_____________" << std::endl;
    }

    void dampenInertia() {
        fx = 0.0;
        fy = 0.0;
    }

    void addForce(Body b) {
        double G = 6.67e-11; // Gravitational Constant
        double DAMP = 3E1; // Dampening

        double dx = b.px - px;
        double dy = b.py - py;
        double ds = sqrt(dx * dx + dy * dy);

        double df = (G * mass * b.mass) / (ds * ds + DAMP * DAMP);
        fx += df * (dx / ds);
        fy += df * (dy / ds);
    }

    bool in(Quad *q) const {
        return q->contains(px, py);
    }

    bool isSetDefault() const {
        return isDefault;
    }

    bool isSetInternal() {
        return internal;
    }

    void setInternal() {
        internal = true;
    }

    double getHash() const {
        return px + py + vx + vy;
    }


    Body plus(Body b) const {
        double ms, nx, ny;
        ms = mass + b.mass;
        nx = (px * mass + b.px * b.mass) / ms;
        ny = (py * mass + b.py * b.mass) / ms;

        return {nx, ny, 0, 0, 0, 0, ms};
    }


    Body() = default;

    Body(double px, double py, double vx, double vy, double fx, double fy,
         double mass) : px(px), py(py), vx(vx), vy(vy), fx(fx), fy(fy), mass
            (mass) {
        isDefault = false;
        internal = false;
    }
};


#endif //BARNESHUT_BODY_H
