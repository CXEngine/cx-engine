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

define ASSET_RULE
$$(OUT_DIR)/../examples/$(1).cxpk: examples/$(1)/assets
	@$$(call MD,$$(@D))
	$$(CXPK) pack $$< $$@
endef

$(foreach name,$(EXAMPLES_NAMES),$(eval $(call ASSET_RULE,$(name))))

define BINARY_RULE
$$(OUT_DIR)/examples/$(1)$$(EXE_EXT): examples/$(1)/main.cpp $(TARGETS) $$(if $$(wildcard examples/$(1)/assets),$$(OUT_DIR)/../examples/$(1).cxpk)
	@$$(call MD,$$(@D))
	$$(CXX) $$(CXXFLAGS) $$(INCLUDES) $$< -o $$@ $$(EXAMPLE_LIBS) $$(LDFLAGS) $$(LDLIBS)
endef

$(foreach name,$(EXAMPLES_NAMES),$(eval $(call BINARY_RULE,$(name))))

clean-examples:
	@$(call RD,$(OUT_DIR)/examples)
