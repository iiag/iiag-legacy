#
# Makefile
#   This makefile only works right now with gmake :(
#
# The following configurations are accepted on the command line:
#   LUAV=[lua library]
#   WITH_NCURSES=0/1
#   WITH_SDL=0/1
#   WITH_INTROSPECTION=0/1
#   SERVER_WITH_INTROSPECTION=0/1
#

SHELL         = bash
CLIENT_TARGET = iiag
SERVER_TARGET = iiag_server
CC            = gcc
LUAV          = lua
DESTDIR       = /opt/iiag
MANDIR        = /usr/share/man/man6

# Flags that are used regardless of compiliation options go here
CLIENT_CCFL := -c -g -Wall `pkg-config --cflags $(LUAV)`
SERVER_CCFL := -c -g -Wall -DSERVER `pkg-config --cflags $(LUAV)`
CLIENT_LDFL := -Wall -lm `pkg-config --libs $(LUAV)`
SERVER_LDFL := -Wall -lm `pkg-config --libs $(LUAV)`

# List of source files. Source files both in the client and server go here
GENERIC_SRCS := world.c zone.c io/display.c log.c inventory.c util.c item.c \
                creature.c player.c vector.c trigger.c config.c faction.c io/input.c \
                generator.c names.c room.c tile_object.c recipe.c lua/init.c lua/io.c \
                lua/form.c controls.c introspection.c net/net.c net/packet.c \
                io/ncurses/controls.c io/ncurses/input.c io/ncurses/display.c \
                io/ncurses/keys.c io/nogr/display.c io/nogr/input.c spells.c library.c \
                io/sdl/display.c io/sdl/input.c

# Source files exclusive to the client or server go here
CLIENT_SRCS := main.c $(GENERIC_SRCS)
SERVER_SRCS := server.c $(GENERIC_SRCS)

# Default configuration files
ifndef WITH_NCURSES
  WITH_NCURSES := 1
endif

ifndef WITH_SDL
  WITH_SDL := 1
endif

# Compilation option handling
ifeq ($(WITH_NCURSES),1)
  CLIENT_CCFL := $(CLIENT_CCFL) -DWITH_NCURSES
  CLIENT_LDFL := $(CLIENT_LDFL) -lncursesw
endif

ifeq ($(WITH_SDL),1)
  CLIENT_CCFL := $(CLIENT_CCFL) -DWITH_SDL
  CLIENT_LDFL := $(CLIENT_LDFL) -lSDL2 -lSDL2_ttf
endif

ifeq ($(WITH_INTROSPECTION),1)
  CLIENT_CCFL := $(CLIENT_CCFL) -DWITH_INTROSPECTION
endif

ifeq ($(SERVER_WITH_INTROSPECTION),1)
  SERVER_CCFL := $(SERVER_CCFL) -DWITH_INTROSPECTION
endif

# Construct file lists
CLIENT_OBJS := $(addprefix build/obj/,$(patsubst %.c,%.o,$(CLIENT_SRCS)))
CLIENT_DEPS := $(addprefix build/dep/,$(patsubst %.c,%.d,$(CLIENT_SRCS)))
CLIENT_SRCS := $(addprefix src/,$(CLIENT_SRCS))

SERVER_OBJS := $(addprefix build/sobj/,$(patsubst %.c,%.o,$(SERVER_SRCS)))
SERVER_DEPS := $(addprefix build/sdep/,$(patsubst %.c,%.d,$(SERVER_SRCS)))
SERVER_SRCS := $(addprefix src/,$(SERVER_SRCS))

# All the make rules
.PHONY: all clean install

all: $(CLIENT_TARGET) $(SERVER_TARGET)

jerva: all src/jni_main.c src/jni_main.h
	$(CC) -I/usr/lib/jvm/java-8-jdk/include/linux/ -I/usr/lib/jvm/java-8-jdk/include/ -fPIC -c -o build/obj/jni_main.o src/jni_main.c
	$(CC) build/obj/jni_main.o $(filter-out build/obj/main.o,$(CLIENT_OBJS)) $(CLIENT_LDFL) -shared -o libiiag.so

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) $(CLIENT_LDFL) -o $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) $(SERVER_LDFL) -o $(SERVER_TARGET)

build/obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CLIENT_CCFL) $< -fPIC -o $@

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

# Include automagically generated dependencies
-include $(CLIENT_DEPS) $(SERVER_DEPS)

