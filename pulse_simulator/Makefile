# Allerta Inc Pulse Simulator Build System
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

#TODO: Figure out the proper way to grab these libss
PULSE_LIBS=-lapp_common -lpulse_os -lpulse_protocol -lpulse_widgets
EXTERNAL_LIBS=-lgtk-x11-2.0

CC=gcc
CFLAGS=-I$(INCLUDEDIR) -std=gnu99 -g -Wall $(OBJS) -L$(LIBDIR) -D PULSE_SIMULATOR

pulse_app:
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS) $(PULSE_LIBS) $(EXTERNAL_LIBS) $(SRCS)

.PHONY: clean

clean:
	rm -rf $(BUILDDIR)/*

all: pulse_app

