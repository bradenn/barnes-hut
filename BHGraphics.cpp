/* *
 * @project barnesHut
 * @file BHGraphics.cpp.cc
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/8/21
 */

#include <SDL.h>


#include <iostream>

using std::string;
#define CHAR_WIDTH 7
#define CHAR_HEIGHT 8

#include "BHGraphics.h"
#include "homespun_font.h"

BHGraphics::BHGraphics(int w, int h) : vw(w), vh(h) {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("N-Bodies", 0, 0, w + 340 + 1, h
                                                             + 1,
                              SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED);
    camera = new Camera{0, 0, 0, 0, 0, 0, 0.7, 1024};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderSetScale(renderer, 2, 2);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}


BHGraphics::~BHGraphics() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void BHGraphics::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void BHGraphics::render() {
    SDL_RenderPresent(renderer);
}

void BHGraphics::drawRect(float x, float y, float w, float h) {
    Point2D p1 = map3D(x, y, 0);
    Point2D p2 = map3D(x + w, y, 0);
    Point2D p3 = map3D(x, y + h, 0);
    Point2D p4 = map3D(x + w, y + h, 0);
    SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderDrawLineF(renderer, p1.x, p1.y, p3.x, p3.y);
    SDL_RenderDrawLineF(renderer, p3.x, p3.y, p4.x, p4.y);
    SDL_RenderDrawLineF(renderer, p2.x, p2.y, p4.x, p4.y);
}

void BHGraphics::fillRect(float x, float y, float w, float h) {
    auto r = SDL_FRect{};
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_RenderFillRectF(renderer, &r);
}

void BHGraphics::setColor(int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void BHGraphics::setAlphaColor(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void BHGraphics::drawMeter(float x, float y, float w, float h, float p) {
    auto o = new SDL_FRect{x, y, w, h};
    auto r = new SDL_FRect{x + 2, y + 2, (w - 4) * p, h - 4};
    SDL_RenderDrawRectF(renderer, o);
    SDL_RenderFillRectF(renderer, r);
}

void BHGraphics::drawString(string s, int size, float x, float y) {
    float pos = x;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (char c : s) {
        int w = drawChar(c, size, pos, y);
        pos += w * size + (4 * log(size));
    }
}

int BHGraphics::drawChar(char c, int size, float x, float y) {
    char ch = c & 0x7F;
    if (ch < ' ') {
        ch = 0;
    } else {
        ch -= ' ';
    }
    int aWidth = 0;

    // 'font' is a multidimensional array of [96][char_width]
    // which is really just a 1D array of size 96*char_width.
    const uint8_t *chr = font[ch];
    // Draw pixels
    for (int j = 0; j < CHAR_WIDTH; j++) {
        if (chr[j] == 0x00) continue;
        aWidth++;
        for (int i = 0; i < CHAR_HEIGHT; i++) {
            if (chr[j] & (1 << i)) {
                fillRect(x + j * size, y + i * size, size, size);
            }
        }
    }
    return aWidth;
}


void BHGraphics::drawCube(float x, float y, float z, float w, float h, float
d) {
    auto tfr = map3D(x, y, z + d);
    auto tfl = map3D(x + w, y, z + d);
    auto bfr = map3D(x, y + h, z + d);
    auto bfl = map3D(x + w, y + h, z + d);
    auto tbr = map3D(x, y, z);
    auto tbl = map3D(x + w, y, z);
    auto bbr = map3D(x, y + h, z);
    auto bbl = map3D(x + w, y + h, z);

//    drawString(std::to_string(w), 2, (tfr.x + bfr.x) / 2, (tfr.y + bfr.y) / 2);

    SDL_RenderDrawLineF(renderer, tfr.x, tfr.y, bfr.x, bfr.y);
    SDL_RenderDrawLineF(renderer, tfr.x, tfr.y, tfl.x, tfl.y);
    SDL_RenderDrawLineF(renderer, tfl.x, tfl.y, bfl.x, bfl.y);
    SDL_RenderDrawLineF(renderer, bfr.x, bfr.y, bfl.x, bfl.y);

    SDL_RenderDrawLineF(renderer, tfr.x, tfr.y, tbr.x, tbr.y);
    SDL_RenderDrawLineF(renderer, bfr.x, bfr.y, bbr.x, bbr.y);
    SDL_RenderDrawLineF(renderer, tfl.x, tfl.y, tbl.x, tbl.y);
    SDL_RenderDrawLineF(renderer, bfl.x, bfl.y, bbl.x, bbl.y);

    SDL_RenderDrawLineF(renderer, tbr.x, tbr.y, tbl.x, tbl.y);
    SDL_RenderDrawLineF(renderer, tbr.x, tbr.y, bbr.x, bbr.y);
    SDL_RenderDrawLineF(renderer, bbr.x, bbr.y, bbl.x, bbl.y);
    SDL_RenderDrawLineF(renderer, bbl.x, bbl.y, tbl.x, tbl.y);
}

void BHGraphics::drawPixel3D(float x, float y, float z) {
    Point2D loc = map3D(x, y, z);

    SDL_RenderDrawPointF(renderer, loc.x, loc.y);
}

void BHGraphics::drawLine(float x1, float y1, float x2, float y2) {
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
}

Point3D matMultiply(const float vec[3], float mat[3][3]) {
    int colsA = 3;
    int rowsA = 3;
    int colsB = 1;

    auto result = new float[3];

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            float sum = 0;
            for (int k = 0; k < colsA; k++) {
                sum += mat[i][k] * vec[k];
            }
            result[i] = sum;
        }
    }
    auto p = Point3D{result[0], result[1], result[2]};
    delete[]result;
    return p;
}

Point2D matMultiply2D(const float vec[2], float mat[2][3]) {

    int colsA = 2;
    int rowsA = 3;
    int colsB = 1;

    auto result = new float[2];

    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            float sum = 0;
            for (int k = 0; k < colsA; k++) {
                sum += mat[i][k] * vec[k];
            }
            result[i] = sum;
        }
    }
    auto p = Point2D{result[0], result[1]};
    delete[]result;
    return p;
}


Point3D BHGraphics::rotate3D(Point3D p) {
    float t = camera->yaw;
    float rotY[3][3] = {
            {cos(t),  0, sin(t)},
            {0,       1, 0},
            {-sin(t), 0, cos(t)},
    };
    float in[3] = {p.x, p.y, p.z};
    Point3D p2 = matMultiply(in, rotY);

    float r = camera->pitch;
    float rotP[3][3] = {
            {1, 0,      0},
            {0, cos(r), -sin(r)},
            {0, sin(r), cos(r)},
    };

    float in2[3] = {p2.x, p2.y, p2.z};
    Point3D p3 = matMultiply(in2, rotP);
    return p3;
}

void BHGraphics::scaleCamera(float s) {
    camera->scale = s;
}

void BHGraphics::setRadius(float r) {
    // Maintain scale with expansion
    float delta = camera->radius / r;
    camera->scale *= delta;
    camera->radius = r;
}

void BHGraphics::rotate(float y, float r, float p) {
    camera->yaw = y;
    camera->pitch = p;
}

Point2D BHGraphics::map3D(float x, float y, float z) {
    float proj[3][3] = {
            {camera->scale, 0,             0},
            {0,             camera->scale, 0}
    };

    auto p3 = rotate3D(Point3D{x, y, z});

    float in[3] = {p3.x, p3.y, p3.z};

    auto p2 = matMultiply2D(in, proj);
    p2.x += 512;
    p2.y += 512;

    return p2;
}


