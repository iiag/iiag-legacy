#
# Makefile
#

SHELL         = bash
CLIENT_TARGET = iiag
SERVER_TARGET = iiag_server
CC            = gcc
LUAV          = lua
DESTDIR       = /opt/iiag
MANDIR        = /usr/share/man/man6

# Add -DWITH_INTROSPECTION to *_CCFL to enable introspection
CLIENT_CCFL  := -c -g -Wall -DWITH_NCURSES -DWITH_SDL `pkg-config --cflags $(LUAV)`
SERVER_CCFL	 := -c -g -Wall -DSERVER `pkg-config --cflags $(LUAV)`
CLIENT_LDFL         := -Wall -lncursesw -lSDL2 -lSDL2_ttf -lm `pkg-config --libs $(LUAV)`
SERVER_LDFL            := -Wall -lm `pkg-config --libs $(LUAV)`

CLIENT_SRCS := main.c world.c zone.c io/display.c log.c inventory.c util.c item.c \
               creature.c player.c vector.c trigger.c config.c faction.c io/input.c \
               generator.c names.c room.c tile_object.c recipe.c lua/init.c lua/io.c \
               lua/form.c controls.c introspection.c net/net.c net/packet.c \
               io/ncurses/controls.c io/ncurses/input.c io/ncurses/display.c \
               io/ncurses/keys.c io/nogr/display.c io/nogr/input.c spells.c library.c \
               io/sdl/display.c io/sdl/input.c

SERVER_SRCS := server.c world.c zone.c io/display.c log.c inventory.c util.c item.c \
               creature.c player.c vector.c trigger.c config.c faction.c io/input.c \
               generator.c names.c room.c tile_object.c recipe.c lua/init.c lua/io.c \
               lua/form.c controls.c introspection.c net/net.c net/packet.c \
               io/ncurses/controls.c io/ncurses/input.c io/ncurses/display.c \
               io/ncurses/keys.c io/nogr/display.c io/nogr/input.c spells.c library.c \
               io/sdl/display.c io/sdl/input.c

CLIENT_OBJS := $(addprefix build/obj/,$(patsubst %.c,%.o,$(CLIENT_SRCS)))
CLIENT_DEPS := $(addprefix build/dep/,$(patsubst %.c,%.d,$(CLIENT_SRCS)))
CLIENT_SRCS := $(addprefix src/,$(CLIENT_SRCS))

SERVER_OBJS := $(addprefix build/sobj/,$(patsubst %.c,%.o,$(SERVER_SRCS)))
SERVER_DEPS := $(addprefix build/sdep/,$(patsubst %.c,%.d,$(SERVER_SRCS)))
SERVER_SRCS := $(addprefix src/,$(SERVER_SRCS))

.PHONY: all clean install

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) $(CLIENT_LDFL) -o $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) $(SERVER_LDFL) -o $(SERVER_TARGET)

build/obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CLIENT_CCFL) $< -o $@

build/sobj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(SERVER_CCFL) $< -o $@

clean:
	rm -rf build
	rm -f $(CLIENT_TARGET)
	rm -f $(SERVER_TARGET)

install: all
	mkdir -p $(DESTDIR)
	cp iiag $(DESTDIR)
	cp iiag_server $(DESTDIR)
	cp -r script $(DESTDIR)
	cp -r names $(DESTDIR)
	cp -r tileset $(DESTDIR)
	mkdir -p $(MANDIR)
	cp doc/iiag.6 $(MANDIR)
	gzip -f $(MANDIR)/iiag.6

-include $(CLIENT_DEPS) $(SERVER_DEPS)
