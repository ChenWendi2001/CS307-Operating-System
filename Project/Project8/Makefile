all:vmm.o
	gcc -o vmm vmm.o


vmm.o:vmm.c
	gcc -c vmm.c

clean:
	rm *.o vmm a.out

check:
	diff a.out correct.txt