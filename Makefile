SUBDIRS = driver demo

.PHONY: all
all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean: 
	rm *~
	for dir in $(SUBDIRS); do \
		$(MAKE) --directory=$$dir clean; \
	done

install:
	$(MAKE) install --directory=driver
