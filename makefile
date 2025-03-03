CC = gcc
#CFLAGS = -ansi -pedantic-errors -Wall -Wextra -fPIC
CFLAGS = -Wall -Wextra -pedantic-errors -fPIC
INCLUDES = -I ./dependencies/include/ -I./include/

SRC = ./src/watch_dog.c \
      ./src/watch_dog_util.c \
      ./dependencies/src/heap_pq_scheduler.c \
      ./dependencies/src/heap_pq.c \
      ./dependencies/src/heap.c \
      ./dependencies/src/dvector.c \
      ./dependencies/src/uid.c \
      ./dependencies/src/task.c \
      ./test/watch_dog.c

OUTPUT = watchdog

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDES) -o $(OUTPUT) -lpthread

clean:
	rm -f $(OUTPUT)
