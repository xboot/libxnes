#
# debugger top directory
#
DEBUGGER_DIR = $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

#
# debugger
#
INCDIRS		+=	$(DEBUGGER_DIR)
SRCDIRS		+=	$(DEBUGGER_DIR)
