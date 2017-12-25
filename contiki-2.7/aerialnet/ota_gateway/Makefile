DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = gateway

ifndef TARGET
TARGET=ez240
endif

all: $(CONTIKI_PROJECT)

CONTIKI_TARGET_SOURCEFILES += deluge_tx.c serial_thread.c

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
