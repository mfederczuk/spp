# C/C++ Makefile Template for applications and libraries.
# Copyright (C) 2020 Michael Federczuk
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# === user definitions ======================================================= #

SOFTWARE = exe

TARGET = spp
PACKAGE = $(TARGET)

SRC = src
BIN := bin

CCFLAGS  = -Iinclude -std=c11 \
           -Wall -Wextra -pedantic -Wpedantic -pedantic-errors -Werror=pedantic

-include debug_build.mk
ifneq "$(DEBUG_BUILD)" "1"
 BIN := $(BIN)/release
 CCFLAGS += -DNDEBUG -O3
else
 BIN := $(BIN)/debug
endif

# === colors ================================================================= #

ifneq "$(NO_COLOR)" "1"
 # reset:      0
 # bold:       1
 # italic:     3
 # underline:  4
 # black:     30  |  bright black:   90
 # red:       31  |  bright red:     91
 # green:     32  |  bright green:   92
 # yellow:    33  |  bright yellow:  93
 # blue:      34  |  bright blue:    94
 # magenta:   35  |  bright magenta: 95
 # cyan:      36  |  bright cyan:    96
 # white:     37  |  bright white:   97
 override _ascii_esc = $(shell printf '\033[$(1)m')

 override _green_clr_fxn      := 32
 override _yellow_clr_fxn     := 33
 override _blue_clr_fxn       := 34
 override _magenta_clr_fxn    := 35
 override _bright_red_clr_fxn := 91
 override _bold_fxn           := 1

 reset_fx        := $(call _ascii_esc,0)
 error_fx        := $(call _ascii_esc,$(_bright_red_clr_fxn);$(_bold_fxn))
 warning_fx      := $(call _ascii_esc,$(_yellow_clr_fxn))
 object_build_fx := $(call _ascii_esc,$(_blue_clr_fxn))
 target_build_fx := $(call _ascii_esc,$(_blue_clr_fxn);$(_bold_fxn))
 test_build_fx   := $(call _ascii_esc,$(_magenta_clr_fxn);$(_bold_fxn))
 install_fx      := $(call _ascii_esc,$(_green_clr_fxn))
 uninstall_fx    := $(call _ascii_esc,$(_bright_red_clr_fxn))
 clean_fx        := $(call _ascii_esc,$(_bright_red_clr_fxn))
endif

# === preconditions ========================================================== #

override EXE_SOFTWARE := exe
override LIB_SOFTWARE := lib

# prevent make from automatically building object files from source files
.SUFFIXES:

# just for debugging purposes
-include _debug.mk

# check if the SOFTWARE variable is defined
ifndef SOFTWARE
 $(error $(error_fx)SOFTWARE is not defined$(reset_fx))
endif
override SOFTWARE := $(strip $(SOFTWARE))
# check if the Makefile has been configured
# we're comparing every configuration variable with their pre-defined value
ifeq "$(SOFTWARE),$(TARGET),$(PACKAGE),$(SRC),$(SRC_MAIN),$(SRC_TEST),$(BIN),$(INC),$(LINKS),$(LINK_DIRS),$(TEST),$(MAIN),$(CCFLAGS),$(CXXFLAGS)" \
     "exe|lib,,$(TARGET),,src/main,src/test,bin,include/$(PACKAGE),,,,,-Iinclude -std=c17   -Wall -Wextra,-Iinclude -std=c++17 -Wall -Wextra"
 $(error $(error_fx)Makefile is not configured$(reset_fx))
endif
# check if a supported SOFTWARE type has been given
ifneq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 ifneq "$(SOFTWARE)" "$(LIB_SOFTWARE)"
  $(error $(error_fx)Software type ("$(SOFTWARE)") is unknown$(reset_fx))
 endif
endif

# check if the TARGET variable is defined
ifndef TARGET
 $(error $(error_fx)TARGET is not defined$(reset_fx))
endif
override TARGET := $(strip $(TARGET))

