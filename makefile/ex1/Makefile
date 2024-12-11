
# blah: blah.c
# 	gcc blah.c -o blah

blah: blah.o
	gcc blah.o -o blah

blah.o: blah.c
	gcc -c blah.c -o blah.o

clean:
	rm -f blah blah.o
