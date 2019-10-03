TARGET ?= spp
PREFIX ?= /usr/local/bin

OBJECTS=
include src/subdir.mk

all: $(OBJECTS)
	$(info Building executable '$(TARGET)' ...)
	@$(CC) $(OBJECTS) -o '$(TARGET)'
#all

clean:
	$(info Cleaning...)
	@rm -vrf bin '$(TARGET)'
#clean

install: all
	$(info Installing executable '$(TARGET)' into '$(PREFIX)' ...)
	install '$(TARGET)' '$(PREFIX)'
#install

deinstall:
	$(info Deinstalling '$(PREFIX)/$(TARGET)' ...)
	@rm -vf '$(PREFIX)/$(TARGET)'
#deinstall

.PHONY: all clean install deinstall
