#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

TOOLS_ROOT ?= $(SOURCE_ROOT)mico-os/MiCoder

OPENOCD_PATH      := $(TOOLS_ROOT)/OpenOCD/
OPENOCD_CFG_PATH  := $(MAKEFILES_PATH)/OpenOCD/
PATH :=

JTAG         ?= jlink_swd


ifeq ($(HOST_OS),Win32)
################
# Windows settings
################
COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/Win32/
export SHELL       = cmd.exe
EXECUTABLE_SUFFIX  := .exe
OPENOCD_FULL_NAME := $(OPENOCD_PATH)Win32/openocd_mico.exe

# Python
ifneq ($(wildcard C:\Python34\python.exe),)
PYTHON_FULL_NAME := C:\Python34\python.exe
endif
ifneq ($(wildcard C:\Python27\python.exe),)
PYTHON_FULL_NAME := C:\Python27\python.exe
endif

ifneq ($(IAR),1)
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END :=\"
else
SLASH_QUOTE_START :="\"
SLASH_QUOTE_END :=\""
endif

ESC_QUOTE:="
ESC_SPACE:=$(SPACE)
CAT               := type
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)" -n
ECHO              := echo
QUOTES_FOR_ECHO   :=
CMD_TRUNC         := "$(COMMON_TOOLS_PATH)trunc$(EXECUTABLE_SUFFIX)"
PERL              := "$(COMMON_TOOLS_PATH)perl$(EXECUTABLE_SUFFIX)"
PYTHON            := "$(COMMON_TOOLS_PATH)Python27/python$(EXECUTABLE_SUFFIX)"
LINT_EXE          := "$(TOOLS_ROOT)/splint/splint/bin/splint$(EXECUTABLE_SUFFIX)"
PERL_ESC_DOLLAR   :=$$
CLEAN_COMMAND     := if exist "$(SOURCE_ROOT)build" $(call CONV_SLASHES,$(COMMON_TOOLS_PATH))rmdir /s /q "$(SOURCE_ROOT)build"
MKDIR              = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
RMDIR              = if exist $(subst /,\,$1) rmdir /s /q $(subst /,\,$1)
CPDIR              = xcopy /s /q /i $(subst /,\,$1) $(subst /,\,$2)
CONV_SLASHES       = $(subst /,\,$1)
DIR                = $(dir $(subst /,\,$1))
TOUCH              = $(ECHO) >
CYGWIN :=
DEV_NULL          := nul
TRUE_CMD          := call
FALSE_CMD         := fail > nul 2>&1

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) $(1)>>$(2)

endef

WRITE_FILE_CREATE =$(file >$(1),$(2))
WRITE_FILE_APPEND =$(file >>$(1),$(2))

else  # Win32
ifeq ($(HOST_OS),Linux32)
################
# Linux 32-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/Linux32/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux32/openocd_mico"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PYTHON            := "/usr/bin/python"
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(SOURCE_ROOT)build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);

else # Linux32
ifeq ($(HOST_OS),Linux64)
################
# Linux 64-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/Linux64/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux64/openocd_mico"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PYTHON            := "/usr/bin/python"
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(SOURCE_ROOT)build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);
# # Check the file writing is working correctly
# # should result in: $'""\"\"\\"\\\"\\
# TEST_DATA := $$'""\"\"\\"\\\"\\
# $(info TEST_DATA=$(TEST_DATA))
# $(info $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA)))
# $(info done=$(shell $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA))))

else # Linux64
ifeq ($(HOST_OS),OSX)
################
# OSX settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/OSX/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)OSX/openocd_mico"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
ECHO              := "$(COMMON_TOOLS_PATH)echo"
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := "/usr/bin/perl"
PYTHON            := "/usr/bin/python"
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(SOURCE_ROOT)build
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(2)' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(2)' >> $(1);

else # OSX

$(error incorrect 'make' used ($(MAKE)) - please use:  (Windows) .\make.exe <target_string>    (OS X, Linux) ./make <target_string>)

