CFLAGS=-g
SOURCES=KeyCharacterMap.o main.o
TARGET=dumpkeychars

all: $(SOURCES) link

link:
	c++ $(SOURCES) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)
