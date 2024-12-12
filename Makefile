CC=gcc
CFLAGS=-O0
APP=print_stack
ALL_APPS=get_stack print_stack

OBJS_PRINT_STACK=print_stack.o

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
    OBJS+=get_stack_args.o
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

