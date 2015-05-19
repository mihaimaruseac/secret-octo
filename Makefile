.PHONY: all clean

TARGET = ./zones
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = 

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGET) $(NPTARGET)
