#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_ADDRESSES 6
#define SIZEOF_ADDR sizeof(uint64_t*)

#if defined(CONFIG_DEMO)
#define LENGTH_PADDING_1 9
#define LENGTH_SHELLCODE 18
#define LENGTH_NOP 1
#define LENGTH_PADDING_2 3
#define STACK_CONTENT_DST_STR "---"
#else
#define LENGTH_PADDING_1 3
#define LENGTH_SHELLCODE 9
#define LENGTH_NOP 25
#define LENGTH_PADDING_2 1
#define STACK_CONTENT_DST_STR "nop"
#endif

#define ARR_SIZE(_arr_) (sizeof(_arr_) / sizeof(_arr_[0]))

#define SET_EMPTY_CHAR(_arr_, _char_) do { \
	size_t size = ARR_SIZE(_arr_) - 1; \
	memset(_arr_, _char_, size); \
	_arr_[size] = 0; \
} while (0)

#if defined(DEBUG_SET_EMPTY)
static char c_debug = 'a';
#define SET_EMPTY(_arr_) do { \
	SET_EMPTY_CHAR(_arr_, c_debug); \
	c_debug++; \
} while (0)
#else
#define SET_EMPTY(_arr_) SET_EMPTY_CHAR(_arr_, ' ')
#endif

#define WRITE_FIELD_STR(_arr_, _offset_, _str_) do { \
	if (ARR_SIZE(_arr_) <= _offset_ + strlen(_str_)) { \
		printf("Error (line %d): field size (%ld) <= offset (%d) + " \
			"string length (%lu)\n", __LINE__, ARR_SIZE(_arr_), \
			_offset_, strlen(_str_)); \
	} else { \
		memcpy(_arr_ + _offset_, _str_, strlen(_str_)); \
	} \
} while (0)

#define WRITE_FIELD_ADDR(_arr_, _offset_, _addr_) do { \
	if (ARR_SIZE(_arr_) <= _offset_ + sizeof(_addr_)) { \
		printf("Error (line %d): field size (%ld) <= offset (%d) + " \
			"address length (%lu)\n", __LINE__, ARR_SIZE(_arr_), \
			_offset_, sizeof(_addr_)); \
	} else { \
		snprintf(_arr_ + _offset_, sizeof(_arr_) - _offset_, "%p", \
			(uint64_t*)_addr_); \
	} \
} while (0)

#if defined(DEBUG)

#define TEST_GDB_STACK 1

char *test[] = {
	"0", /* argv[0] */
	"0x00007fffffffd6f0", "0x00007fffffffd5c0", "0x00007fffffffd580",
#if (TEST_GDB_STACK)
	"0x00007fffffffd670", "0x00007fffffffd540", "0x00007fffffffd500",
#endif
};
#endif

enum stack {
	STACK_NO_GDB,
	STACK_GDB,
	STACK_NUM
};

enum stack_content {
	STACK_CONTENT_NONE_PRE,
	STACK_CONTENT_RETURN_ADDR,
	STACK_CONTENT_RBP,
	STACK_CONTENT_PADDING_1,
	STACK_CONTENT_SHELL_CODE,
	STACK_CONTENT_NOP,
#if defined(CONFIG_DEMO)
	STACK_CONTENT_PADDING_2,
#endif
	STACK_CONTENT_DST,
	STACK_CONTENT_FILL,
	STACK_CONTENT_RSB,
	STACK_CONTENT_NONE_POST,
};

enum position {
	POS_TOP,
	POS_BOTTOM,
	POS_COUNT
};

struct {
	struct {
		uint64_t rbp;
		uint64_t dst;
		uint64_t rsb;
		enum stack_content current_content;
		uint64_t next;
	} position[POS_COUNT];

	enum stack top;
	uint64_t offset;
	uint64_t current;

} stack_descriptor;

struct stack_entry {
	char desc[11];
	char stack[18];
};

struct line {
	struct stack_entry ngdb;
	char space1[4];
	char nop_overlap[4];
	char space2[2];
	struct stack_entry wgdb;
	char space3[3];
	char address[17];
};

static int is_single_stack;

static void usage(void)
{
	printf("usage:\n\n"
		"  print_stack "
		"<$rsb> "
		"<&dst> "
		"<$rbp>\n"
		"    or\n"
		"  print_stack "
		"<no-gdb $rsb> "
		"<no-gdb &dst> "
		"<no-gdb $rbp> "
		"<gdb $rsb> "
		"<gdb &dst> "
		"<gdb $rbp>\n"
	      );
}

