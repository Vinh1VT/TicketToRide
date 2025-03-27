CC = gcc
CFLAGS = -Wall
OBJ_DIR = ./objects
SRCS = tickettorideapi/codingGameServer.c tickettorideapi/ticketToRide.c fonctions.c projet.c manualPlayer.c
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
TARGET = projet

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

git : 
	git -C ../tickettorideapi pull
	make all 

q : all
	./projet Vinh

rebuild: clean all

.PHONY: all clean rebuild git q