/* *
 * @project barnesHut
 * @file BHGraphics.cpp.cc
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/8/21
 */

#define SDL_MAIN_HANDLED

#include <SDL.h>


#include <iostream>
#include <utility>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>

using std::string;
#define CHAR_WIDTH 7
#define CHAR_HEIGHT 8

#include "pixelfont.h"
#include "BHGraphics.h"


BHGraphics::BHGraphics(int w, int h) : vw(w), vh(h) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);


    window = SDL_CreateWindow("N-Bodies", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w + 1, h + 1,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    glContext = SDL_GL_CreateContext(window);
    camera = new Camera{0, 0, 0, 45, 0, 45, 1, 1024};


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetSwapInterval(1);

    quad = gluNewQuadric();
    glLoadIdentity();
    glOrtho(0, vw, vh, 0, -2048, 2048);


//    SDL_RenderSetScale(renderer, 2, 2);
//    SDL_RenderClear(renderer);
//    SDL_RenderPresent(renderer);
}


BHGraphics::~BHGraphics() {
    gluDeleteQuadric(quad);
    SDL_DestroyRenderer(renderer);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void BHGraphics::clear() {
//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//    SDL_RenderClear(renderer);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void BHGraphics::render() {

//    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//    SDL_RenderPresent(renderer);
    SDL_GL_SwapWindow(window);
}

void BHGraphics::beginSimulationFrame() {
    glPushMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glLoadIdentity();

    glOrtho(-1024, 1024, -1024, 1024, -1024, 1024);
    glMatrixMode(GL_MODELVIEW);
    gluPerspective(90, (double) vh / (double) vw, -1000, 1000);


    glMatrixMode(GL_MODELVIEW);
    glScalef(camera->scale, camera->scale, camera->scale);


    glRotatef(camera->pitch, 1, 0, 0);
    glRotatef(-camera->yaw, 0, 1, 0);


    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_COLOR_MATERIAL);



}

void BHGraphics::endSimulationFrame() {
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void BHGraphics::strokeRect(float x, float y, float w, float h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void BHGraphics::fillRect(float x, float y, float w, float h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void BHGraphics::setColor(int r, int g, int b) {
    GLfloat div = 255.0;
    glColor3f((GLfloat) r / div, (GLfloat) g / div, (GLfloat) b / div);
}

void BHGraphics::setAlphaColor(int r, int g, int b, int a) {
    GLfloat div = 255.0;
    glColor4f((GLfloat) r / div, (GLfloat) g / div, (GLfloat) b / div, (GLfloat) a / div);
}

void BHGraphics::drawMeter(float x, float y, float w, float h, float p) {
    strokeRect(x, y, w, h);
    fillRect(x + 2, y + 2, (w - 4) * p, h - 4);
}

void BHGraphics::drawString(string s, float size, float x, float y) {
    float pos = x;

    for (char c: s) {
        int w = drawChar(c, size, pos, y);
        pos += w * size + (4 * log(size));
    }
}


void BHGraphics::drawString3D(string s, float size, float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    drawString(std::move(s), size, 0, 0);
    glPopMatrix();
}

float BHGraphics::drawStringGetLength(string s, float size, float x, float y) {
    float pos = x;

    for (char c: s) {
        int w = drawChar(c, size, pos, y);
        pos += w * size + (4 * log(size));
    }
    return pos - x + 16;
}

int BHGraphics::drawChar(char c, float size, float x, float y) {
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


void BHGraphics::drawOrigin(float x, float y, float z, float w, float h, float d) {
    glBegin(GL_LINES);
    glColor3f(0.5, 0.5, 1);
    glVertex3f(x, y, z - d);
    glVertex3f(x, y, z + d);

    glColor3f(0.5, 1, 0.5);
    glVertex3f(x - w, y, z);
    glVertex3f(x + w, y, z);

    glColor3f(1, 0.5, 0.5);
    glVertex3f(x, y - h, z);
    glVertex3f(x, y + h, z);
    glEnd();

    glColor3f(1, 0.5, 0.5);
    drawString("-x", 6, x - w, y);
    drawString("+x", 6, x + w, y);

    glColor3f(0.5, 1, 0.5);
    drawString3D("-y", 6, x - w, y, z);
    drawString3D("+y", 6, x + w, y, z);

    glColor3f(0.5, 0.5, 1);
    drawString3D("-z", 6, x, y, z - d);
    drawString3D("+z", 6, x, y, z + d);



}


void BHGraphics::drawCube(float x, float y, float z, float w, float h, float
d) {


    glBegin(GL_LINES);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(x, y, z + d);

    glVertex3f(x + w, y, z + d);

    glVertex3f(x, y, z);

    glVertex3f(x + w, y, z);

    glVertex3f(x, y + h, z);

    glVertex3f(x + w, y + h, z);

    glVertex3f(x + w, y + h, z + d);

    glVertex3f(x, y + h, z + d);

    // Next

    glVertex3f(x, y, z + d);

    glVertex3f(x, y, z);

    glVertex3f(x + w, y, z + d);

    glVertex3f(x + w, y, z);


    glVertex3f(x, y + h, z + d);

    glVertex3f(x, y + h, z);

    glVertex3f(x + w, y + h, z + d);

    glVertex3f(x + w, y + h, z);


    glVertex3f(x, y + h, z);
    glVertex3f(x, y, z);

    glVertex3f(x + w, y + h, z);
    glVertex3f(x + w, y, z);

    glVertex3f(x + w, y + h, z + d);
    glVertex3f(x + w, y, z + d);

    glVertex3f(x, y + h, z + d);
    glVertex3f(x, y, z + d);


    glEnd();

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

void BHGraphics::drawSphere(float x, float y, float z, float r) {
    glPushMatrix();
    auto q = gluNewQuadric();
    glTranslatef(x, y, z);
    gluSphere(q, r, 16, 16);
    gluDeleteQuadric(q);
    glPopMatrix();
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

void BHGraphics::centerFull(float s) {
    float delta = s / (camera->radius * camera->scale);
    camera->scale *= delta;

}

void BHGraphics::setRadius(float r) {
    // Maintain scale with expansion
    float delta = camera->radius / r;
    camera->scale *= delta;
    camera->radius = r;
}

void BHGraphics::rotate(float y, float r, float p) {
    camera->yaw = y;
    camera->pitch = r;
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
//
//void
//BHGraphics::drawLabeledMeter(float x, float y, string label, float w, float h,
//                             float p, float offset) {
//    drawString(std::move(label), 1.5, x, y);
//    y = y + 15;
//    auto o = new SDL_FRect{x, y, w, h};
//    if (p < 0) {
//        x = x - abs(p) * (w - offset);
//    } else {
//        x = x + abs(p) * (w - offset);
//    }
//    auto r = new SDL_FRect{x + offset, y + 2, (w - offset) * abs(p), h - 4};
//    SDL_RenderDrawRectF(renderer, o);
//    SDL_RenderFillRectF(renderer, r);
//}

void BHGraphics::drawCircle(float x, float y, float r) {
    float points = r;
    glBegin(GL_LINES);
    for (int i = 0; i < (int) points; i++) {
        float phiDx = (float) (M_PI * 2) / points;
        float phi = phiDx * (float) i;
        glVertex3f(x + r * cos(phi), y + r * sin(phi), 0);
    }
    glEnd();

}




