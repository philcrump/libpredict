CC = gcc
COPT = -O1
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=gnu11 -D_GNU_SOURCE

LIBPREDICT_DIR = .
SRCS = $(LIBPREDICT_DIR)/julian_date.c \
		$(LIBPREDICT_DIR)/moon.c \
		$(LIBPREDICT_DIR)/observer.c \
		$(LIBPREDICT_DIR)/orbit.c \
		$(LIBPREDICT_DIR)/refraction.c \
		$(LIBPREDICT_DIR)/sdp4.c \
		$(LIBPREDICT_DIR)/sgp4.c \
		$(LIBPREDICT_DIR)/sun.c \
		$(LIBPREDICT_DIR)/celestial.c \
		$(LIBPREDICT_DIR)/unsorted.c


OBJS = ${SRCS:.c=.o}

LIBSDIR = 
LIBS = -lm

static: ${OBJS}
	@echo "  AR     libpredict.a"
	@ar rcs libpredict.a $(OBJS)

%.o: %.c
	@echo "  CC     "$<
	@${CC} ${COPT} ${CFLAGS} -I $(LIBPREDICT_DIR) -MMD -MP -c -fPIC -o $@ $<

clean:
	rm -fv libpredict.a $(OBJS)


