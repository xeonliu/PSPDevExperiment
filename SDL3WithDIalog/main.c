#include <SDL3/SDL.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspmoduleinfo.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspgum.h>
#include <string.h>

// /* Define the module info section */
// PSP_MODULE_INFO("GETREKT", 0, 1, 1);
// /* Define the main thread's attribute value (optional) */
// PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* You can skip the "register callback" section if you have included SDL_main.h*/
/* Look at SDL3 documentation for more information.*/
#include <SDL3/SDL_main.h>

pspUtilityMsgDialogParams dialog;

static void ConfigureDialog(pspUtilityMsgDialogParams *dialog, size_t dialog_size)
{
    memset(dialog, 0, dialog_size);

    dialog->base.size = dialog_size;
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,
                                &dialog->base.language); // Prompt language
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,
                                &dialog->base.buttonSwap); // X/O button swap

    dialog->base.graphicsThread = 0x11;
    dialog->base.accessThread = 0x13;
    dialog->base.fontThread = 0x12;
    dialog->base.soundThread = 0x10;
}

static unsigned int __attribute__((aligned(16))) list[262144];

static void ShowMessageDialog(const char *message, int enableYesno)
{
    ConfigureDialog(&dialog, sizeof(dialog));
    dialog.mode = PSP_UTILITY_MSGDIALOG_MODE_TEXT;
    dialog.options = PSP_UTILITY_MSGDIALOG_OPTION_TEXT;

    if (enableYesno)
        dialog.options |= PSP_UTILITY_MSGDIALOG_OPTION_YESNO_BUTTONS | PSP_UTILITY_MSGDIALOG_OPTION_DEFAULT_NO;

    strcpy(dialog.message, message);

    sceUtilityMsgDialogInitStart(&dialog);

    for (;;)
    {
        sceGuStart(GU_DIRECT, list);
        // clear screen
        sceGuClearColor(0xff554433);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
        sceGuFinish();
        sceGuSync(0, 0);

        switch (sceUtilityMsgDialogGetStatus())
        {

        case 2:
            sceUtilityMsgDialogUpdate(1);
            break;

        case 3:
            sceUtilityMsgDialogShutdownStart();
            break;

        case 0:
            // the flag is 0, not 4???
            // no default, fall through?
            return;
        }

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

    SDL_Window *window = SDL_CreateWindow(
        "window",
        480,
        272,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

    SDL_FRect square = {216, 96, 34, 64};

    int running = 1;
    SDL_Event event;
    while (running)
    {
        // event loop
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                SDL_OpenGamepad(event.gdevice.which);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                if (event.gbutton.button == SDL_GAMEPAD_BUTTON_START)
                    running = 0;
                if (event.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_DOWN)
                {
                    ShowMessageDialog("This is a utility message dialog.\n"
                                      "After you acknowledge it, this program will\n"
                                      "show an error message dialog.",
                                      1);
                }
                break;
            }
        }

        // Clear the screen
        SDL_RenderClear(renderer);

        // Draw a red square
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &square);

        // Draw everything on a white background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderPresent(renderer);
    }

    return 0;
}