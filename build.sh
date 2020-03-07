gcc -c ../tuntap_linux.c -DCMAKE_BUILD -g -I../
gcc -c ../n2n.c -DGIT_RELEASE=1.0 -DPACKAGE_OSNAME=linux -DCMAKE_BUILD -g -I../
gcc -c node.c -DCMAKE_BUILD -g -I../
gcc -o node node.o tuntap_linux.o n2n.o
