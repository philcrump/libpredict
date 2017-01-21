
SRCS = main.c

CFLAGS = -Og -Wall

LIBPREDICT_DIR = ..
SRCS += $(LIBPREDICT_DIR)/julian_date.c \
		$(LIBPREDICT_DIR)/observer.c \
		$(LIBPREDICT_DIR)/orbit.c \
		$(LIBPREDICT_DIR)/refraction.c \
		$(LIBPREDICT_DIR)/sdp4.c \
		$(LIBPREDICT_DIR)/sgp4.c \
		$(LIBPREDICT_DIR)/sun.c \
		$(LIBPREDICT_DIR)/unsorted.c

LDFLAGS = -lm

all:

	gcc $(SRCS) $(CFLAGS) $(LDFLAGS) -o example