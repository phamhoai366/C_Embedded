object := foo.o bar.o all.o

all: $(object)
	$(CC) $^ -o all

# foo.o: foo.c
# 	$(CC) -c foo.c -o foo.o

# bar.o: bar.c
# 	$(CC) -c bar.c -o bar.o

# all.o: all.c
# 	$(CC) -c all.c -o all.o

$(object): %.o: %.c 
	$(CC) -c $^ -o $@
 
all.c: 
	echo "int main() { return 0; }" > all.c

%.c:
	touch $@

clean:
	rm -f *.c *.o all