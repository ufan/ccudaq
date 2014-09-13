#/***************************************************
# File Name:	makefile
# Abstract:
# Author:	zhangzhelucky
# Update History:
#
#Before Record		old copies can be found in backup directory, it 
#			is so detailed and too long to make us confused.
#2013-04-09 19-14	make it simpler to read
#
#****************************************************/

CURRENTTIME	:=	$(shell date "+%y%m%d%H%M%S")
BACKUP_MAIN 	:=	_V3.5.0-build

LIBDIRS		:=	-L./libs
LIBS		:=	-lxx_usb -lncurses -lpthread
INCDIRS		:=	-I./include
FLAGS		:=	-O -Wall -fPIC -g $(INCDIRS) $(LIBDIRS) $(LIBS) -DNDEBUG
RFLAGS		:=      -Wl,-rpath=./libs

OBJECTS		:=	main.o manager.o ccu.o adc.o modul.o log.o display.o

daq:	$(OBJECTS)
	g++ -o daq $(OBJECTS) $(FLAGS) $(RFLAGS)

        #cp -r . ../backup2013daq/backup$(BACKUP_MAIN)$(CURRENTTIME)/
	rm -rf *~
	@-mkdir ./log
	@echo "Done"

main.o:	main.cpp
	g++ -c -g main.cpp

manager.o:	manager.cpp manager.h
	g++ -c -g manager.cpp

ccu.o:	ccu.cpp ccu.h
	g++ -c -g ccu.cpp

adc.o: adc.h adc.cpp modul.h
	g++ -c -g adc.cpp

modul.o:modul.cpp modul.h
	g++ -c -g modul.cpp

log.o:log.cpp log.h
	g++ -c -g log.cpp

display.o:display.cpp display.h global.h
	g++ -c -g display.cpp

.PHONY:clean
clean:
	rm -fr daq $(OBJECTS) *.gch log/

run:
	./daq