endif # OSX
endif # Linux64
endif # Linux32
endif # Win32


# Set shortcuts to the compiler and other tools
RM      := "$(COMMON_TOOLS_PATH)rm$(EXECUTABLE_SUFFIX)" -f
CP      := "$(COMMON_TOOLS_PATH)cp$(EXECUTABLE_SUFFIX)" -f
MV      := "$(COMMON_TOOLS_PATH)mv$(EXECUTABLE_SUFFIX)" -f
MAKE    := "$(COMMON_TOOLS_PATH)make$(EXECUTABLE_SUFFIX)"
BIN2C   := "$(COMMON_TOOLS_PATH)bin2c$(EXECUTABLE_SUFFIX)"


SHOULD_I_WAIT_FOR_DOWNLOAD := $(filter download download_apps ota2_download ota2_factory_download, $(MAKECMDGOALS))
BUILD_STRING ?= $(strip $(filter-out $(MAKEFILE_TARGETS) debug download download_apps download_only run terminal total, $(MAKECMDGOALS)))
BUILD_STRING_TO_DIR = $(subst .,/,$(1))
DIR_TO_BUILD_STRING = $(subst /,.,$(1))
CLEANED_BUILD_STRING := $(BUILD_STRING)
BUILD_DIR    :=  $(SOURCE_ROOT)build

OUTPUT_DIR   := $(BUILD_DIR)/$(CLEANED_BUILD_STRING)

# Newline Macro
define newline


endef


# Use VERBOSE=1 to get full output
ifneq ($(VERBOSE),1)
QUIET:=@
SILENT:=-s
QUIET_SHELL =$(shell $(1))
QUIET_SHELL_IN_MACRO =$$(shell $(1))
else
QUIET:=
SILENT:=
QUIET_SHELL =$(shell $(1)$(info $(1)))
QUIET_SHELL_IN_MACRO =$$(shell $(1)$$(info $(1)))
endif



COMMA :=,

SPACE :=
SPACE +=

# $(1) is a string to be escaped
ESCAPE_BACKSLASHES =$(subst \,\\,$(1))


