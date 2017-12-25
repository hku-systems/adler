DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = rssi-produce
all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1

CONTIKI = ../..



viewrssi: ViewRSSI.class
	make login | java ViewRSSI

include $(CONTIKI)/Makefile.include
