#include "cursor.h"
mCursor::mCursor()
{
    square = {252, 100, 10, 10};
}

mCursor::~mCursor()
{
}

void mCursor::draw(SDL_Renderer *renderer, int elapsedTime)
{
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);
    square.x += (pad.Lx - 120) * elapsedTime / 1000;
    square.y += (pad.Ly - 150) * elapsedTime / 1000;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &square);
}

/// @brief Do not need to wait gameCtrl in event loop, Peek at any time.
/// @param button
/// @return
bool mCursor::getButton(int button)
{
    SceCtrlData input;
    SceCtrlData inputSnapshot;
    sceCtrlPeekBufferPositive(&inputSnapshot, 1);

    if (inputSnapshot.Buttons != 0)
    {
        if (inputSnapshot.Buttons & button)
        {
            return true;
        }
    }
    return false;
}