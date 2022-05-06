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
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <OpenGL/glu.h>

#include <iostream>

using std::string;

struct Camera {
    float x{}, y{}, z{};
    float pitch = 45, roll = 45, yaw = 0;
    float scale = 0.7;
    float radius = 0;
};

struct Point3D {
    float x, y, z;
};

struct Point2D {
    float x, y;
};

class BHGraphics {
private:
    int vw, vh;
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_GLContext glContext{};
    GLUquadric *quad{};
    Camera *camera{};
    bool state{};
public:
    BHGraphics(int w, int h);

    ~BHGraphics();

    void clear() ;

    void render();


    int drawChar(char c, float size, float x, float y);

    void fillRect(float x, float y, float w, float h);

    void setColor(int r, int g, int b);

    void setAlphaColor(int r, int g, int b, int a);

    void drawCube(float x, float y, float z, float w, float h, float d);

    void drawOrigin(float x, float y, float z, float w, float h, float d);

    Point2D map3D(float x, float y, float z);

    Point3D rotate3D(Point3D p);

    Camera getCamera() {
        return *camera;
    }

    void rotate(float y, float p, float r);

    void scaleCamera(float s);

    void drawMeter(float x, float y, float w, float h, float p);

    void setRadius(float r);


    void drawString(string s, float size, float x, float y);
    void drawString3D(string s, float size, float x, float y, float z);

    void centerFull(float s);

    void drawCircle(float x, float y, float r);

    float drawStringGetLength(string s, float size, float x, float y);

    void strokeRect(float x, float y, float w, float h);

    void drawSphere(float x, float y, float z, float r);

    void beginSimulationFrame();

    void endSimulationFrame();
};


#endif //BARNESHUT_BHGRAPHICS_H
