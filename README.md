######################################################################
######                                                          ######
######                  David Carneros Prado.                   ######
######             Diseño de infraestructuras de red            ######
######            Renderizado de graficos distribuidos          ######
######                                                          ######
######################################################################

###COMO EJECUTAR LA PRACTICA: 



#Para compilar, hay dos opciones
    1. Automática con makefile
        § make all
    2. Manual:
        § mpicc pract2.c -o pract2 -lX11

#Para ejecutar: para elegir lo diferentes filtros hay que cambiar el argumento al ejecutar.
        § mpirun -np 1 ./pract2 <filtro>

    Red:
        $ make Red
        $ mpirun -np 1 ./pract2 R
    Blue:
        $ make Blue
        $ mpirun -np 1 ./pract2 B
    Green:
        $ make Green
        $ mpirun -np 1 ./pract2 G
    BlackWhite:
        $ make BlackWhite
        $ mpirun -np 1 ./pract2 W
    Sepia:
        $ make Sepia
        $ mpirun -np 1 ./pract2 S
    Negativo:
        $ make Negativo
        $ mpirun -np 1 ./pract2 N

