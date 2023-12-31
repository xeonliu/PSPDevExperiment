#pragma once
#include <SDL3/SDL.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
class mCursor;
class App
{
public:
    App();
    ~App();
    void run();
    void handleEvent();
    void draw(int dt);

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint64 frameStartTime;
    Uint64 frameEndTime;
    mCursor *c;
};
