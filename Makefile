nodos := 16
run := mpirun
mcc  := mpicc
cc := gcc


all: 
	$(mcc) pract2.c -o pract2 -lX11

Red:
	$(run) -np 1 ./pract2 R

Green:
	$(run) -np 1 ./pract2 G

Blue:
	$(run) -np 1 ./pract2 B

BlackWhite:
	$(run) -np 1 ./pract2 W

Sepia:
	$(run) -np 1 ./pract2 S

Negativo:
	$(run) -np 1 ./pract2 N
run:
	$(run) -np 1 ./pract2 A