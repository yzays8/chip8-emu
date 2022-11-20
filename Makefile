CC = g++
TARGET = emu
OBJS = emu.o chip8.o

CXXFLAGS += -O2 -Wall `sdl2-config --cflags`
LIBS = `sdl2-config --libs`

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

$(TARGET): $(OBJS) Makefile
	$(CC) $(OBJS) $(LIBS) -o $@

%.o: %.cpp Makefile
	$(CC) $(CXXFLAGS) -c $<