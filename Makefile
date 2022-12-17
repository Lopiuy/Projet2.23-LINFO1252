CFLAGS=-g -Wall -Werror

all: tests lib_tar.o

lib_tar.o: lib_tar.c lib_tar.h

tests: tests.c lib_tar.o

clean:
	rm -f *.o tests *.tar

empty: empty.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c empty.txt > empty.tar

tf: twentyfour.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c twentyfour.txt > tf.tar

sh: sixhundred.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c sixhundred.txt > sh.tar

both: empty.txt twentyfour.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c twentyfour.txt empty.txt > both.tar

three: empty.txt twentyfour.txt sixhundred.txt
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c twentyfour.txt empty.txt sixhundred.txt > three.tar

folder: Folder
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c Folder > folder.tar

df: DoubleFold
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c DoubleFold > df.tar

symfile: Folder
	ln -s first.txt Folder/symfile.txt

submit: all
	tar --posix --pax-option delete=".*" --pax-option delete="*time*" --no-xattrs --no-acl --no-selinux -c *.h *.c Makefile > soumission.tar