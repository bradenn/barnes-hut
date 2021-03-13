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
    double px = 0, py = 0, pz = 0, vx = 0, vy = 0, vz = 0, fx = 0, fy = 0, fz
            = 0;
    double mass = 0;
    bool isDefault = true;
    bool internal = true;

public:
    void update(double dt) {
        vx += dt * fx / mass;
        vy += dt * fy / mass;
        vz += dt * fz / mass;
        px += dt * vx;
        py += dt * vy;
        pz += dt * vz;
    }

    float fmap(float value,
               float istart,
               float istop,
               float ostart,
               float ostop) {
        return ostart +
               (ostop - ostart) * ((value - istart) / (istop - istart));
    }

    void draw(BHGraphics *bh) {
        bh->setColor(255, 255, 255);
        bh->drawPixel3D(px, py, pz);
    }

    double distance(Body b) const {
        double dx = px - b.px;
        double dy = py - b.py;
        double dz = pz - b.pz;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    void dampenInertia() {
        fx = 0.0;
        fy = 0.0;
        fz = 0.0;
    }

    void addForce(Body b) {
        double G = 6.67e-11; // Gravitational Constant
        double DAMP = 0.02; // Dampening

        double dx = b.px - px;
        double dy = b.py - py;
        double dz = b.pz - pz;
        double ds = sqrt(dx * dx + dy * dy + dz * dz);

        double df = (G * mass * b.mass) / (ds * ds + DAMP * DAMP);
        fx += df * (dx / ds);
        fy += df * (dy / ds);
        fz += df * (dz / ds);
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
        double ms, nx, ny, nz;
        ms = mass + b.mass;
        nx = (px * mass + b.px * b.mass) / ms;
        ny = (py * mass + b.py * b.mass) / ms;
        nz = (pz * mass + b.pz * b.mass) / ms;

        return {nx, ny, nz, 0, 0, 0, 0, 0, 0, ms};
    }


    Body() = default;

    Body(double px, double py, double pz, double vx, double vy, double vz,
         double fx,
         double fy, double fz,
         double mass) : px(px), py(py), pz(pz), vx(vx), vy(vy), vz(vz), fx
            (fx), fy
                                (fy), fz(fz), mass
                                (mass) {
        isDefault = false;
        internal = false;
    }

    void repluse() {
        vx = -vx;
        vy = -vy;
        vz = -vz;
    }
};


#endif //BARNESHUT_BODY_H
