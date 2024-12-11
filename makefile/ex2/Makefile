file := blah1 blah2
var := abcdef

all: $(file)
	echo $(var)
	echo ${var}

blah1: blah1.c
	gcc blah1.c -o blah1

blah2: blah2.c
	gcc blah2.c -o blah2

# blah1 blah2:
# 	gcc $@.c -o $@

clean:
	rm -f $(file)