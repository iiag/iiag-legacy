#
# Makefile
#

SHELL  = bash
TARGET = iiag
CC     = gcc
CCFL   = -c -g -Wall
LDFL   = -Wall -lncurses -lm

SRCS := main.c world.c zone.c display.c log.c inventory.c util.c item.c \
        creature.c \
        iml/iml.c iml/lang.c \
        form/form.c form/crtr.c form/item.c

OBJS := $(addprefix obj/,$(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix dep/,$(patsubst %.c,%.d,$(SRCS)))
SRCS := $(addprefix src/,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFL) -o $(TARGET)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CCFL) $< -o $@

clean:
	rm -rf obj
	rm -rf dep
	rm -f $(TARGET)

-include $(DEPS)
