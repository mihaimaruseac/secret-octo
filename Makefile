.PHONY: all clean

GEN = ./gen
COVER = ./cover
ESPRESSO = ./espresso
COMPUTE = ./compute
PARAMGEN = ./param_generator
TARGETS = $(GEN) $(COVER) $(ESPRESSO) $(COMPUTE) $(PARAMGEN)

CC = gcc
CFLAGS = -I/usr/local/include/pbc -Wall -Wextra -g -O0
LDFLAGS = -lm -lgmp -lpbc

OBJS = space.o globals.o crypto.o

all: $(TARGETS)

$(GEN): $(OBJS)
$(COVER): $(OBJS)
$(ESPRESSO): $(OBJS)
$(COMPUTE): $(OBJS)
$(PARAMGEN): $(OBJS)

clean:
	@$(RM) $(OBJS) $(TARGETS)
