## Compiler
CC = gcc
mingwCC = x86_64-w64-mingw32-gcc

## flags
CFLAGS = -Wall -std=c99

## GTK Flags
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

## x86_64-w64-mingw32-gcc flags
mingwFLAGS = -mwindows 

## Output directory
OUTPUT_DIR = bin

.PHONY: all clean copy_deps

all: $(OUTPUT_DIR)/schedule $(OUTPUT_DIR)/stop_tasks

# main: main.c
# 	$(CC) $(CFLAGS) -o main main.c $(GTK_CFLAGS)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DIR)/schedule: schedule_task.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/schedule schedule_task.c

$(OUTPUT_DIR)/stop_tasks: stop_tasks.c | $(OUTPUT_DIR)
	$(mingwCC) -o $(OUTPUT_DIR)/stop_tasks stop_tasks.c $(mingwFLAGS) $(GTK_CFLAGS)
	ldd $(OUTPUT_DIR)/stop_tasks.exe | grep ucrt64 | sed 's/.*=> //g' | sed -E 's/ \(0x.*\)//g' | xargs -I {} cp {} $(OUTPUT_DIR) -v

## Coping dependencies in current directory
# ldd stop_tasks.exe | grep ucrt64 | sed 's/.*=> //g' | sed -E 's/ \(0x.*\)//g' | xargs -I {} cp {} ./ -v
# ref : https://stackoverflow.com/questions/34323865/application-compiled-in-mingw-w64-msys2-the-application-was-unable-to-start-co
# ref : https://stackoverflow.com/questions/72253410/how-to-properly-use-xargs-with-cp

# Clean up object files and the executable
clean:
	rm -rvf $(OUTPUT_DIR)