CFLAGS=-g -Wall -Werror

all: tests lib_tar.o

lib_tar.o: lib_tar.c lib_tar.h

tests: tests.c lib_tar.o

couille: tests.c lib_tar.c
	gcc -o $@ -Wall -Werror $^

clean:
	rm -f lib_tar.o tests soumission.tar

empty: empty.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c empty.txt > empty.tar

tf: twentyfour.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c twentyfour.txt > tf.tar

both: empty.txt twentyfour.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c twentyfour.txt empty.txt > both.tar


submit: all
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c Makefile > soumission.tar