# the compiler: gcc for C program
CC = gcc
# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
# the build target executable:
TARGET = FTPclient FTPserver
all: $(TARGET)
$(TARGET): %:%.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -rf $(TARGET)