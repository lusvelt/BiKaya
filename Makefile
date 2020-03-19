
all:
	$(MAKE) -f makefiles/umps.mk
	$(MAKE) -f makefiles/uarm.mk

umps:
	$(MAKE) -f makefiles/umps.mk
	
umps2:
	$(MAKE) -f makefiles/umps.mk

uarm:
	$(MAKE) -f makefiles/uarm.mk

clean:
	$(MAKE) -f makefiles/umps.mk clean
	$(MAKE) -f makefiles/uarm.mk clean

print:
	$(MAKE) -f makefiles/uarm.mk print

.PHONY: umps umps2 uarm clean all print