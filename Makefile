.PHONY: all clean

GEN = ./gen
COVER = ./cover
TARGETS = $(GEN) $(COVER) ./zones
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = space.o globals.o

all: $(TARGETS)

$(GEN): $(OBJS)

$(COVER): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGETS)
