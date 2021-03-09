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

class BHGraphics {
private:
    int vw, vh;
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    bool state{};
public:
    BHGraphics(int w, int h);

    ~BHGraphics();

    void clear();

    void render();

    void drawRect(int x, int y, int w, int h);

    void fillRect(int x, int y, int w, int h);


    void drawCircle(double x, double y, double r);

    void fillCircle(double x, double y, double r);

    void drawPixel(float x, float y);
};


#endif //BARNESHUT_BHGRAPHICS_H
