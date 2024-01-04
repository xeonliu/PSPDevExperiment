TARGET = main
OBJS= main.o
CFLAGS = -g -I../build/include
LIBDIR  = ../build/lib
LIBS =   -lavformat -lavcodec -lavutil -lswscale -lm -lz  -lGL -lGLU -lglut -lz \
         -lpspvfpu -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -lpspvram
BUILD_PRX = 1
EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = FFmpeg example

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak