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

static int largestQuad = 0;
static int smallestQuad = 10;


class Quad {
private:
    float xm, ym, zm, l;
public:
    Quad(float xm, float ym, float zm, float l) : xm(xm), ym(ym), zm(zm), l(l) {}

    // This is not the wey
    bool contains(double x, double y, double z) const {
        float xs = xm - l / 2;
        float xe = xm + l / 2;
        float ys = ym - l / 2;
        float ye = ym + l / 2;
        float zs = zm - l / 2;
        float ze = zm + l / 2;
        return (x >= xs && x <= xe && y >= ys && y <= ye && z >= zs && z <= ze);
    }

    void draw(BHGraphics *bh) const {

        int c = (int)fmap(l, (float)smallestQuad, (float)largestQuad, 0, 128);
        bh->setAlphaColor(255-c,255-c,255-c, 10);

        bh->drawCube(xm-(l / 2), ym-(l / 2), zm-(l / 2), l, l, l);
    }

    double length() const {
        return l;
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



    Quad *SubTree(int val) {
        int octMap[8][3] = {
                {-1, -1, -1},
                {1,  -1, -1},
                {-1, 1,  -1},
                {1,  1,  -1},
                {-1, -1, 1},
                {1,  -1, 1},
                {-1, 1,  1},
                {1,  1,  1},
        };
        float qx = xm + (l / 4) * (float) octMap[val][0];
        float qy = ym + (l / 4) * (float) octMap[val][1];
        float qz = zm + (l / 4) * (float) octMap[val][2];
        if(smallestQuad >(int) l) {
            smallestQuad =(int) l;
        }

        if(largestQuad < (int)l) {
            largestQuad = (int) l;
        }
        return new Quad(qx, qy, qz, l / 2);
    }

};


#endif //BARNESHUT_QUAD_H
