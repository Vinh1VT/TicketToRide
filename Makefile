CC = gcc
CFLAGS = -Wall
OBJ_DIR = ./objects
SRCS = tickettorideapi/codingGameServer.c tickettorideapi/ticketToRide.c projet.c
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

rebuild: clean all

.PHONY: all clean rebuild