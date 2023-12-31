#pragma once
#include <SDL3/SDL.h>
#include <pspkernel.h>
#include <pspctrl.h>

#include <pspdisplay.h>

class mCursor
{
public:
    explicit mCursor();
    ~mCursor();
    void draw(SDL_Renderer *, int);

private:
    bool getButton(int button);
    int updateInterval;
    SDL_FRect square;
};