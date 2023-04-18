SRCDIR = src
ROM_PATH = $(abspath $(if $(filter /%,$(ROM)),$(ROM),$(dir $(lastword $(MAKEFILE_LIST)))$(ROM)))

.PHONY: all
all:
	make all -C $(SRCDIR)

.PHONY: clean
clean:
	make clean -C $(SRCDIR)

.PHONY: run
run:
	make run ROM=$(ROM_PATH) -C $(SRCDIR)

.PHONY: debug
debug:
	make debug ROM=$(ROM_PATH) -C $(SRCDIR)
