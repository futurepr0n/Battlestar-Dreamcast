TARGET = battlestar.elf
OBJS = main.o audio_system.o loadobj.o blitobj.o movement.o initstorysplash.o wfont.o romdisk.o 
KOS_ROMDISK_DIR = romdisk

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

KOS_LOCAL_CFLAGS = -I$(KOS_BASE)/addons/zlib
	
clean: rm-elf
	-rm -f $(OBJS)
	
rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS) 
	kos-c++ -o $(TARGET) $(OBJS) -lpng -lz -lmp3 -lmodplug -lm

wfont.o: wfont.bin
	$(KOS_BASE)/utils/bin2o/bin2o wfont.bin wfont wfont.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	-rm -f $(OBJS) romdisk_boot.img
	$(KOS_STRIP) $(TARGET)


