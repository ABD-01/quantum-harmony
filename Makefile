CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic 
LDFLAGS = -shared -lws2_32

OUT_DIR = resources
TARGET	= serial_upload.dll
SOURCES = serial_upload.c bridge.c

OBJ = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(OUT_DIR)/$(TARGET)

$(OUT_DIR)/$(TARGET): $(OBJ) | $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@rm -f *.o

serial_upload.o: serial_upload.c
	$(CC) $(CFLAGS) -c $< -o $@

bridge.o: bridge.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR):
	@mkdir -p $@

clean:
	@rm -f *.o $(OUT_DIR)/$(TARGET)