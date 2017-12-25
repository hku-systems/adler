CONTIKI = ../..
ifndef TARGET
TARGET=ez240
endif

MOTES=/dev/ttyUSB1

CONTIKI_TARGET_SOURCEFILES += codeload.c

all:OTA  #rt-leds test-button test-cfs tcprudolph0

APPS = deluge

include $(CONTIKI)/Makefile.include
