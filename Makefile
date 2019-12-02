
all:
	$(MAKE) -f makefiles/base.mk ARCH=umps
	$(MAKE) -f makefiles/base.mk ARCH=uarm

umps:
	$(MAKE) -f makefiles/base.mk ARCH=umps
	
umps2:
	$(MAKE) -f makefiles/base.mk ARCH=umps

uarm:
	$(MAKE) -f makefiles/base.mk ARCH=uarm

clean:
	$(MAKE) -f makefiles/base.mk clean

.PHONY: umps umps2 uarm clean all