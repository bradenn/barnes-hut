/* *
 * @project barnesHut
 * @file BHGraphics.h
 * 
 * @abstract  
 *
 * @author Braden Nicholson
 * @date 3/8/21
 */

#ifndef BARNESHUT_BHGRAPHICS_H
#define BARNESHUT_BHGRAPHICS_H

#include <SDL.h>

#include <iostream>

using std::string;

struct Camera {
    float x{}, y{}, z{};
    float pitch = 0, roll = 0, yaw = 0;
    float scale = 0.7;
    float radius = 0;
};

struct Point3D {
    float x, y, z;
};

struct Rotation {
    float p, r, y;
};

struct Point2D {
    float x, y;
};

class BHGraphics {
private:
    int vw, vh;
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    Camera *camera{};
    bool state{};
public:
    BHGraphics(int w, int h);

    ~BHGraphics();

    void clear();

    void render();

    void drawRect(float x, float y, float w, float h);

    int drawChar(char c, int size, float x, float y);

    void fillRect(float x, float y, float w, float h);

    void drawPixel3D(float x, float y, float z);

    void setColor(int r, int g, int b);

    void setAlphaColor(int r, int g, int b, int a);

    void drawString(string s, int size, float x, float y);

    void drawCube(float x, float y, float z, float w, float h, float d);

    Point2D map3D(float x, float y, float z);

    Point3D rotate3D(Point3D p);

    Camera getCamera() {
        return *camera;
    }

    void rotate(float y, float p, float r);

    void scaleCamera(float s);

    void drawMeter(float x, float y, float w, float h, float p);

    void setRadius(float r);

    void drawLine(float x, float y);

    void drawLine(float x1, float y1, float x2, float y2);
};


#endif //BARNESHUT_BHGRAPHICS_H
