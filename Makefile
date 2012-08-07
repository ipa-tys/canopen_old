SUBDIRS = driver demo

.PHONY: all
all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean: 
	rm -f *~
	for dir in $(SUBDIRS); do \
	  $(MAKE) --directory=$$dir clean; \
	done

uninstall:
	$(MAKE) uninstall --directory=driver

install:
	$(MAKE) install --directory=driver

# $(MAKE) --directory=$$dir clean; \