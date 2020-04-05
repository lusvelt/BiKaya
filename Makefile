
all: uarm umps

test: test-uarm test-umps

test-uarm:
	$(MAKE) -f makefiles/uarm.mk TEST=1

test-umps:
	$(MAKE) -f makefiles/umps.mk TEST=1

umps:
	$(MAKE) -f makefiles/umps.mk
	
umps2:
	$(MAKE) -f makefiles/umps.mk

uarm:
	$(MAKE) -f makefiles/uarm.mk

doc:
	doxygen Doxyfile

clean:
	$(MAKE) -f makefiles/umps.mk clean
	$(MAKE) -f makefiles/uarm.mk clean
	rm -rf docs

.PHONY: umps umps2 uarm clean test all doc