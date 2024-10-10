#
# xnes top directory
#
XNES_DIR = $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

#
# xnes
#
INCDIRS		+=	$(XNES_DIR) \
				$(XNES_DIR)/mapper
SRCDIRS		+=	$(XNES_DIR) \
				$(XNES_DIR)/mapper