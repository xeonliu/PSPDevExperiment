#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspsdk.h>
#include "string.h"
#include "module/module.h"

PSP_MODULE_INFO("Main Program", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
#define printf pspDebugScreenPrintf

int main()
{
    SceUID modid;
    int ret;
    int fd;
    pspDebugScreenInit();
    pspDebugScreenSetXY(0, 0);
    printf("Screen Init.\n");

    modid = sceKernelLoadModule("ms0:/ModuleName.prx", 0, NULL);
    printf("Modid: 0x%x\n", modid);
    ret = sceKernelStartModule(modid, 0, NULL, &fd, NULL);
    printf("Return value: 0x%x\nStatus: %d\n", ret, fd);

    if (ret == SCE_KERNEL_ERROR_UNKNOWN_MODULE)
    {
        printf("SceKernel Error: Unknown Module\n");
    }
    else
    {
        for (int i = 0; i < 10; ++i)
        {
            printf("Call add(): %d\n", add());
        }
    }

    while (1)
    {
        /* code */
    }
}
