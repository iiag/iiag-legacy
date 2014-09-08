#
# Makefile
#

SHELL         = bash
CLIENT_TARGET = iiag
SERVER_TARGET = iiag_server
CC            = gcc
LUAV          = lua
DESTDIR       = /opt/iiag

# Add -DWITH_INTROSPECTION to *_CCFL to enable introspection
CLIENT_CCFL  := -c -g -Wall `pkg-config --cflags $(LUAV)`
SERVER_CCFL	 := -c -g -Wall -DSERVER `pkg-config --cflags $(LUAV)`
LDFL         := -Wall -lncurses -lm `pkg-config --libs $(LUAV)`

CLIENT_SRCS := main.c world.c zone.c io/display.c log.c inventory.c util.c item.c \
               creature.c player.c vector.c trigger.c config.c faction.c io/input.c \
               generator.c names.c room.c lua/init.c lua/io.c lua/form.c commands.c \
               introspection.c controlls.c net/net.c net/packet.c

SERVER_SRCS := server.c world.c zone.c io/display.c log.c inventory.c util.c item.c \
               creature.c player.c vector.c trigger.c config.c faction.c io/input.c \
               generator.c names.c room.c lua/init.c lua/io.c lua/form.c commands.c \
               introspection.c controlls.c net/net.c net/packet.c

CLIENT_OBJS := $(addprefix obj/,$(patsubst %.c,%.o,$(CLIENT_SRCS)))
CLIENT_DEPS := $(addprefix dep/,$(patsubst %.c,%.d,$(CLIENT_SRCS)))
CLIENT_SRCS := $(addprefix src/,$(CLIENT_SRCS))

SERVER_OBJS := $(addprefix sobj/,$(patsubst %.c,%.o,$(SERVER_SRCS)))
SERVER_DEPS := $(addprefix sdep/,$(patsubst %.c,%.d,$(SERVER_SRCS)))
SERVER_SRCS := $(addprefix src/,$(SERVER_SRCS))

.PHONY: all clean install

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) $(LDFL) -o $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) $(LDFL) -o $(SERVER_TARGET)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CLIENT_CCFL) $< -o $@

sobj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(SERVER_CCFL) $< -o $@

clean:
	rm -rf obj
	rm -rf dep
	rm -rf sobj
	rm -rf sdep
	rm -f $(CLIENT_TARGET)
	rm -f $(SERVER_TARGET)

install: all
	mkdir -p $(DESTDIR)
	cp iiag $(DESTDIR)
	cp iiag_server $(DESTDIR)
	cp -r script $(DESTDIR)
	cp -r names $(DESTDIR)

-include $(DEPS)