# check if the SRC, SRC_MAIN and SRC_TEST variables are defined
ifdef SRC_MAIN
 # SRC_MAIN is defined

 # check if SRC_TEST is defined
 # if SRC_MAIN is defined, SRC_TEST must also be defined
 ifndef SRC_TEST
  $(error $(error_fx)SRC_MAIN is defined but SRC_TEST is not defined. \
          If you don't want to use tests, \
          consider using SRC instead of SRC_MAIN$(reset_fx))
 endif
 override SRC_TEST := $(strip $(SRC_TEST))

 # check if SRC_MAN and SRC_TEST are equal
 # SRC_MAIN and SRC_TEST are not allowed to be equal, there's no way good way to
 # hold main source files and test source files apart
 ifeq "$(SRC_MAIN)" "$(SRC_TEST)"
  $(error $(error_fx)SRC_MAIN and SRC_TEST are equal. \
          If you don't want to use tests, \
          consider using just SRC instead of SRC_MAIN and SRC_TEST$(reset_fx))
 endif
 override SRC_MAIN := $(strip $(SRC_MAIN))

 # make sure that, if building an executable, MAIN is defined
 ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
  ifndef MAIN
   $(warning $(warning_fx)SRC_TEST is defined but MAIN is not defined. \
             Tests will not be linked with the object files$(reset_fx))
  endif
 endif

 # check if the SRC variable defined
 # just give a warning if it is, we ignore it anyway
 ifdef SRC
  $(warning $(warning_fx)SRC is ignored if SRC_MAIN is defined. \
            Consider removing it$(reset_fx))
 endif
else
 # SRC_MAIN is not defined

 # check if the SRC variable is defined
 # we obviously can't do anything if neither SRC nor SRC_MAIN are defined
 ifndef SRC
  $(error $(error_fx)Neither SRC nor SRC_MAIN are defined$(reset_fx))
 endif
 override SRC_MAIN := $(strip $(SRC))

 # check if the SRC_TEST variable is defined
 # if we're using SRC, we don't want tests. give a warning; SRC_TEST is ignored
 ifdef SRC_TEST
  $(warning $(warning_fx)SRC_TEST is ignored if SRC_MAIN is not defined and \
            SRC is defined. \
            Consider removing it$(reset_fx))
 endif
 override SRC_TEST := /dev/null
endif

# note about SRC, SRC_MAIN and SRC_TEST:
#  internally, we always use SRC_MAIN and SRC_TEST
#  if tests are supposed to be disabled, SRC_TEST will be set to "/dev/null"

# check if the main and test source directories even exist and if they are
# directories
ifeq "$(shell test -e '$(SRC_MAIN)' || echo x)" "x"
 $(error $(error_fx)Source directory does not exist$(reset_fx))
endif
ifeq "$(shell test -d '$(SRC_MAIN)' || echo x)" "x"
 $(error $(error_fx)Specified source directory is not a directory$(reset_fx))
endif
# don't check if test source directory if disabled
ifneq "$(SRC_TEST)" "/dev/null"
 ifeq "$(shell test -e '$(SRC_TEST)' || echo x)" "x"
  $(error $(error_fx)Test source directory does not exist$(reset_fx))
 endif
 ifeq "$(shell test -d '$(SRC_TEST)' || echo x)" "x"
  $(error $(error_fx)Specified test source directory is not a directory$(reset_fx))
 endif
endif

# check if the BIN variable is defined
ifndef BIN
 $(error $(error_fx)BIN is not defined$(reset_fx))
endif
override BIN := $(strip $(BIN))
# checking if BIN is project root
ifeq "$(BIN)" "."
 $(error $(error_fx)BIN is not allowed to be the root directory$(reset_fx))
endif

# check if the INC variable is defined, but only do it if we're not bulding an
# executable
ifneq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 ifndef INC
  $(error $(error_fx)INC is not defined$(reset_fx))
 endif
 override INC := $(strip $(INC))
endif

# warnings about LINKS and LINK_DIRS
ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 # for executables

 # check if LINK_DIRS is defined but LINKS isn't
 ifdef LINK_DIRS
  ifndef LINKS
   $(warning $(warning_fx)LINK_DIRS is defined, but LINKS isn't. \
             Specifying link directories without links doesn't do anything. \
             Consider either removing LINK_DIRS or defining LINKS$(reset_fx))
  endif
 endif
else
 # for libraries

 # we can't link anything if we're building a lib ourselves, so give out a
 # warning if LINKS or LINK_DIRS are defined
 ifdef LINKS
  $(warning $(warning_fx)LINKS is defined, but is ignored when building a \
            library. Consider removing LINKS$(reset_fx))
 endif
 ifdef LINK_DIRS
  $(warning $(warning_fx)LINK_DIRS is defined, but is ignored when building a \
            library. Consider removing LINK_DIRS$(reset_fx))
 endif
endif

# check if TEST is not defined if SRC_TEST is defined
ifneq "$(SRC_TEST)" "/dev/null"
 ifndef TEST
  $(error $(error_fx)SRC_TEST is defined but TEST is not defined. \
          If you don't want to use tests, \
          consider using just SRC instead of SRC_MAIN and SRC_TEST$(reset_fx))
 endif
 override TEST := $(strip $(TEST))
endif

# === variables ============================================================== #

# conventional make variables
SHELL = /bin/sh
prefix      = /usr/local
exec_prefix = $(prefix)
bindir      = $(exec_prefix)/bin
includedir  = $(prefix)/include
libdir      = $(exec_prefix)/lib

# normally, shared object files also have the .o extension, to hold them apart
# we're going to use .so (which literally stands for shared object).
# it's important that these two variables are different
shared_object_ext = so
static_object_ext = o

# better not change those
shared_lib_prefix = lib
static_lib_prefix = lib
shared_lib_suffix = .so
static_lib_suffix = .a

# unix like executables usually don't have a suffix, if you want you can change
# that
exe_prefix =
exe_suffix =

# specifically for test executables
test_prefix = $(exe_prefix)
test_suffix = _test$(exe_suffix)

# in case these are not defined for some reason
CC      ?= cc
CXX     ?= c++
AR      ?= ar
INSTALL ?= install

# === custom functions ======================================================= #

# checks if argument 1 and 2 are equal
override _eq = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))

# gets the target executable name of the test tuple
override _test_target = $(word 1,$(subst :, ,$(1)))
# gets the source file of the test tuple
override _test_source = $(word 2,$(subst :, ,$(1)))

# pipe commands into this function to color/style them
# argument should be an ascii escape sequence (the *_fx variables)
override _color_pipe = sed -E s/'.*'/'$(1)\0$(reset_fx)'/g

# gets the object file from a source file
override _static_object = $(BIN)/$(1).$(static_object_ext)
override _shared_object = $(BIN)/$(1).$(shared_object_ext)

# finds all C language files in directory of argument 1
override _find_c_files   = $(foreach \
		__file, \
		$(shell find '$(1)' \
				-type f \
				-name '*.[ci]' \
		), \
		$(__file:$(1)/%=%) \
)
# finds all C++ language files in directory of argument 1
override _find_cxx_files = $(foreach \
		__file, \
		$(shell find '$(1)' \
				-type f \
				'(' \
						-name '*.C'   -o \
						-name '*.cc'  -o \
						-name '*.cp'  -o \
						-name '*.ii'  -o \
						-name '*.c++' -o \
						-name '*.cpp' -o \
						-name '*.CPP' -o \
						-name '*.cxx'    \
				')' \
		), \
		$(__file:$(1)/%=%) \
)

# === constants ============================================================== #

override LINK_FLAGS := $(addprefix -L,$(LINK_DIRS)) $(addprefix -l,$(LINKS))

# all main C/C++ source files
override C_SOURCES   := $(sort $(call _find_c_files,$(SRC_MAIN)))
override CXX_SOURCES := $(sort $(call _find_cxx_files,$(SRC_MAIN)))

# checking if source files were found
ifeq "$(C_SOURCES)$(CXX_SOURCES)" ""
 $(error $(error_fx)No source files found$(reset_fx))
endif

# all test C/C++ source files
override TEST_C_SOURCES   := $(sort $(call _find_c_files,$(SRC_TEST)))
override TEST_CXX_SOURCES := $(sort $(call _find_cxx_files,$(SRC_TEST)))

# shared objects
override SHARED_C_OBJECTS   := $(sort $(foreach __source_file,$(C_SOURCES), \
	$(call _shared_object,$(__source_file)) \
))
override SHARED_CXX_OBJECTS := $(sort $(foreach __source_file,$(CXX_SOURCES), \
	$(call _shared_object,$(__source_file)) \
))
override SHARED_OBJECTS     := $(sort $(SHARED_C_OBJECTS) $(SHARED_CXX_OBJECTS))

# static objects
override STATIC_C_OBJECTS   := $(sort $(foreach __source_file,$(C_SOURCES), \
	$(call _static_object,$(__source_file)) \
))
override STATIC_CXX_OBJECTS := $(sort $(foreach __source_file,$(CXX_SOURCES), \
	$(call _static_object,$(__source_file)) \
))
override STATIC_OBJECTS     := $(sort $(STATIC_C_OBJECTS) $(STATIC_CXX_OBJECTS))

# targets
override SHARED_LIB_TARGET := $(shared_lib_prefix)$(TARGET)$(shared_lib_suffix)
override STATIC_LIB_TARGET := $(static_lib_prefix)$(TARGET)$(static_lib_suffix)
override EXE_TARGET        := $(exe_prefix)$(TARGET)$(exe_suffix)

# test tuples
# these are in format of <test executable name>:<test source file>
override C_TESTS   := $(sort $(foreach __source_file,$(TEST_C_SOURCES), \
	$(test_prefix)$(basename $(notdir $(__source_file)))$(test_suffix):$(__source_file) \
))
override CXX_TESTS := $(sort $(foreach __source_file,$(TEST_CXX_SOURCES), \
	$(test_prefix)$(basename $(notdir $(__source_file)))$(test_suffix):$(__source_file) \
))

# extracts just the targets from the test tuples
override TEST_C_TARGETS   := $(sort $(foreach __test,$(C_TESTS), \
	$(call _test_target,$(__test)) \
))
override TEST_CXX_TARGETS := $(sort $(foreach __test,$(CXX_TESTS), \
	$(call _test_target,$(__test)) \
))
override TEST_TARGETS     := $(sort $(TEST_C_TARGETS) $(TEST_CXX_TARGETS))

# === default rule =========================================================== #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 all: target
 .PHONY: all
else
 all: targets
 .PHONY: all
endif

# === universe rule ========================================================== #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 ifneq "$(SRC_TEST)" "/dev/null"
  _universe: target tests
	$(warning $(warning_fx)The `_universe` target is deprecated, use the `universe` target instead$(reset_fx))
  .PHONY: _universe
 else
  _universe: target
	$(warning $(warning_fx)The `_universe` target is deprecated, use the `universe` target instead$(reset_fx))
  .PHONY: _universe
 endif
else
 ifneq "$(SRC_TEST)" "/dev/null"
  _universe: targets tests
	$(warning $(warning_fx)The `_universe` target is deprecated, use the `universe` target instead$(reset_fx))
  .PHONY: _universe
 else
  _universe: targets
	$(warning $(warning_fx)The `_universe` target is deprecated, use the `universe` target instead$(reset_fx))
  .PHONY: _universe
 endif
endif

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 ifneq "$(SRC_TEST)" "/dev/null"
  universe: target tests
  .PHONY: universe
 else
  universe: target
  .PHONY: universe
 endif
else
 ifneq "$(SRC_TEST)" "/dev/null"
  universe: targets tests
  .PHONY: universe
 else
  universe: targets
  .PHONY: universe
 endif
endif

# === building object files ================================================== #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 objects: $(STATIC_OBJECTS)
 $(STATIC_C_OBJECTS):   $(call _static_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CC)  $(CCFLAGS) -c '$<' -o '$@'
 $(STATIC_CXX_OBJECTS): $(call _static_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CXX) $(CXXFLAGS) -c '$<' -o '$@'
 .PHONY: objects
else
 objects: objects/shared objects/static
 objects/shared: $(SHARED_OBJECTS)
 objects/static: $(STATIC_OBJECTS)
 $(SHARED_C_OBJECTS):   $(call _shared_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CC)  $(CCFLAGS) -c '$<' -o '$@' -fPIC
 $(SHARED_CXX_OBJECTS): $(call _shared_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CXX) $(CXXFLAGS) -c '$<' -o '$@' -fPIC
 $(STATIC_C_OBJECTS):   $(call _static_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CC)  $(CCFLAGS) -c '$<' -o '$@'
 $(STATIC_CXX_OBJECTS): $(call _static_object,%): $(SRC_MAIN)/%
	@mkdir -p '$(dir $@)'
	$(info $(object_build_fx)Building file '$@'...$(reset_fx))
	@$(CXX) $(CXXFLAGS) -c '$<' -o '$@'
 .PHONY: objects objects/shared objects/static
endif

# === building targets ======================================================= #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 target: $(EXE_TARGET)
 $(EXE_TARGET): $(STATIC_OBJECTS)
	$(info $(target_build_fx)Building target '$@'...$(reset_fx))
  ifeq "$(CXX_SOURCES)" ""
	@$(CC)  $(CCFLAGS)  $^ -o '$@' $(LINK_FLAGS)
  else
	@$(CXX) $(CXXFLAGS) $^ -o '$@' $(LINK_FLAGS)
  endif
  .PHONY: target
else
 targets: $(SHARED_LIB_TARGET) $(STATIC_LIB_TARGET)
 $(SHARED_LIB_TARGET): $(SHARED_OBJECTS)
	$(info $(target_build_fx)Building target '$@'...$(reset_fx))
  ifeq "$(CXX_SOURCES)" ""
	@$(CC)  $(CCFLAGS)  $^ -o '$@' -shared
  else
	@$(CXX) $(CXXFLAGS) $^ -o '$@' -shared
  endif
 $(STATIC_LIB_TARGET): $(STATIC_OBJECTS)
	$(info $(target_build_fx)Building target '$@'...$(reset_fx))
	@$(AR) rs '$@' $^ 2>/dev/null
 .PHONY: targets
endif

# === testing ================================================================ #

ifneq "$(SRC_TEST)" "/dev/null"
 # finds the test source file to a given test target from all test tuples
 override _find_test_source = $(foreach __test,$(C_TESTS) $(CXX_TESTS), \
	$(if \
		$(call _eq,$(1),$(call _test_target,$(__test))), \
		$(call _test_source,$(__test)) \
	) \
 )

 ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
  # static objects without the main file
  ifdef MAIN
   override STATIC_C_OBJECTS_FOR_EXE_TEST   := $(sort $(filter-out $(call _static_object,$(MAIN)),$(STATIC_C_OBJECTS)))
   override STATIC_CXX_OBJECTS_FOR_EXE_TEST := $(sort $(filter-out $(call _static_object,$(MAIN)),$(STATIC_CXX_OBJECTS)))
   override STATIC_OBJECTS_FOR_EXE_TEST     := $(sort $(STATIC_C_OBJECTS_FOR_EXE_TEST) $(STATIC_CXX_OBJECTS_FOR_EXE_TEST))
  endif

  tests: $(TEST_TARGETS)
  .SECONDEXPANSION:
  $(TEST_C_TARGETS): %:   $(STATIC_C_OBJECTS_FOR_EXE_TEST) \
                          $(SRC_TEST)/$$(strip $$(call _find_test_source,%))
	$(info $(test_build_fx)Building test '$@'...$(reset_fx))
	@$(CC)  $(CCFLAGS)  $^ -o '$@'
  .SECONDEXPANSION:
  $(TEST_CXX_TARGETS): %: $(STATIC_OBJECTS_FOR_EXE_TEST) \
                          $(SRC_TEST)/$$(strip $$(call _find_test_source,%))
	$(info $(test_build_fx)Building test '$@'...$(reset_fx))
	@$(CXX) $(CXXFLAGS) $^ -o '$@'
  test: $(TEST_TARGETS)
	@$(TEST) $(addprefix ./,$^)
  .PHONY: tests test
 else
  tests: $(TEST_TARGETS)
  .SECONDEXPANSION:
  $(TEST_C_TARGETS): %:   $(STATIC_C_OBJECTS) \
                          $(SRC_TEST)/$$(strip $$(call _find_test_source,%))
	$(info $(test_build_fx)Building test '$@'...$(reset_fx))
	@$(CC)  $(CCFLAGS)  $^ -o '$@'
  .SECONDEXPANSION:
  $(TEST_CXX_TARGETS): %: $(STATIC_OBJECTS) \
                          $(SRC_TEST)/$$(strip $$(call _find_test_source,%))
	$(info $(test_build_fx)Building test '$@'...$(reset_fx))
	@$(CXX) $(CXXFLAGS) $^ -o '$@'
  test: $(TEST_TARGETS)
	@$(TEST) $(addprefix ./,$^)
  .PHONY: tests test
 endif
endif

# === installing ============================================================= #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 install: install/target
 install/target: install/$(EXE_TARGET)
 install/$(EXE_TARGET): install/%: %
	$(info $(install_fx)Installing target '$(@:install/%=%)' to '$(DESTDIR)$(bindir)'...$(reset_fx))
	@mkdir -p '$(DESTDIR)$(bindir)'
	@$(INSTALL) -m755 '$(@:install/%=%)' '$(DESTDIR)$(bindir)'
 .PHONY: install install/target install/$(EXE_TARGET)
else
 install: install/targets install/headers
 install/targets: install/$(SHARED_LIB_TARGET) install/$(STATIC_LIB_TARGET)
 install/$(SHARED_LIB_TARGET) install/$(STATIC_LIB_TARGET): install/%: %
	$(info $(install_fx)Installing target '$(@:install/%=%)' to '$(DESTDIR)$(libdir)'...$(reset_fx))
	@mkdir -p '$(DESTDIR)$(libdir)'
	@$(INSTALL) -m644 '$(@:install/%=%)' '$(DESTDIR)$(libdir)'
 install/headers:
	$(info $(install_fx)Installing headers to '$(DESTDIR)$(includedir)'...$(reset_fx))
	@mkdir -p '$(DESTDIR)$(includedir)'
	@cp -r '$(INC)' '$(DESTDIR)$(includedir)'
 .PHONY: install install/targets \
         install/$(SHARED_LIB_TARGET) install/$(STATIC_LIB_TARGET) \
         install/headers
endif

# === uninstalling =========================================================== #

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 uninstall: uninstall/target
 uninstall/target: uninstall/$(EXE_TARGET)
 uninstall/$(EXE_TARGET):
	@rm -fv '$(DESTDIR)$(bindir)/$(@:uninstall/%=%)' | \
		$(call _color_pipe,$(uninstall_fx))
 .PHONY: uninstall uninstall/target uninstall/$(EXE_TARGET)
else
 uninstall: uninstall/targets uninstall/headers
 uninstall/targets: uninstall/$(SHARED_LIB_TARGET) uninstall/$(STATIC_LIB_TARGET)
 uninstall/$(SHARED_LIB_TARGET) uninstall/$(STATIC_LIB_TARGET): %:
	@rm -fv '$(DESTDIR)$(libdir)/$(@:uninstall/%=%)' | \
		$(call _color_pipe,$(uninstall_fx))
 uninstall/headers:
	@rm -rfv '$(DESTDIR)$(includedir)/$(notdir $(INC))' | \
		$(call _color_pipe,$(uninstall_fx))
 .PHONY: uninstall uninstall/targets \
         uninstall/$(SHARED_LIB_TARGET) uninstall/$(STATIC_LIB_TARGET) \
         uninstall/headers
endif

# === cleaning =============================================================== #

override _clean_empty_dir = if [ -d '$(1)' ]; then \
	find '$(1)' -depth -type d -exec rm -dfv '{}' ';' 2>/dev/null \
		| $(call _color_pipe,$(clean_fx)) ; \
fi

override CLEANING_STATIC_OBJECTS := $(addprefix clean/,$(STATIC_OBJECTS))
override CLEANING_SHARED_OBJECTS := $(addprefix clean/,$(SHARED_OBJECTS))
override CLEANING_OBJECTS        := $(CLEANING_SHARED_OBJECTS) \
                                    $(CLEANING_STATIC_OBJECTS)

override CLEANING_TEST_TARGETS := $(addprefix clean/,$(TEST_TARGETS))

ifeq "$(SOFTWARE)" "$(EXE_SOFTWARE)"
 ifneq "$(SRC_TEST)" "/dev/null"
  clean: clean/objects clean/target clean/tests
  .PHONY: clean
 else
  clean: clean/objects clean/target
  .PHONY: clean
 endif

 clean/objects: $(CLEANING_STATIC_OBJECTS)
 clean/$(BIN):
	@rm -rfv '$(BIN)' | $(call _color_pipe,$(clean_fx))
 $(CLEANING_STATIC_OBJECTS): %:
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
	@$(call _clean_empty_dir,$(BIN))
 .PHONY: clean/objects $(CLEANING_STATIC_OBJECTS)

 clean/target: clean/$(EXE_TARGET)
 clean/$(EXE_TARGET):
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
 .PHONY: clean/target clean/$(EXE_TARGET)

 ifneq "$(SRC_TEST)" "/dev/null"
  clean/tests: $(CLEANING_TEST_TARGETS)
  $(CLEANING_TEST_TARGETS): %:
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
  .PHONY: clean/tests $(CLEANING_TEST_TARGETS)
 endif
else
 ifneq "$(SRC_TEST)" "/dev/null"
  clean: clean/objects clean/targets clean/tests
  .PHONY: clean
 else
  clean: clean/objects clean/targets
  .PHONY: clean
 endif

 clean/objects: clean/objects/shared clean/objects/static
 clean/objects/shared: $(CLEANING_SHARED_OBJECTS)
 clean/objects/static: $(CLEANING_STATIC_OBJECTS)
 clean/$(BIN):
	@rm -rfv '$(BIN)' | $(call _color_pipe,$(clean_fx))
 $(CLEANING_OBJECTS): %:
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
	@$(call _clean_empty_dir,$(BIN))
 .PHONY: clean/objects \
         clean/objects/shared clean/objects/static \
         $(CLEANING_OBJECTS)

 clean/targets: clean/$(SHARED_LIB_TARGET) clean/$(STATIC_LIB_TARGET)
 clean/$(SHARED_LIB_TARGET) clean/$(STATIC_LIB_TARGET): %:
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
 .PHONY: clean/targets clean/$(SHARED_LIB_TARGET) clean/$(STATIC_LIB_TARGET)

 ifneq "$(SRC_TEST)" "/dev/null"
  clean/tests: $(CLEANING_TEST_TARGETS)
  $(CLEANING_TEST_TARGETS): %:
	@rm -fv '$(@:clean/%=%)' | $(call _color_pipe,$(clean_fx))
  .PHONY: clean/tests $(CLEANING_TEST_TARGETS)
 endif
endif

# === version ================================================================ #

_version:
	@echo 2.3.0
.PHONY: _version

# = other.mk ================================================================= #

-include other.mk
