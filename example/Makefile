CC = gcc
COPT = -O3
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=gnu11 -D_GNU_SOURCE
CFLAGS += -D BUILD_VERSION="\"$(shell git describe --dirty --always)\""	\
		-D BUILD_DATE="\"$(shell date '+%Y-%m-%d_%H:%M:%S')\""

LIBPREDICT_DIR = ..
LIBPREDICT_SRCS = $(LIBPREDICT_DIR)/julian_date.c \
		$(LIBPREDICT_DIR)/moon.c \
		$(LIBPREDICT_DIR)/observer.c \
		$(LIBPREDICT_DIR)/orbit.c \
		$(LIBPREDICT_DIR)/refraction.c \
		$(LIBPREDICT_DIR)/sdp4.c \
		$(LIBPREDICT_DIR)/sgp4.c \
		$(LIBPREDICT_DIR)/sun.c \
		$(LIBPREDICT_DIR)/celestial.c \
		$(LIBPREDICT_DIR)/unsorted.c

BIN = example
SRC = main.c \
	$(LIBPREDICT_SRCS)

LIBSDIR = 
LIBS = -lm

all:
	$(CC) $(COPT) $(CFLAGS) $(SRC) -o $(BIN) $(LIBSDIR) $(LIBS)

debug: COPT = -Og -ggdb -fno-omit-frame-pointer -D__DEBUG
debug: all

clean:
	rm -fv $(BIN)

