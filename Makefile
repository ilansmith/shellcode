CC=gcc
CFLAGS=-O0
APP=get_stack

OBJS=get_stack.o

MACHINE:="$(shell gcc -dumpmachine)"
SUPPORTED:="x86_64-linux-gnu"

define assert_machine
$(if $(filter $(MACHINE),$(SUPPORTED)), \
	@echo building for: $(MACHINE), \
	$(error build target: $(MACHINE), supported targets: $(SUPPORTED)))
endef

# configuration
# =============
CONFIG_DEBUG=y

ifeq ($(CONFIG_DEBUG),y)
    CFLAGS+=-ggdb
endif

%: %.c
	$(GCC) -o $@ $(CFLAGS) $<

%_pre.c: %.c
	$(CPP) -x c -o $@ $<

.PHONY: all verify_targets clean cleanall

all: $(APP)

$(APP): $(OBJS) verify_targets
	$(CC) -o $@ $(OBJS)

verify_targets:
	$(call assert_machine)

clean:
	@echo "removing executables"
	@rm -f $(APP)
	@echo "removing object files"
	@rm -f *.o

cleanall: clean
	@echo "removing pre compilation files"
	@rm -f *_pre.c
	@echo "removing tag file"
	@rm -f tags

