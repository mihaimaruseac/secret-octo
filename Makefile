.PHONY: all clean

TARGET = ./zones
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = space.o globals.o

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGET) $(NPTARGET)
