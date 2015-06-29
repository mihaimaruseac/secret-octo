.PHONY: all clean

GEN = ./gen
COVER = ./cover
ESPRESSO = ./espresso
COMPUTE = ./compute
TARGETS = $(GEN) $(COVER) $(ESPRESSO) $(COMPUTE)
CC = gcc
CFLAGS = -Wall -Wextra -g -O0
LDFLAGS = -lm
OBJS = space.o globals.o

all: $(TARGETS)

$(GEN): $(OBJS)
$(COVER): $(OBJS)
$(ESPRESSO): $(OBJS)
$(COMPUTE): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGETS)
