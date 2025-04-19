CC = gcc
CFLAGS = -Wall -g
OBJ_DIR = ./objects

SRCS_CMN = tickettorideapi/clientAPI.c tickettorideapi/ticketToRide.c fonctions.c manualPlayer.c randomBot.c firstBot.c
SRCS_MAIN = projet.c
SRCS_STAT = stat.c

OBJS_CMN = $(SRCS_CMN:%.c=$(OBJ_DIR)/%.o)
OBJS_MAIN = $(SRCS_MAIN:%.c=$(OBJ_DIR)/%.o)
OBJS_STAT = $(SRCS_STAT:%.c=$(OBJ_DIR)/%.o)

TARGETS = projet stat

all: $(TARGETS)

main : projet

projet: $(OBJS_CMN) $(OBJS_MAIN)
	$(CC) $(CFLAGS) -o $@ $^

stat : $(OBJS_CMN) $(OBJS_STAT)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGETS)

git :
	git -C ../tickettorideapi pull
	make all


rebuild: clean all

.PHONY: all clean rebuild git