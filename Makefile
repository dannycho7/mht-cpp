main:
	$(MAKE) -C src/
mhtc.so:
	$(MAKE) -C src/ mhtc.so
test:
	$(MAKE) -C src test
clean:
	$(MAKE) -C src/ clean
