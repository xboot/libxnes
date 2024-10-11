#
# Top Makefile
#

.PHONY: all clean

all:
	$(MAKE) -C examples/linux all
	$(MAKE) -C examples/windows all
	$(MAKE) -C examples/debugger all

clean:
	$(MAKE) -C examples/linux clean
	$(MAKE) -C examples/windows clean
	$(MAKE) -C examples/debugger clean
