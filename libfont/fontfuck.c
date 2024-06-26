/**
 * @brief This file is a reversed version of TPU's libfont.prx (fontfunk) library
 * Based on the libfont.prx binary, with reference to hz86/mgspw, a locolization plugin for Metal Gear Solid: Peace Walker
 */
#include <pspkernel.h>
#include "ftttlib.h"
// The Error Code definitions.

// https://github.com/uofw/uofw/blob/master/include/common/errors.h#L286
#define SCE_ERROR_KERNEL_LIBRARY_IS_NOT_LINKED 0x8002013A

// https://www.psdevwiki.com/vita/Error_Codes
#define SCE_FONT_ERROR_FILEOPEN 0x80460005
#define SCE_FONT_ERROR_FILECLOSE 0x80460006
#define SCE_FONT_ERROR_FILEREAD 0x80460007
#define SCE_FONT_ERROR_FILESEEK 0x80460008

// https://github.com/pspdev/pspsdk/blob/master/src/user/pspmoduleinfo.h

PSP_MODULE_INFO("fontfuck", PSP_MODULE_SINGLE_LOAD | PSP_MODULE_SINGLE_START, 1, 1);

typedef struct sceFont_t_initRec
{
    void *userData;
    unsigned long maxNumFonts;
    void *cache;
    void *(*allocFunc)(void *, unsigned long);
    void (*freeFunc)(void *, void *);
    void *(*openFunc)(void *, void *, signed long *);
    signed long (*closeFunc)(void *, void *);
    unsigned long (*readFunc)(void *, void *, void *, unsigned long, unsigned long, signed long *);
    signed long (*seekFunc)(void *, void *, unsigned long);
    signed long (*onErrorFunc)(void *, signed long);
    signed long (*whenDoneReadFunc)(void *, signed long);
} sceFont_t_initRec;

int fuck_thread()
{
    // printf("Fontfuck Start!\n");
    int status;
    SceUID modid = sceKernelLoadModule("disc0:/PSP_GAME/USRDIR/oldfont.prx", 0, NULL);
    if (modid > 0)
    {
        sceKernelStartModule(modid, 0, NULL, &status, 0);
    }
    sceKernelExitDeleteThread(0);
    return 0;
}

int module_start(int argc, char **argv)
{
    SceUID thid;
    thid = sceKernelCreateThread("fontfuck", fuck_thread, 0x19, 0x800, 0, NULL);
    if (thid > 0)
    {
        sceKernelStartThread(thid, argc, argv);
    }
    return 0;
}

int module_stop(int argc, char **argv)
{
    return 0;
}

/* Custom IO Functions */
static void *my_open(void *pdata, char *filename, int *error)
{
    SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);
    if (fd < 0)
    {
        *error = SCE_FONT_ERROR_FILEOPEN;
        return 0;
    }
    *error = 0;
    return (void *)fd;
}

static int my_close(void *pdata, void *fileid)
{
    return sceIoClose((SceUID)fileid) < 0 ? SCE_FONT_ERROR_FILECLOSE : 0;
}

static int my_read(void *pdata, void *fileid, void *pbuf, int byte, int unit, int *error)
{
    int count = byte * unit;
    int retv = sceIoRead((SceUID)fileid, pbuf, count);
    if (retv < count)
    {
        *error = SCE_FONT_ERROR_FILEREAD;
        return 0;
    }
    *error = 0;
    return unit;
}

static int my_seek(void *pdata, void *fileid, int offset)
{
    return sceIoLseek32((SceUID)fileid, offset, PSP_SEEK_SET) < 0 ? SCE_FONT_ERROR_FILESEEK : 0;
}

int sceFontNewLib(sceFont_t_initRec *params, int *errorCode)
{
    int iVar1;

    if (params->openFunc == NULL)
    {
        params->openFunc = (void *)my_open;
        params->closeFunc = (void *)my_close;
        params->readFunc = (void *)my_read;
        params->seekFunc = (void *)my_seek;
    }

    while ((iVar1 = sceFtttNewLib(params, errorCode)) == SCE_ERROR_KERNEL_LIBRARY_IS_NOT_LINKED)
    {
        sceKernelDelayThread(200000);
    }

    return iVar1;
}

/* Patch Open Function */
int sceFontOpen(int fontLibHandle, int index, int mode, int errorCodePtr)
{
    return sceFtttOpen(fontLibHandle, (int)"disc0:/PSP_GAME/USRDIR/font.pgf", mode, errorCodePtr);
}

/* Below are just simple wrapper functions */
int sceFontFlush(int fontHandle)
{
    return sceFtttFlush(fontHandle);
}

int sceFontFindOptimumFont(int fontLibHandle, int fontStylePtr, int errorCodePtr)
{
    return sceFtttFindOptimumFont(fontLibHandle, fontStylePtr, errorCodePtr);
}

int sceFontGetFontInfo(int fontHandle, int fontInfoPtr)
{
    return sceFtttGetFontInfo(fontHandle, fontInfoPtr);
}