# MXCHIP internal only - Add gerrit hook for changeid
ifneq ($(wildcard $(TOOLS_ROOT)/style/gerrit_commit-msg),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

MXCHIP_INTERNAL :=NO

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/commit-msg

$(SOURCE_ROOT).git/hooks/commit-msg:  $(TOOLS_ROOT)/style/gerrit_commit-msg
	$(QUIET)$(ECHO) Adding gerrit git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/gerrit_commit-msg $(SOURCE_ROOT).git/hooks/commit-msg


endif
endif

# MXCHIP internal only - Add git commit hook for style checker
ifneq ($(wildcard $(TOOLS_ROOT)/style/git_style_checker.pl),)
ifneq ($(wildcard $(TOOLS_ROOT)/style/pre-commit),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/pre-commit

$(SOURCE_ROOT).git/hooks/pre-commit: $(TOOLS_ROOT)/style/pre-commit
	$(QUIET)$(ECHO) Adding style checker git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/pre-commit $(SOURCE_ROOT).git/hooks/pre-commit

endif
endif
endif

# MXCHIP internal only - Add git push hook for commit checker
ifneq ($(wildcard $(TOOLS_ROOT)/style/git_commit_checker.pl),)
ifneq ($(wildcard $(TOOLS_ROOT)/style/pre-push),)
ifneq ($(wildcard $(SOURCE_ROOT).git),)

TOOLCHAIN_HOOK_TARGETS += $(SOURCE_ROOT).git/hooks/pre-push

$(SOURCE_ROOT).git/hooks/pre-push: $(TOOLS_ROOT)/style/pre-push
	$(QUIET)$(ECHO) Adding commit checker git hook
	$(QUIET)$(CP) $(TOOLS_ROOT)/style/pre-push $(SOURCE_ROOT).git/hooks/pre-push

endif
endif
endif

# MXCHIP internal only - Copy Eclipse .project file if it doesn't exist
ifeq ($(wildcard $(SOURCE_ROOT)/.project),)

$(info Create Eclipse .project file to source tree root)
ECLIPSE_PROJECT_FILE := $(SOURCE_ROOT)/.project
$(shell $(call WRITE_FILE_CREATE, $(ECLIPSE_PROJECT_FILE) ,<?xml version="1.0" encoding="UTF-8"?>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,<projectDescription>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<name>$(notdir $(abspath $(SOURCE_ROOT)))</name>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<comment></comment>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<projects>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</projects>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<buildSpec>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<buildCommand>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<triggers>clean$(comma)full$(comma)incremental$(comma)</triggers>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<arguments>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			</arguments>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		</buildCommand>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<buildCommand>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<triggers>full$(comma)incremental$(comma)</triggers>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			<arguments>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,			</arguments>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		</buildCommand>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</buildSpec>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	<natures>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.core.cnature</nature>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.core.ccnature</nature>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,		<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,	</natures>))
$(shell $(call WRITE_FILE_APPEND, $(ECLIPSE_PROJECT_FILE) ,</projectDescription>))

endif

# MXCHIP internal only - Copy Eclipse .cproject file if it doesn't exist
ifeq ($(wildcard $(SOURCE_ROOT)/.cproject),)
ifneq ($(wildcard $(TOOLS_ROOT)/eclipse_project/$(HOST_OS)/.cproject),)

$(info Copying Eclipse .cproject file to source tree root)

$(shell $(CP) $(TOOLS_ROOT)/eclipse_project/$(HOST_OS)/.cproject $(SOURCE_ROOT) )

endif
endif

#########
# Expand wildcard platform names.
# Consider all platforms in platforms/* and platform/*/*
define EXPAND_WILDCARD_PLATFORMS
$(eval POSSIBLE_PLATFORMS :=) \
$(eval EXPANDED_PLATFORMS :=) \
$(foreach ENTRY, $(1), \
$(eval WILDCARD_PLATFORM := $(call BUILD_STRING_TO_DIR, $(ENTRY))) \
$(eval POSSIBLE_PLATFORMS += $(subst board/,,$(wildcard board/$(WILDCARD_PLATFORM)))) \
$(eval POSSIBLE_PLATFORMS += $(subst board/,,$(wildcard board/$(WILDCARD_PLATFORM)/*)))) \
$(eval $(foreach PLATFORM, $(POSSIBLE_PLATFORMS), \
$(eval $(if $(wildcard board/$(PLATFORM)/$(notdir $(PLATFORM)).mk), EXPANDED_PLATFORMS += $(call DIR_TO_BUILD_STRING, $(PLATFORM))))))\
${EXPANDED_PLATFORMS}
endef

##########
# Recurse directories to find valid MiCO components.
# $(1) = starting directory
# $(2) = name of variable to which to add components that are found
define RECURSE_DIR_COMPONENT_SEARCH
$(foreach file, $(wildcard $(1)/*), $(if $(wildcard $(file)/*), $(if $(wildcard $(file)/$(notdir $(file)).mk), $(eval $(2) += $(file)),) $(call RECURSE_DIR_COMPONENT_SEARCH,$(file),$(2)),))
endef


##########
# Find all valid components.
# $(1) = name of variable to which to add components that are found
# $(2) = list of component directories
define FIND_VALID_COMPONENTS
$(call RECURSE_DIR_COMPONENT_SEARCH, $(patsubst %/,%,$(SOURCE_ROOT)),$(1)) \
$(eval $(1) := $(subst ./,,$($(strip $(1))))) \
$(foreach compdir, $(2),$(eval $(1) := $(patsubst $(compdir)/%,%,$($(strip $(1)))))) \
$(eval $(1) := $(subst /,.,$($(strip $(1)))))
endef

##########
# Strip duplicate items in list without sorting
# $(1) = List of items to de-duplicate
unique = $(eval seen :=)$(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))${seen}


CURRENT_MAKEFILE = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
