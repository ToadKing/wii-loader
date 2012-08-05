#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <ogc/machine/processor.h>

#include "dol.h"
//#include "asm.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

const char loader_dol[] = "sd:/elf/temp.dol";

extern void __exception_closeall();
extern s32 __IOS_ShutdownSubsystems();

int main(int argc, char **argv) {
   VIDEO_Init();
   switch(VIDEO_GetCurrentTvMode()) {
      case VI_NTSC:
         rmode = &TVNtsc480IntDf;
         break;
      case VI_PAL:
         rmode = &TVPal528IntDf;
         break;
      case VI_MPAL:
         rmode = &TVMpal480IntDf;
         break;
      default:
         rmode = &TVNtsc480IntDf;
         break;
   }

   PAD_Init();

   xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
   console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
   
   VIDEO_Configure(rmode);
   VIDEO_SetNextFramebuffer(xfb);
   VIDEO_SetBlack(FALSE);
   VIDEO_Flush();
   VIDEO_WaitVSync();

   if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

   printf("\n\nloading\n");
   VIDEO_WaitVSync();
   fatInitDefault();
   fatMountSimple("sd:", &__io_wiisd);
   FILE * fp = fopen(loader_dol, "rb");
   if (fp == NULL)
   {
      printf("not found\n");
      VIDEO_WaitVSync();
      return 0;
   }
   fseek(fp, 0, SEEK_END);
   size_t size = ftell(fp);
   fseek(fp, 0, SEEK_SET);
   u8 *mem = (u8 *)0x92000000; // should be safe for this small program to use
   fread(mem, 1, size, fp);
   fclose(fp);
   fatUnmount("sd:");
   void (*ep)() = (void(*)())load_dol_image(mem);
   printf("jumping to 0x%08X\n", (unsigned int)ep);

   SYS_ResetSystem(SYS_SHUTDOWN,0,0);

   __lwp_thread_stopmultitasking(ep);

   return 0; // fixes gcc warning
}
