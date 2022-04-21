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

static float GLOBAL_MAX = 0;
static float GLOBAL_MIN = 10;

static float largestBody = 0;

class Body {
private:
    float px = 0, py = 0, pz = 0,
            vx = 0, vy = 0, vz = 0,
            fx = 0, fy = 0, fz = 0;

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

        if (vx > GLOBAL_MAX) {
            GLOBAL_MAX = vx;
        }

        if (vx < GLOBAL_MIN) {
            GLOBAL_MIN = vx;
        }

        if (vy > GLOBAL_MAX) {
            GLOBAL_MAX = vy;
        }
        if (vy < GLOBAL_MIN) {
            GLOBAL_MIN = vy;
        }

        if (vz > GLOBAL_MAX) {
            GLOBAL_MAX = vz;
        }
        if (vz < GLOBAL_MIN) {
            GLOBAL_MIN = vz;
        }
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

    void draw(BHGraphics *bh) {
        bh->setAlphaColor(fmap(vx, GLOBAL_MIN, GLOBAL_MAX, 32, 255),
                          fmap(vy, GLOBAL_MIN, GLOBAL_MAX, 32, 255),
                          fmap(vz, GLOBAL_MIN, GLOBAL_MAX, 32, 255), 255);
        float radius = fmap(mass, 25, 200000, 4, 255) ;
        for (float i = 0; i < M_PI * 2; i += M_PI_2 / 4) {
            for (float j = 0; j < M_PI * 2; j += M_PI_2 / 4) {
                float posX = cos(i) * sin(j) * radius;
                float posZ = sin(i) * sin(j) * radius;
                float posY = cos(j) * radius;
                bh->drawPixel3D(px + posX, py + posY, pz + posZ);
            }
        }
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
