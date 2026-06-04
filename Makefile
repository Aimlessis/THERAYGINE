CC     = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = src/main.c \
      src/core.c \
      src/renderer.c \
      src/camera.c \
      src/editor.c \
      src/raygui_impl.c

TARGET = engine

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
