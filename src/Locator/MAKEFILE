# Makefile for compiling Locator.c into a shared library

CC = gcc
CFLAGS = -fPIC
LDFLAGS = -shared
OUTPUT = Locator.so
SOURCE = Locator.c
LIBS = ../../KalmanFilter/lib/libev3dev-c.a

all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(OUTPUT)