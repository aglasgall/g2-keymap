CPPFLAGS=-g
SOURCES=KeyCharacterMap.o main.o
TARGET=dumpkeychars
VERSION=0.3

all: $(SOURCES) link

link:
	c++ $(SOURCES) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)

release:
	zip g2-keymap-$(VERSION).zip vision-keypad.kl vision-keypad.kl.orig vision-keypad.kcm.bin vision-keypad.kcm.bin.orig
