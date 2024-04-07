.DEFAULT_GOAL := all

define clang_format
	find src tests \
		\( -name "*.cpp" -or -name "*.hpp" \) \
		-exec clang-format $(1) {} +
endef

.PHONY: all
all: build
	make -C build

.PHONY: test
test:
	make -C build test

build:
	cmake -B build

.PHONY: lint
lint:
	$(call clang_format,--dry-run -Werror)
	run-clang-tidy -q -p build

.PHONY: format
format:
	$(call clang_format,-i)

.PHONY: run-daemon
run-daemon:
	build/dekuf-client

.PHONY: run-ui
run-ui:
	build/dekuf-client-ui

.PHONY: clean
clean:
	$(RM) -r build
