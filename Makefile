CC=gcc
CFLAGS=-Wall -Werror -O0 -m64 -mpreferred-stack-boundary=4 \
       -fno-stack-protector -DCONFIG_DEMO
LDFLASG=-static -z execstack
APP=naive
ALL_APPS=get_stack print_stack naive

OBJS_PRINT_STACK=print_stack.o
OBJS_NAIVE=naive.o

MACHINE:="$(shell gcc -dumpmachine)"
SUPPORTED:="x86_64-linux-gnu"

define assert_machine
$(if $(filter $(MACHINE),$(SUPPORTED)), \
	@echo building for: $(MACHINE), \
	$(error build target: $(MACHINE), supported targets: $(SUPPORTED)))
endef

# configuration
# =============
CONFIG_GET_STACK=n
CONFIG_DEBUG=n

ifeq ($(CONFIG_GET_STACK),y)
    OBJS_NAIVE+=get_stack_args.o
    CFLAGS+=-DGET_STACK
endif

ifeq ($(CONFIG_DEBUG),y)
    CFLAGS+=-ggdb -DDEBUG
endif

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

%_pre.c: %.c
	$(CPP) -x c -o $@ $<

.PHONY: all verify_targets clean cleanall

all: $(APP)

naive: $(OBJS_NAIVE) verify_targets
	$(CC) -o $@ $(LDFLASG) $(OBJS_NAIVE)

print_stack: $(OBJS_PRINT_STACK) verify_targets
	$(CC) -o $@ $(LDFLASG) $<

verify_targets:
	$(call assert_machine)

clean:
	@echo "removing executables"
	@rm -f $(ALL_APPS)
	@echo "removing object files"
	@rm -f *.o

cleanall: clean
	@echo "removing pre compilation files"
	@rm -f *_pre.c
	@echo "removing tag file"
	@rm -f tags

