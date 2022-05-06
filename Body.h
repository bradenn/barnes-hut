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

#include<iostream>
#include<cmath>
#include "Quad.h"

/*
 * Basic Structure inspired by this:
 * ftp://ftp.cs.princeton.edu/pub/cs126/barnes-hut/Body.java
 */

static float GLOBAL_MAX = 2;
static float GLOBAL_MIN = 0;

static float largestBody = 0;

class Body {
private:
    float px = 0, py = 0, pz = 0,
            vx = 0, vy = 0, vz = 0,
            fx = 0, fy = 0, fz = 0;

    float mass = 0, radius = 0;

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
        if (value < istart) return 0;
        if (value > istop) return istop / 2;
        return ostart +
               (ostop - ostart) * ((value - istart) / (istop - istart));
    }

    void draw(BHGraphics *bh, float min, float max, float maxMass) {

//        bh->setColor(fmap(vx, min, max, 32, 200),
//                     fmap(vy, min, max, 32, 200),
//                     fmap(vz, min, max, 32, 200));


        radius = fmap(mass, 0, maxMass, 1, 25);

        bh->drawSphere(px, py, pz, radius);

    }

    float getMass() {
        return mass;
    }

    float distance(Body b) {
        float dx = px - b.px;
        float dy = py - b.py;
        float dz = pz - b.pz;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    void clearForce() {
        fx = 0.0;
        fy = 0.0;
        fz = 0.0;
    }

    void addForce(Body b, float DAMP, float G) {

        float dx = b.px - px;
        float dy = b.py - py;
        float dz = b.pz - pz;
        float ds = sqrt(dx * dx + dy * dy + dz * dz);

        float df = (G * mass * b.mass) / (ds * ds + DAMP);
        fx += df * (dx / ds);
        fy += df * (dy / ds);
        fz += df * (dz / ds);
    }

    float avgForce() {
        return (vx + vy + vz) / 3;
    }


    bool handleCollision(Body body) {
        //((m1 – m2)u1 + 2m2u2) / (m1 + m2)

        if (distance(body) <= radius * 2) {

            float m1 = mass;

            float m2 = body.mass;
            // vx += dt * fx / mass
            float dx = body.px-px;
            float dy = body.py-py;
            float dz = body.pz-pz;
            float ds = sqrt(dx * dx + dy * dy + dz * dz);

            float ux1 = vx;
            float ux2 = body.vx;
            float vdx = ((m1 - m2) * ux1 + 2 * m2 * ux2) / (m1 + m2);

            fx = (dx / ds) * vdx;

            float uy1 = vy;
            float uy2 = body.vy;
            float vdy = ((m1 - m2) * uy1 + 2 * m2 * uy2) / (m1 + m2);

            fy = (dy / ds) * vdy;

            float uz1 = vz;
            float uz2 = body.vz;
            float vdz =  ((m1 - m2) * uz1 + 2 * m2 * uz2) / (m1 + m2);

            fz = (dz / ds) * vdz;
        }
        return false;
    }

    float ld() const {
        float l = 0;
        if (abs(px) > l) l = abs(px);
        if (abs(py) > l) l = abs(py);
        if (abs(pz) > l) l = abs(pz);
        return l;
    }


    bool in(Quad *q) const {
        return q->contains(px, py, pz);
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

    float getHash() const {
        return px + py + vx + vy + mass;
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
        GLOBAL_MIN = 10;
        GLOBAL_MAX = 0;
        if (mass > largestBody) {
            largestBody = mass;
        }

        isDefault = false;
        internal = false;
    }

};


#endif //BARNESHUT_BODY_H
