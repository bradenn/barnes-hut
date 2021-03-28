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
    float px = 0, py = 0, pz = 0, vx = 0, vy = 0, vz = 0, fx = 0, fy = 0, fz
            = 0;
    float mass = 0;
    bool isDefault = true;
    bool internal = true;

public:
    void update(float dt) {
        vx += dt * fx / mass;
        vy += dt * fy / mass;
        vz += dt * fz / mass;
        px += dt * vx;
        py += dt * vy;
        pz += dt * vz;
    }

    static float fmap(float value,
                      float istart,
                      float istop,
                      float ostart,
                      float ostop) {
        return ostart +
               (ostop - ostart) * ((value - istart) / (istop - istart));
    }

    void draw(BHGraphics *bh) const {
        bh->setColor(
                128, 128, 128);
        bh->drawPixel3D(px, py, pz);
    }

    double distance(Body b) const {
        double dx = px - b.px;
        double dy = py - b.py;
        double dz = pz - b.pz;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    void clearForce() {
        fx = 0.0;
        fy = 0.0;
        fz = 0.0;
    }

    void addForce(Body b) {
        float G = 6.67e-11; // Gravitational Constant
        float DAMP = 1e1; // Dampening

        float dx = b.px - px;
        float dy = b.py - py;
        float dz = b.pz - pz;
        float ds = sqrt(dx * dx + dy * dy + dz * dz);

        float df = (G * mass * b.mass) / (ds * ds + DAMP * DAMP);
        fx += df * (dx / ds);
        fy += df * (dy / ds);
        fz += df * (dz / ds);
    }

    float ld() const {
        float l = 0;
        if (abs(px) > l) l = abs(px);
        if (abs(py) > l) l = abs(py);
        if (abs(pz) > l) l = abs(pz);
        return l;
    }

    bool in(Quad *q) const {
        return q->contains(px, py);
    }

    bool isSetDefault() const {
        return isDefault;
    }

    bool isSetInternal() const {
        return internal;
    }

    void setInternal() {
        internal = true;
    }

    double getHash() const {
        return px + py + vx + vy;
    }


    Body plus(Body b) const {
        float ms, nx, ny, nz;
        ms = mass + b.mass;
        nx = (px * mass + b.px * b.mass) / ms;
        ny = (py * mass + b.py * b.mass) / ms;
        nz = (pz * mass + b.pz * b.mass) / ms;

        return {nx, ny, nz, 0, 0, 0, 0, 0, 0, ms};
    }


    Body() = default;

    Body(float px, float py, float pz, float vx, float vy, float vz,
         float fx,
         float fy, float fz,
         float mass) : px(px), py(py), pz(pz), vx(vx), vy(vy), vz(vz), fx
            (fx), fy
                               (fy), fz(fz), mass
                               (mass) {
        isDefault = false;
        internal = false;
    }

};


#endif //BARNESHUT_BODY_H
