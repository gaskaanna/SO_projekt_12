# Makefile

# Kompilator i flagi
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -fPIC
INCLUDES = -I./include
LIBS = -lrt -lpthread

# Katalogi
SRC_DIR = src
OBJ_DIR = obj

# Pliki źródłowe
SRCS = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = piekarnia

.PHONY: all clean rebuild

all: rebuild

rebuild: clean $(TARGET)

$(TARGET): $(OBJ_DIR) $(SRC_DIR)/main.c $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_DIR)/main.c $(OBJS) $(LIBS) -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)