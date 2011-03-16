CPPFLAGS=-g
SOURCES=KeyCharacterMap.o main.o
TARGET=dumpkeychars
VERSION=0.3
DUMP=$(addsuffix .dump,$(shell ls *.kcm.bin *.kcm.bin.orig))

all: $(TARGET) $(DUMP)

$(TARGET): $(SOURCES)
	c++ $^ -o $@

%.dump: % $(TARGET)
	./$(TARGET) $< > $@

clean:
	rm -f *.o $(TARGET) $(DUMP)

release:
	zip g2-keymap-$(VERSION).zip vision-keypad.kl vision-keypad.kl.orig vision-keypad.kcm.bin vision-keypad.kcm.bin.orig 
