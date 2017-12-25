#DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI = ../..
ifndef TARGET
TARGET=ez240
endif

all: test-deluge#rt-leds test-button test-cfs tcprudolph0

CONTIKI_TARGET_SOURCEFILES += deluge_rx.c

%.tgz: %.ihex
	mkdir $(basename $<) ; \
	mv $< $(basename $<) ; \
	echo $(basename $<)/$(basename $<).ihex 600 > $(basename $<)/runfile ; \
	tar czf $@ $(basename $<)

%.class: %.java
	javac $(basename $<).java

viewrssi: ViewRSSI.class
	make login | java ViewRSSI

include $(CONTIKI)/Makefile.include
