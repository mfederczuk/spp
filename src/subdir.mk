#src/subdir.mk

OBJECTS += \
bin/directives.o \
bin/main.o       \
bin/spp.o        \
bin/utils.o

bin/%.o: src/%.c
	$(info Building file '$@' ...)
	@mkdir -p bin
	@$(CC) -Iinclude -c '$<' -o '$@'
#bin/%.o

src/%.c:
	$(error $@: no such file)
#src/%.c
