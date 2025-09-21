CC        ?= cc
AR        ?= ar
RANLIB    ?= ranlib
CFLAGS    ?= -std=c11 -O2 -Wall -Wextra -Werror -pedantic
LDFLAGS   ?=

PREFIX    ?= /usr/local
DESTDIR   ?=
BUILD_DIR ?= build

LIB        := $(BUILD_DIR)/libvec.a
LIB_OBJS   := $(BUILD_DIR)/vec.o

TEST_SRC   := tests/test_vec.c
TEST_BIN   := $(BUILD_DIR)/test_vec

VALGRIND ?= valgrind -q --leak-check=full --show-leak-kinds=all --error-exitcode=99

.PHONY: all lib test clean install

# Default: build the library
all: lib

# Build the static library
lib: $(LIB)

$(LIB): $(LIB_OBJS)
	$(AR) rcs $@ $^
	@command -v $(RANLIB) >/dev/null 2>&1 && $(RANLIB) $@ || true

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/vec.o: vec.c vec.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c vec.c -o $@

# Tests: build and run pipeline
$(TEST_BIN): $(TEST_SRC) tests/test.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) -lvec -o $@ $(LDFLAGS)

test: $(TEST_BIN)
	@if command -v valgrind >/dev/null 2>&1; then \
		$(VALGRIND) $(TEST_BIN); \
	else \
		echo "valgrind not found, running tests without it"; \
		$(TEST_BIN); \
	fi

clean:
	rm -rf $(BUILD_DIR)

install: $(LIB) vec.h
	install -d $(DESTDIR)$(PREFIX)/include
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 vec.h $(DESTDIR)$(PREFIX)/include/vec.h
	install -m 644 $(LIB) $(DESTDIR)$(PREFIX)/lib/libvec.a
