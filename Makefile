CXX ?= g++
AR ?= ar
BUILD ?= release
LINK ?= dynamic
LIBTYPE ?= auto

ifeq ($(OS),Windows_NT)
	PLATFORM := windows
	
	# commands
	MD = if not exist "$(subst /,\,$(patsubst %/,%,$(1)))" mkdir "$(subst /,\,$(patsubst %/,%,$(1)))"
	RM = if exist "$(subst /,\,$(1))" del /Q /F "$(subst /,\,$(1))"
	RD = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
	CP = xcopy /Y /E /I "$(subst /,\,$(1))" "$(subst /,\,$(2))"
	
	# extensions
	EXE_EXT := .exe
	LIB_EXT := .dll
	STATIC_LIB_EXT := .lib
	
	# sources
	SOURCES := $(shell powershell -NoProfile -Command "Get-ChildItem -Path 'src' -Recurse -Filter *.cpp | Resolve-Path -Relative | ForEach-Object { $$_ -replace '\\\\','/' -replace '^\./','' }")
else
	PLATFORM := posix
	
	# commands
	MD = mkdir -p "$(1)"
	RM = rm -f "$(1)"
	RD = rm -rf "$(1)"
	CP = cp -r "$(1)" "$(2)"
	
	# extensions
	EXE_EXT := .elf
	LIB_EXT := .so
	STATIC_LIB_EXT := .a

	# sources
	SOURCES := $(shell find src -type f -name "*.cpp")
endif

ROOT_DIR     := .
EXTERNAL_DIR := extern

BUILD_DIR  := build/$(BUILD)
OUT_DIR    := out/$(BUILD)-$(LINK)
ASSETS_DIR := assets

LIB_NAME   := cx-engine
STATIC_LIB := $(OUT_DIR)/lib$(LIB_NAME)$(STATIC_LIB_EXT)
SHARED_LIB := $(OUT_DIR)/lib$(LIB_NAME)$(LIB_EXT)

CXXSTD := -std=c++20
WARNINGS := -Wall
PIC := -fPIC

COMMON_CXXFLAGS := $(CXXSTD) $(WARNINGS) $(PIC)

ifeq ($(BUILD),debug)
	CXXFLAGS := $(COMMON_CXXFLAGS) -O0 -g -DCX_DEBUG
else ifeq ($(BUILD),release)
	CXXFLAGS := $(COMMON_CXXFLAGS) -O3 -DNDEBUG -DCX_RELEASE
else
	$(error Unknown BUILD=$(BUILD))
endif

ifeq ($(LINK),static)
	CXXFLAGS += -DSFML_STATIC
endif

INCLUDES := \
	-I$(ROOT_DIR)/include \
	-I$(EXTERNAL_DIR)/ \
	-I$(EXTERNAL_DIR)/rapidjson/include \

ifeq ($(PLATFORM),posix)
ifeq ($(LINK),static)
	LDFLAGS += -static-libstdc++ -static-libgcc
	SFML_LIBS := \
		-lsfml-graphics-s \
		-lsfml-window-s \
		-lsfml-audio-s \
		-lsfml-system-s \
		-lharfbuzz -lfreetype \
		-Wl,-Bstatic -lFLAC -lvorbis -lvorbisenc -lvorbisfile -logg -Wl,-Bdynamic \
		-lasound -ludev -lpthread \
		-lGL -lX11 -lXrandr -lXcursor -lXi
else
	SFML_LIBS := -lsfml-window -lsfml-audio -lsfml-graphics -lsfml-system -lGL
endif
else
	SFML_DIR ?= C:/SFML
	INCLUDES += -I$(SFML_DIR)/include
	LDFLAGS  += -L$(SFML_DIR)/lib

ifeq ($(LINK),static)
	SFML_LIBS := \
		-lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s \
		-lopengl32 -lgdi32 -lwinmm -lfreetype
else
	SFML_LIBS := -lsfml-system -lsfml-window -lsfml-audio -lsfml-graphics
endif
endif

EXTERN_LIBS := xxhash
XXHASH_LIBS := -L$(EXTERNAL_DIR)/xxhash -lxxhash

LDLIBS := \
	$(SFML_LIBS) \
	$(XXHASH_LIBS)

OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

PREFIX ?= /usr/local

.PHONY: all clean dirs libs $(EXTERN_LIBS) install uninstall

ifeq ($(LIBTYPE),auto)
    ifeq ($(LINK),static)
        TARGETS := $(STATIC_LIB)
    else
        TARGETS := $(STATIC_LIB) $(SHARED_LIB)
    endif
else ifeq ($(LIBTYPE),static)
    TARGETS := $(STATIC_LIB)
else ifeq ($(LIBTYPE),shared)
    TARGETS := $(SHARED_LIB)
else ifeq ($(LIBTYPE),both)
    TARGETS := $(STATIC_LIB) $(SHARED_LIB)
else
    $(error Unknown LIBTYPE=$(LIBTYPE). Supported: auto, static, shared, both)
endif

all: dirs libs $(TARGETS)

dirs:
	@$(call MD,$(BUILD_DIR))
	@$(call MD,$(OUT_DIR))

libs: $(EXTERN_LIBS)

$(EXTERN_LIBS):
	$(MAKE) -C $(EXTERNAL_DIR)/$@ $(JFLAG) BUILD=$(BUILD)

$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(SHARED_LIB): $(OBJECTS) libs
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	@$(call MD,$(@D))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

install: all
	@echo "Installing to $(DESTDIR)$(PREFIX)..."
	@$(call MD,$(DESTDIR)$(PREFIX)/lib)
	@$(call MD,$(DESTDIR)$(PREFIX)/include)
	@for target in $(TARGETS); do \
		$(call CP,$$target,$(DESTDIR)$(PREFIX)/lib/); \
	done
	@$(call CP,include/cx-engine,$(DESTDIR)$(PREFIX)/include/)

uninstall:
	@echo "Uninstalling from $(DESTDIR)$(PREFIX)..."
	@$(call RM,$(DESTDIR)$(PREFIX)/lib/lib$(LIB_NAME)$(STATIC_LIB_EXT))
	@$(call RM,$(DESTDIR)$(PREFIX)/lib/lib$(LIB_NAME)$(LIB_EXT))
	@$(call RD,$(DESTDIR)$(PREFIX)/include/cx-engine)

clean:
	@$(call RD,build)
	@$(call RD,out)
	@for lib in $(EXTERN_LIBS); do \
		$(MAKE) -C $(EXTERNAL_DIR)/$$lib clean || true; \
	done
