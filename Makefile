
# Makefile for Visual Studio C++ NMAKE

CC=cl.exe
CFLAGS= -nologo -EHsc -std:c++14 -Zi 

all: traceer.dll

clean:
	@if exist *~ del *~
	@for %i in ( traceer ) do @for %j in ( pdb dll ilk lib exp ) do @if exist %i.%j del %i.%j
	@for %i in ( vc140.pdb traceer.obj ) do @if exist %i del %i
traceer.dll traceer.lib: traceer.cpp traceer.h
	$(CC) $(CFLAGS) /LDd traceer.cpp 
