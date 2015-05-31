.PHONY: all clean

GEN = ./gen
TARGETS = $(GEN) ./zones
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = space.o globals.o

all: $(TARGETS)

$(GEN): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGET) $(NPTARGET)
