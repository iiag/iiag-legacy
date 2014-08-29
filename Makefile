#
# Makefile
#

SHELL   = bash
TARGET  = iiag
CC      = gcc
LUAV    = lua
ARCH    = 64BIT
DESTDIR = /opt/iiag
CCFL   := -c -g -Wall -DWITH_INTROSPECTION -D_$(ARCH) `pkg-config --cflags $(LUAV)`
LDFL   := -Wall -lncurses -lm `pkg-config --libs $(LUAV)`

SRCS := main.c world.c zone.c display.c log.c inventory.c util.c item.c \
        creature.c player.c vector.c trigger.c config.c faction.c input.c \
        generator.c names.c lua/init.c lua/io.c lua/form.c commands.c \
        introspection.c

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

install: all
	mkdir -p $(DESTDIR)
	cp iiag $(DESTDIR)
	cp -r script $(DESTDIR)
	cp -r names $(DESTDIR)

-include $(DEPS)
