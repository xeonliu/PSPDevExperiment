#include "app.h"
#include "cursor.h"

App::App()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("window", 480, 272, 0);
    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    c = new mCursor;
}

App::~App()
{
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
}

void App::run()
{

    while (1)
    {
        // currentFrame time in ms
        Uint64 currentFrameTime = frameEndTime - frameStartTime;
        frameStartTime = SDL_GetTicks();
        // handleEvent();
        draw(currentFrameTime);
        frameEndTime = SDL_GetTicks();
    }
}

void App::handleEvent()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            // throw "QUIT";
            break;
        case SDL_EVENT_GAMEPAD_ADDED:
            SDL_OpenGamepad(event.gdevice.which);
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            // handle gamepad
        default:
            break;
        }
    }
}

void App::draw(int dt)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_FRect square = {216, 96, 34, 64};
    // Draw a red square
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &square);
    // Draw everything on a white background
    c->draw(renderer, dt);
    sceDisplayWaitVblankStart();
    SDL_RenderPresent(renderer);
}
