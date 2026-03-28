# examples/build.mk

EXAMPLES_SRC_DIRS := $(wildcard examples/*/)
EXAMPLES_NAMES    := $(patsubst examples/%/,%,$(EXAMPLES_SRC_DIRS))
EXAMPLES_BINS     := $(addprefix $(OUT_DIR)/examples/,$(addsuffix $(EXE_EXT),$(EXAMPLES_NAMES)))

ifeq ($(LINK),static)
    EXAMPLE_LIBS := $(STATIC_LIB)
else
    EXAMPLE_LIBS := -L$(OUT_DIR) -l$(LIB_NAME)
    ifeq ($(PLATFORM),posix)
        EXAMPLE_LIBS += -Wl,-rpath,'$$ORIGIN/..'
    endif
endif

.PHONY: examples clean-examples

examples: $(EXAMPLES_BINS)

$(OUT_DIR)/examples/%$(EXE_EXT): examples/%/main.cpp $(TARGETS)
	@$(call MD,$(@D))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(EXAMPLE_LIBS) $(LDFLAGS) $(LDLIBS)

clean-examples:
	@$(call RD,$(OUT_DIR)/examples)
