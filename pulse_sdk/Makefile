# Allerta Inc Pulse Build System
# Copyright 2011

SHELL=/bin/sh

LIBDIR=lib
OBJDIR=lib/obj
SRCDIR=src
BUILDDIR=build
TOOLSDIR=tools
INCLUDEDIR=include

LIBS=$(wildcard $(LIBDIR)/*.a)
OBJS=$(wildcard $(OBJDIR)/*.o)
SRCS=$(wildcard $(SRCDIR)/*.c)

CC=arm-none-eabi-gcc
OB=arm-none-eabi-objcopy
CS=$(TOOLSDIR)/fw_checksummer/checksummer.py
CFLAGS=-I$(INCLUDEDIR) -std=gnu99 -g -Os -Wall -Wno-format -fno-builtin -mcpu=arm7tdmi-s -march=armv4t -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -DNDEBUG -Wno-unused-parameter -T$(OBJDIR)/firmware.ld -Wl,--cref -Wl,--gc-sections -Wl,-Map=$(BUILDDIR)/pulse_app.map
OBFLAGS=-O ihex

pulse_app:
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS),-\( $(LIBS) $(OBJS) $(SRCS) -lgcc -Wl,-\) 
	$(OB) $(OBFLAGS) $(BUILDDIR)/$@ $(BUILDDIR)/$@.hex
	$(CS) $(BUILDDIR)/$@.hex $(BUILDDIR)/$@.bin

.PHONY: clean

clean:
	rm -rf $(BUILDDIR)/*

all: pulse_app

