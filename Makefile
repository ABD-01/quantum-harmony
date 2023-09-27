## Compiler
CC = gcc

## flags
CFLAGS = -Wall -std=c99

## GTK Flags
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

main: main.c schedule stop_tasks
	$(CC) $(CFLAGS) -o main main.c $(GTK_CFLAGS)

schedule: schedule_task.c
	$(CC) $(CFLAGS) -o schedule schedule_task.c

stop_tasks: stop_tasks.c
	$(CC) $(CFLAGS) -o stop_tasks stop_tasks.c $(GTK_CFLAGS)

# Clean up object files and the executable
clean:
	rm -f main