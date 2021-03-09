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


#include "BHGraphics.h"

BHGraphics::BHGraphics(int w, int h) : vw(w), vh(h) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(vw, vh, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

void BHGraphics::render() {
    SDL_RenderPresent(renderer);
}

void BHGraphics::drawRect(int x, int y, int w, int h) {
    auto r = SDL_Rect{};
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_RenderDrawRect(renderer, &r);
}

void BHGraphics::fillRect(int x, int y, int w, int h) {
    auto r = SDL_Rect{};
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_RenderFillRect(renderer, &r);
}
float fmap(float value,
           float istart,
           float istop,
           float ostart,
           float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

void BHGraphics::drawPixel(float x, float y) {
    SDL_RenderDrawPointF(renderer, x, y);
}
void BHGraphics::fillCircle(double x, double y, double r) {
    float radius = fmap(r, 1, 1500, 0.1, 0.4);
    for(float i = 0.0f; i < radius; i+=0.01){
        drawCircle(x, y, i);
    }
}

void BHGraphics::drawCircle(double x, double y, double r) {
    float step = (M_PI * 2) / 50;
    float ox = 0, oy = 0;
    for (float d = 0; d <= M_PI * 2.0; d += step) {

        float cx = ox + (float) r * cos(d);
        float cy = oy - (float) r * sin(d);
        float ofx = x - r/2, ofy = y - r/2;
        SDL_RenderDrawLineF(renderer,
                           (float) ofx + ox,
                           (float) ofy + oy,
                           (float) ofx + cx,
                           (float) ofy + cy);
        ox = cx;
        oy = cy;
    }

}


