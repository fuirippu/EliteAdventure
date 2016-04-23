###########################################################
# Makefile for Elite Adventure on linux                   #
#                                          fuirippu 04/16 #
###########################################################
ALLEGRO_DIR     := ../../allegro
ALLEGRO_LNX_DIR := $(ALLEGRO_DIR)/build_linux
PORT_AUDIO_DIR  := ../../../PortAudio/install

CC := gcc
INCDIRS_FLAGS := -I$(ALLEGRO_DIR)/build_common/include -I$(ALLEGRO_LNX_DIR)/include -I$(PORT_AUDIO_DIR)/include
WARNING_FLAGS := -Wall -Wextra -Wno-unknown-pragmas -Werror
CFLAGS := $(INCDIRS_FLAGS) -std=c99 -D_DEBUG -DUSE_ALSA $(WARNING_FLAGS) -O2

LDFLAGS := -L$(ALLEGRO_LNX_DIR)/lib -L$(PORT_AUDIO_DIR)/lib -s
LDLIBS := -lm -lalleg -lportaudio
###########################################################
SRCDIR := ..

_SRCS := alg_gfx.c \
		 alg_main.c \
		 assets.c \
		 colours.c \
		 docked.c \
		 elite.c \
		 file.c \
		 gamelib.c \
		 intro.c \
		 missions.c \
		 obcomp.c \
		 options.c \
		 pilot.c \
		 planet.c \
		 random.c \
		 shipdata.c \
		 shipface.c \
		 sound.c \
		 space.c \
		 stars.c \
		 swat.c \
		 threed.c \
		 trade.c \
		 vector.c

SRCS := $(patsubst %,$(SRCDIR)/%,$(_SRCS))
###########################################################
OBJDIR := obj

_OBJS := $(_SRCS:.c=.o)

OBJS := $(patsubst %,$(OBJDIR)/%,$(_OBJS))
###########################################################
DEPDIR := dep

_DEPS := $(_SRCS:.c=.d)

DEPS := $(patsubst %,$(DEPDIR)/%,$(_DEPS))
###########################################################
.PHONY: all clean 

Elite: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	@echo __Built Elite.__

all: Elite
	@echo __Elite is up-to-date.__

clean:
	rm -rf *.dtmp Elite $(OBJDIR) $(DEPDIR)
###########################################################
# Rule to build an object (.o) from its c source (.c) and
# generate a dependency file (.d)
$(OBJDIR)/%.o: $(DEPDIR)/%.d
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/$(basename $(notdir $<)).c -c -o $@
	@echo __o $(basename $(notdir $<))

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@mkdir -p $(DEPDIR)
	$(CC) -MM $(CFLAGS) $< > $(notdir $<).dtmp
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/$*\.o : ,g' < $(notdir $<).dtmp > $(DEPDIR)/$(basename $(notdir $<)).d
	rm -f $(notdir $<).dtmp
	@echo __d $(basename $(notdir $<))

# Include auto-generated dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
###########################################################