static int stack_descriptor_init(int argc, char **argv)
{
	uint64_t addresses[NUM_ADDRESSES];
	char *err;
	int num_itertations;
	int i;

#if defined(DEBUG)
	argv = test;
	argc = ARR_SIZE(test);
#endif

	if (argc == 1 + NUM_ADDRESSES) {
		is_single_stack = 0;
		num_itertations = NUM_ADDRESSES;
	} else if (argc == 1 + NUM_ADDRESSES / 2) {
		is_single_stack = 1;
		num_itertations = NUM_ADDRESSES / 2;
	} else {
		usage();
		return -1;
	}

	for (i = 0; i < num_itertations; i++) {
		addresses[i] = strtoll(argv[i + 1], &err, 16);
		if (*err) {
			usage();
			return -1;
		}
	}

	if (is_single_stack) {
		stack_descriptor.position[POS_TOP].rbp = addresses[0];
		stack_descriptor.position[POS_TOP].dst = addresses[1];
		stack_descriptor.position[POS_TOP].rsb = addresses[2];
	} else {
		if (addresses[0] > addresses[3]) {
			stack_descriptor.top = STACK_NO_GDB;
			stack_descriptor.position[POS_TOP].rbp = addresses[0];
			stack_descriptor.position[POS_TOP].dst = addresses[1];
			stack_descriptor.position[POS_TOP].rsb = addresses[2];
			stack_descriptor.position[POS_BOTTOM].rbp =
				addresses[3];
			stack_descriptor.position[POS_BOTTOM].dst =
				addresses[4];
			stack_descriptor.position[POS_BOTTOM].rsb =
				addresses[5];
		} else {
			stack_descriptor.top = STACK_GDB;
			stack_descriptor.position[POS_TOP].rbp = addresses[3];
			stack_descriptor.position[POS_TOP].dst = addresses[4];
			stack_descriptor.position[POS_TOP].rsb = addresses[5];
			stack_descriptor.position[POS_BOTTOM].rbp =
				addresses[0];
			stack_descriptor.position[POS_BOTTOM].dst =
				addresses[1];
			stack_descriptor.position[POS_BOTTOM].rsb =
				addresses[2];
		}
	}

	stack_descriptor.current = stack_descriptor.position[POS_TOP].rbp +
		SIZEOF_ADDR;
	stack_descriptor.position[POS_TOP].current_content =
		STACK_CONTENT_RETURN_ADDR;
	stack_descriptor.position[POS_TOP].next =
		stack_descriptor.position[POS_TOP].rbp + SIZEOF_ADDR;

	if (!is_single_stack) {
		stack_descriptor.offset =
			(stack_descriptor.position[POS_TOP].rbp -
			 stack_descriptor.position[POS_BOTTOM].rbp) /
			SIZEOF_ADDR + 1;
		stack_descriptor.position[POS_BOTTOM].current_content =
			STACK_CONTENT_NONE_PRE;
		stack_descriptor.position[POS_BOTTOM].next =
			stack_descriptor.position[POS_TOP].next -
			(stack_descriptor.offset - 2) * SIZEOF_ADDR;
	}

	return 0;
}

static void clear_stack_entry(struct stack_entry *entry)
{
	SET_EMPTY(entry->desc);
	SET_EMPTY(entry->stack);
}

static void clear_line(struct line *l)
{
	memset(l, 0, sizeof(struct line));

	clear_stack_entry(&l->ngdb);
	SET_EMPTY(l->space1);
	SET_EMPTY(l->nop_overlap);
	SET_EMPTY(l->space2);
	clear_stack_entry(&l->wgdb);
	SET_EMPTY(l->space3);
	SET_EMPTY(l->address);
}

static void flush_line(struct line *l)
{
	printf("%s%s", l->ngdb.desc, l->ngdb.stack);
	printf("%s", l->space1);
	if (!is_single_stack) {
		printf("%s", l->nop_overlap);
		printf("%s", l->space2);
		printf("%s%s%s", l->wgdb.stack, "   ", l->wgdb.desc);
		printf("%s", l->space3);
	}
	printf("%s\n", l->address);
}

static void print_headers(void)
{
	struct line l;

	clear_line(&l);
	if (is_single_stack) {
		WRITE_FIELD_STR(l.ngdb.stack, 6, "STACK");
	} else {
		WRITE_FIELD_STR(l.ngdb.stack, 2, "STACK: NO GDB");
		WRITE_FIELD_STR(l.wgdb.stack, 1, "STACK: WITH GDB");
	}
	flush_line(&l);

	clear_line(&l);
	WRITE_FIELD_STR(l.ngdb.stack, 0, "=================");
	if (!is_single_stack)
		WRITE_FIELD_STR(l.wgdb.stack, 0, "=================");
	flush_line(&l);
}

