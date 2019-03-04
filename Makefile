FILES=$(wildcard *.c)
OBJS=$(FILES:%.c=obj/%.o)
DEPS=$(FILES:%.c=dep/%.d)

MAKEFLAGS += --no-print-directory

ARCH=$(shell uname -p)
ifeq ($(ARCH),unknown)
ARCH=$(shell uname -m)
endif
ifeq ($(ARCH),unknown)
ARCH=$(shell arch)
endif

CFLAGS=-Wall -D$(ARCH)_ARCH -D_REENTRANT -g -pipe

CC=gcc

DEPENDS=$(DEPS)

all:lib/libmthread.a

lib/libmthread.a:$(OBJS)
	@echo "Generate $@"
	@ar rcu $@ obj/*.o

$(DEPS): dep/%.d : %.c 
	@echo "Generate $@"
	@$(CC) $(CFLAGS) -I. -M $(patsubst dep/%.d,%.c,$@) | \
	sed 's,$(patsubst dep/%.d,%.o,$@):,$(patsubst %.d,%.o,$@) $@:,g' > $@ 2> /dev/null

$(OBJS):obj/%.o : dep/%.d
	@echo "Generate $@"
	@$(CC) $(CFLAGS) -I. -c $(patsubst obj/%.o,%.c,$@) -o $@

clean:
	@echo "Cleanning"
	@rm dep/* lib/* obj/* *~; printf ""

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif
