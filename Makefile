CBINS := $(patsubst %.c,%,$(wildcard *.c))

all: $(CBINS)

%: %.c
	gcc $< -o $@
clean:
	rm -f $(CBINS)
	rm -rf ec-wc10.zip

package: all
	zip ec-wc10.zip README $(CBINS) fuzzer.py