int sceFontGetNumFontList(int fontLibHandle, int errorCodePtr)
{
    return sceFtttGetNumFontList(fontLibHandle, errorCodePtr);
}

int sceFontCalcMemorySize(int fontLibHandle, int fontStylePtr, int errorCodePtr)
{
    return sceFtttCalcMemorySize(fontLibHandle, fontStylePtr, errorCodePtr);
}

int sceFontClose(int fontHandle)
{
    return sceFtttClose(fontHandle);
}

int sceFontPointToPixelV(int fontLibHandle, int fontPointsV, int errorCodePtr)
{
    return sceFtttPointToPixelV(fontLibHandle, fontPointsV, errorCodePtr);
}

int sceFontPointToPixelH(int fontLibHandle, int fontPointsH, int errorCodePtr)
{
    return sceFtttPointToPixelH(fontLibHandle, fontPointsH, errorCodePtr);
}

int sceFontSetResolution(int fontLibHandle, int hRes, int vRes)
{
    return sceFtttSetResolution(fontLibHandle, hRes, vRes);
}

int sceFontGetShadowImageRect(int fontHandle, int charCode, int shadowRectPtr)
{
    return sceFtttGetShadowImageRect(fontHandle, charCode, shadowRectPtr);
}

int sceFontGetFontInfoByIndexNumber(int fontLibHandle, int fontInfoPtr, int fontIndex)
{
    return sceFtttGetFontInfoByIndexNumber(fontLibHandle, fontInfoPtr, fontIndex);
}

int sceFontGetShadowGlyphImage(int fontHandle, int charCode, int shadowGlyphImagePtr)
{
    return sceFtttGetShadowGlyphImage(fontHandle, charCode, shadowGlyphImagePtr);
}

int sceFontDoneLib(int fontLibHandle)
{
    return sceFtttDoneLib(fontLibHandle);
}

int sceFontOpenUserFile(int fontLibHandle, int fileNameAddr, int mode, int errorCodePtr)
{
    return sceFtttOpenUserFile(fontLibHandle, fileNameAddr, mode, errorCodePtr);
}

int sceFontGetCharImageRect(int fontHandle, int charCode, int charRectPtr)
{
    return sceFtttGetCharImageRect(fontHandle, charCode, charRectPtr);
}

int sceFontGetShadowGlyphImage_Clip(int fontHandle, int charCode, int shadowGlyphImagePtr, int clipXPos, int clipYPos, int clipWidth, int clipHeight)
{
    return sceFtttGetShadowGlyphImage_Clip(fontHandle, charCode, shadowGlyphImagePtr, clipXPos, clipYPos, clipWidth, clipHeight);
}

int sceFontFindFont(int fontLibHandle, int fontStylePtr, int errorCodePtr)
{
    return sceFtttFindFont(fontLibHandle, fontStylePtr, errorCodePtr);
}

int sceFontPixelToPointH(int fontLibHandle, int fontPixelsH, int errorCodePtr)
{
    return sceFtttPixelToPointH(fontLibHandle, fontPixelsH, errorCodePtr);
}

int sceFontGetCharGlyphImage(int fontHandle, int charCode, int glyphImagePtr)
{
    return sceFtttGetCharGlyphImage(fontHandle, charCode, glyphImagePtr);
}

int sceFontGetShadowInfo(int fontHandle, int charCode, int shadowCharInfoPtr)
{
    return sceFtttGetShadowInfo(fontHandle, charCode, shadowCharInfoPtr);
}

int sceFontOpenUserMemory(int fontLibHandle, int memoryFontPtr, int memoryFontLength, int errorCodePtr)
{
    return sceFtttOpenUserMemory(fontLibHandle, memoryFontPtr, memoryFontLength, errorCodePtr);
}

int sceFontGetFontList(int fontLibHandle, int fontStylePtr, int numFonts)
{
    return sceFtttGetFontList(fontLibHandle, fontStylePtr, numFonts);
}

int sceFontGetCharGlyphImage_Clip(int fontHandle, int charCode, int glyphImagePtr, int clipXPos, int clipYPos, int clipWidth, int clipHeight)
{
    return sceFtttGetCharGlyphImage_Clip(fontHandle, charCode, glyphImagePtr, clipXPos, clipYPos, clipWidth, clipHeight);
}

int sceFontGetCharInfo(int fontHandle, int charCode, int charInfoPtr)
{
    return sceFtttGetCharInfo(fontHandle, charCode, charInfoPtr);
}

int sceFontSetAltCharacterCode(int fontLibHandle, int charCode)
{
    return sceFtttSetAltCharacterCode(fontLibHandle, charCode);
}

int sceFontPixelToPointV(int fontLibHandle, int fontPixelsV, int errorCodePtr)
{
    return sceFtttPixelToPointV(fontLibHandle, fontPixelsV, errorCodePtr);
}
