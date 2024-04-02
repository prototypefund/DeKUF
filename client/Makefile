.DEFAULT_GOAL := all

define clang_format
	find src tests \
		\( -name "*.cpp" -or -name "*.hpp" \) \
	-exec clang-format $(1) {} \;
endef

.PHONY: all
all:
	@echo "TODO"

.PHONY: lint
lint:
	@$(call clang_format,"--dry-run")

.PHONY: lint-fix
lint-fix:
	@$(call clang_format,"-i")
