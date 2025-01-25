#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <pspkernel.h>

#define PSPDEBUG
#define printf pspDebugScreenPrintf

#include "include/defs.h"
#include "include/exit.h"
#include "include/dump.h"
#include "include/module_exp.h"
#include "include/module_man.h"

// Patch The Export Function Code of Server to call a different "Supervisor" code.
#define PATCH_PROC 0

// Patch the Import Stub of Supervisor in Server Module to a different "supervisor" code.
#define PATCH_IMPORTED_NID 1

PSP_MODULE_INFO("Client", PSP_MODULE_USER, 1, 1);

// Patch The Export Function of Server.
int server_process_req_patched(int id)
{
    sceKernelDelayThread(10000);
    return 100;
}

// Patch Supervisor Method.
const char *sv_check_permissions_patched(int client)
{
    return "yes";
}

typedef int (*METHOD)(int client);

static void AppClientDoRequest(SceModule *modserver)
{
    int client_id = 666;
    METHOD server_f = (METHOD)pspModuleFindExportedNidAddr(modserver, "supervisor", 0xAB55332D);
    int result = (*server_f)(client_id);

    const char *msg;
    if (result == 1)
    {
        msg = "good";
    }
    else if (result == 0)
    {
        msg = "bad";
    }
    else
    {
        msg = "server misbehaving";
    }

    printf("Client request: %s", msg);
}

int main(int argc, char *argv[])
{
    const int choice = PATCH_IMPORTED_NID;

    pspDebugScreenInit();
    pspSdkInstallNoDeviceCheckPatch();
    pspSdkInstallNoPlainModuleCheckPatch();
    pspSdkInstallKernelLoadModulePatch();
    AppSetupExitCallback();

    SceModule *modsuper = pspModuleLoadStartInKernelPart("supervisor.prx");
    SceModule *modserver = pspModuleLoadStartInKernelPart("server.prx");

    pspForEachLoadedModule(print_module);

    // Store the addresses of original procs.
    METHOD sv_check_permissions_orig = (METHOD)pspModuleFindExportedNidAddr(modsuper, "supervisor", 0xB2C390CB);
    METHOD server_process_req_orig = (METHOD)pspModuleFindExportedNidAddr(modserver, "server", 0xAB55332D);

    printf("Orig Super Process Method: %p\n", (void *)sv_check_permissions_orig);
    printf("Orig Server Process Method: %p\n", (void *)server_process_req_orig);

    // Use Server Module as supposed to.
    AppClientDoRequest(modserver);

    // Trivially,change the whole procedure
    pspModuleSetExportedNidToAddr(modserver, "server", 0xAB55332D,
                                  (MemAddress)server_process_req_patched);
    AppClientDoRequest(modserver);

    // Set as it was
    pspModuleSetExportedNidToAddr(modserver, "server", 0xAB55332D,
                                  (MemAddress)server_process_req_orig);
    AppClientDoRequest(modserver);

    MemAddress baseaddr;
    MemAddress calladdr; // absolute addressing.
    MIPSInstruction newinstr;

    if (choice == PATCH_PROC)
    {
        // Using the base address of the server_process_req and the
        // offset to the "jla supervisor_0xB2C390CB", compute the
        // memory address to be patched / changed.
        baseaddr = (MemAddress)server_process_req_orig;
        calladdr = baseaddr + (0x18 >> 2);
        newinstr = JAL_TO(sv_check_permissions_patched);
    }
    else if (choice == PATCH_IMPORTED_NID)
    {
        MemAddress sv_stub_JR_addr = pspModuleFindImportStubAddrByNid(modserver,
                                                                      0xB2C390CB);
        baseaddr = sv_stub_JR_addr;
        calladdr = baseaddr;
        newinstr = J_TO(sv_check_permissions_patched);
    }

    printf("Patch address: 0x%08X with op: 0x%08X\n", (u32)calladdr, (u32)newinstr);
    // dump before
    printf("before patch\n");
    pspDumpMemRegion(calladdr - 2, calladdr + 1);

    // patch
    int intc = pspSdkDisableInterrupts();
    _sw(newinstr, (u32)calladdr);
    if (choice == PATCH_IMPORTED_NID)
    {
        // newinstr = LUI(0x00, 0x00);
        // newinstr = NOP;
        _sw(NOP, (u32)(calladdr + 0x01));
    }
    sceKernelDcacheWritebackAll();
    sceKernelIcacheInvalidateAll();
    pspSdkEnableInterrupts(intc);

    // dump after
    printf("after patch\n");
    pspDumpMemRegion(calladdr - 2, calladdr + 1);

    AppClientDoRequest(modserver);
    printf("Done\n");
    sceKernelExitDeleteThread(0);
    return 0;
}
