CBINS := $(patsubst %.c,%,$(wildcard *.c))

all: $(CBINS)

%: %.c
	gcc $< -o $@
clean:
	rm -f $(CBINS)
	rm -rf ec-wc10.zip

package: all
	rm -rf ec-wc10
	mkdir ec-wc10
	cp $(CBINS) fuzzer.py ec-wc10/
	zip -r ec-wc10.zip ec-wc10
