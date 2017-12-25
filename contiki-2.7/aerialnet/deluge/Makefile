ifndef TARGET
TARGET=ez240
endif

CONTIKI_PROJECT = test-deluge

all: $(CONTIKI_PROJECT)

APPS = deluge

%.tgz: %.ihex
	mkdir $(basename $<) ; \
	mv $< $(basename $<) ; \
	echo $(basename $<)/$(basename $<).ihex 600 > $(basename $<)/runfile ; \
	tar czf $@ $(basename $<)

%.class: %.java
	javac $(basename $<).java

viewrssi: ViewRSSI.class
	make login | java ViewRSSI
CONTIKI = ../..
include $(CONTIKI)/Makefile.include
