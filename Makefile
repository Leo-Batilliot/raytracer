# =====================================================================
# Makefile — thin wrapper around CMake.
#
# Required rules by the Epitech subject: all, re, clean, fclean.
# We default to a Release build; use `make BUILD_TYPE=Debug` for sanitizers.
# =====================================================================

BUILD_DIR   ?= build
BUILD_TYPE  ?= Release
JOBS        ?= $(shell nproc 2>/dev/null || echo 4)

CMAKE       ?= cmake

# Silence "gmake[N]: Entering/Leaving directory" noise from the generated
# sub-Makefiles that CMake produces under build/. Propagates to recursive
# make invocations via MAKEFLAGS.
MAKEFLAGS   += --no-print-directory

.PHONY: all re clean fclean debug release help

all: $(BUILD_DIR)/CMakeCache.txt
	@$(CMAKE) --build $(BUILD_DIR) -j $(JOBS) -- --no-print-directory

$(BUILD_DIR)/CMakeCache.txt:
	@$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

debug:
	@$(MAKE) BUILD_TYPE=Debug all

release:
	@$(MAKE) BUILD_TYPE=Release all

clean:
	@if [ -d $(BUILD_DIR) ]; then $(CMAKE) --build $(BUILD_DIR) --target clean; fi

fclean:
	@rm -rf $(BUILD_DIR)
	@rm -f raytracer
	@rm -f plugins/*.so plugins/*.dylib plugins/*.dll

re: fclean all

help:
	@echo "Raytracer Makefile — wrapper around CMake"
	@echo ""
	@echo "Targets:"
	@echo "  all       (default) Configure + build in $(BUILD_DIR)/ ($(BUILD_TYPE))"
	@echo "  debug     Build with -O0 -g + Address/UB sanitizers"
	@echo "  release   Build with -O3 -DNDEBUG"
	@echo "  clean     Remove build artifacts (keeps cmake cache)"
	@echo "  fclean    Remove build/ + raytracer binary + plugins/*.so"
	@echo "  re        fclean + all"
	@echo ""
	@echo "Variables (override with e.g. make BUILD_TYPE=Debug):"
	@echo "  BUILD_DIR   = $(BUILD_DIR)"
	@echo "  BUILD_TYPE  = $(BUILD_TYPE)"
	@echo "  JOBS        = $(JOBS)"
