CC = g++
TARGET = emu
OBJS = emu.o chip8.o graphic.o timer.o sound_timer.o delay_timer.o input.o sound.o

CXXFLAGS = -O2 -Wall `sdl2-config --cflags`
LDFLAGS = -pthread
LIBS = `sdl2-config --libs` -lSDL2_mixer

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)

.PHONY: run
run:
	./$(TARGET) $(ROM)

$(TARGET): $(OBJS) Makefile
	$(CC) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

%.o: %.cpp Makefile
	$(CC) $(CXXFLAGS) -c $<