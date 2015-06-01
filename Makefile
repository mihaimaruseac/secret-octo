.PHONY: all clean

GEN = ./gen
COVER = ./cover
ESPRESSO = ./espresso
TARGETS = $(GEN) $(COVER) $(ESPRESSO) #./zones
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = space.o globals.o

all: $(TARGETS)

$(GEN): $(OBJS)
$(COVER): $(OBJS)
$(ESPRESSO): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGETS)