static void set_stack_entry(struct stack_entry *entry, enum position pos)
{
	uint64_t next_decrease;

	switch (stack_descriptor.position[pos].current_content) {
	case STACK_CONTENT_NONE_PRE:
		next_decrease = 1;
		break;
	case STACK_CONTENT_RETURN_ADDR:
		WRITE_FIELD_STR(entry->stack, 0, "oooooooo oooooooo");
		WRITE_FIELD_STR(entry->desc, 0, "ret addr*");
		next_decrease = 1;
		break;
	case STACK_CONTENT_RBP:
		WRITE_FIELD_STR(entry->stack, 0, "_ _ _ _pad_ _ _ _");
		WRITE_FIELD_STR(entry->desc, 0, "$rbp");
		next_decrease = LENGTH_PADDING_1;
		break;
	case STACK_CONTENT_PADDING_1:
		WRITE_FIELD_STR(entry->stack, 7, "pad");
		next_decrease = LENGTH_SHELLCODE;
		break;
	case STACK_CONTENT_SHELL_CODE:
		WRITE_FIELD_STR(entry->stack, 8, "s");
		next_decrease = LENGTH_NOP;
		break;
	case STACK_CONTENT_NOP:
		WRITE_FIELD_STR(entry->stack, 7, "nop");
		next_decrease = LENGTH_PADDING_2;
		break;
#if defined(CONFIG_DEMO)
	case STACK_CONTENT_PADDING_2:
		WRITE_FIELD_STR(entry->stack, 7, "---");
		next_decrease = 1;
		break;
#endif
	case STACK_CONTENT_DST:
		WRITE_FIELD_STR(entry->stack, 0, "_ _ _ _" STACK_CONTENT_DST_STR
			"_ _ _ _");
		WRITE_FIELD_STR(entry->desc, 0, "&dst");
		next_decrease = (stack_descriptor.position[pos].dst -
			stack_descriptor.position[pos].rsb) / SIZEOF_ADDR - 1;
		break;
	case STACK_CONTENT_FILL:
		WRITE_FIELD_STR(entry->stack, 8, "x");
		next_decrease = 1;
		break;
	case STACK_CONTENT_RSB:
		WRITE_FIELD_STR(entry->stack, 0, "_ _ _ _ x _ _ _ _");
		WRITE_FIELD_STR(entry->desc, 0, "$rsb");
		next_decrease = 1;
		break;
	case STACK_CONTENT_NONE_POST:
	default:
		next_decrease = 0;
		break;
	};

	if (stack_descriptor.current == stack_descriptor.position[pos].next) {
		stack_descriptor.position[pos].current_content++;
		stack_descriptor.position[pos].next -= next_decrease *
			SIZEOF_ADDR;
	}
}

static void set_stack_address(struct line *l)
{
	uint64_t address_to_print;

	if (stack_descriptor.current == 
			stack_descriptor.position[POS_TOP].rbp + SIZEOF_ADDR ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_TOP].rbp ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_TOP].dst ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_TOP].rsb ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_BOTTOM].rbp +
			SIZEOF_ADDR ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_BOTTOM].rbp ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_BOTTOM].dst ||
		stack_descriptor.current ==
			stack_descriptor.position[POS_BOTTOM].rsb) {
		address_to_print = stack_descriptor.current;
	} else {
		address_to_print = 0;
	}

	if (address_to_print) {
		snprintf(l->address, sizeof(l->address), "%lx",
			address_to_print);
	}
}

static void set_overlap(struct line *l)
{
	char c;
	size_t str_len = sizeof(l->nop_overlap) - 1;

	if (is_single_stack) {
		if (stack_descriptor.position[POS_TOP].current_content ==
				STACK_CONTENT_NOP) {
			snprintf(l->address, sizeof(l->address), "%lx",
				stack_descriptor.current);
		}
	} else {
		if ((stack_descriptor.position[POS_TOP].current_content ==
				STACK_CONTENT_NOP &&
			(stack_descriptor.position[POS_BOTTOM].current_content==
				STACK_CONTENT_NOP ||
			stack_descriptor.position[POS_BOTTOM].current_content ==
				STACK_CONTENT_DST)) ||
			(stack_descriptor.position[POS_BOTTOM].current_content==
				STACK_CONTENT_NOP &&
			(stack_descriptor.position[POS_TOP].current_content ==
				STACK_CONTENT_NOP ||
			stack_descriptor.position[POS_TOP].current_content ==
				STACK_CONTENT_DST))) {

			c = '#';
			snprintf(l->address, sizeof(l->address), "%lx",
					stack_descriptor.current);
		} else {
			c = ' ';
		}

		memset(l->nop_overlap, c, str_len);
		l->nop_overlap[str_len] = 0;
	}
}

static void print_line(void)
{
	struct line l;

	clear_line(&l);

	set_overlap(&l);

	if (is_single_stack) {
		set_stack_entry(&l.ngdb, POS_TOP);
	} else if (stack_descriptor.top == STACK_NO_GDB) {
		set_stack_entry(&l.ngdb, POS_TOP);
		set_stack_entry(&l.wgdb, POS_BOTTOM);
	} else {
		set_stack_entry(&l.wgdb, POS_TOP);
		set_stack_entry(&l.ngdb, POS_BOTTOM);
	}

	set_stack_address(&l);

	flush_line(&l);
}

static void print_columns(void)
{
	uint64_t end = stack_descriptor.position[is_single_stack ?
		POS_TOP : POS_BOTTOM].rsb;

	do {
		print_line();
		stack_descriptor.current -= SIZEOF_ADDR;
	} while (end <= stack_descriptor.current);
}

int main(int argc, char **argv)
{
	if (stack_descriptor_init(argc, argv))
		return -1;

	print_headers();
	print_columns();
	return 0;
}

