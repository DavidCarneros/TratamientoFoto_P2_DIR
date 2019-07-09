/* Pract2  RAP 09/10    Javier Ayllon*/

#include <openmpi/mpi.h>
//#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 
#include <assert.h>   
#include <unistd.h>   
#define NIL (0)      

#define NUM 4
#define FOTO "foto.dat"
#define FILA_IMAGEN 400
#define COLUMNA_IMAGEN 400
#define INICIO 0
#define FINAL 1

/*Variables Globales */

XColor colorX;
Colormap mapacolor;
char cadenaColor[]="#000000";
Display *dpy;
Window w;
GC gc;

/*Funciones auxiliares */

void initX() {

      dpy = XOpenDisplay(NIL);
      assert(dpy);

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                                     400, 400, 0, blackColor, blackColor);
      XSelectInput(dpy, w, StructureNotifyMask);
      XMapWindow(dpy, w);
      gc = XCreateGC(dpy, w, 0, NIL);
      XSetForeground(dpy, gc, whiteColor);
      for(;;) {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify)
                  break;
      }


      mapacolor = DefaultColormap(dpy, 0);

}

void dibujaPunto(int x,int y, int r, int g, int b) {

        sprintf(cadenaColor,"#%.2X%.2X%.2X",r,g,b);
        XParseColor(dpy, mapacolor, cadenaColor, &colorX);
        XAllocColor(dpy, mapacolor, &colorX);
        XSetForeground(dpy, gc, colorX.pixel);
        XDrawPoint(dpy, w, gc,x,y);
        XFlush(dpy);

}




/* -------------- Declaracion de funciones --------------- */

void tratarImagen(int rank,int *reparto, long bytesLeer, MPI_Comm commPadre,MPI_File imagen,char *flag);
void aplicarFiltro(int x,int y,unsigned char *pixel,MPI_Comm commPadre, char *flag);
void esperarPuntos(MPI_Comm commPadre);
int *RepartirTarea(int rank,int size);

/* Programa principal */

int main (int argc, char *argv[]) {

  int rank,size;
  MPI_Comm commPadre,intercomm;
  int tag;
  int errcodes[NUM];
  MPI_Status status;
  int buf[5];
  int i;


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_get_parent( &commPadre );
  if ( (commPadre==MPI_COMM_NULL)
        && (rank==0) )  {

	initX();

	/* Codigo del maestro */
      MPI_Comm_spawn("pract2",argv,NUM,MPI_INFO_NULL,0,MPI_COMM_WORLD,&intercomm,errcodes);
      printf("[Proceso PRINCIPAL] He creado %d procesos \n",NUM);
      esperarPuntos(intercomm);
      printf("[Proceso PRINCIPAL] Pulse cualquier tecla para finalizar la ejecucion...\n ");
      getchar();
	/*En algun momento dibujamos puntos en la ventana algo como
	dibujaPunto(x,y,r,g,b);  */
        }
  else {      
     /* Reparto */
     int filaReparto = FILA_IMAGEN/NUM;
     int *reparto;
     reparto = RepartirTarea(rank,size);

     long bytesLeer = filaReparto*FILA_IMAGEN*3*sizeof(unsigned char);
      
      MPI_File imagen;
      MPI_File_open(MPI_COMM_WORLD, FOTO, MPI_MODE_RDONLY, MPI_INFO_NULL, &imagen);
      MPI_File_set_view(imagen, rank*bytesLeer, 
            MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);

      printf("[Proceso %d] y tengo inicio %d y final %d\n",rank,reparto[INICIO],reparto[FINAL]);
      tratarImagen(rank,reparto,bytesLeer,commPadre,imagen,argv[2]);

  }

  MPI_Finalize();

}
/**
 * 
 * Funcion encargada de leer los pixeles del fichero foto.dat y llamar a la funcion aplicarfiltro por cada pixel
 * de la imagen
 * 
*/
void tratarImagen(int rank,int *reparto, long bytesLeer, MPI_Comm commPadre,MPI_File imagen,char *flag)
{

      int i,j;
      unsigned char pixel[3];
      int mensaje[5];
      MPI_Status status;

      /* Recorremos cada fila que nos ha tocado */
      for(i=reparto[INICIO];i<reparto[FINAL];i++)
      {
            /* Recorremos cada columna */
            for(j=0;j<COLUMNA_IMAGEN;j++)
            {
                  MPI_File_read(imagen, pixel, 3, MPI_UNSIGNED_CHAR, &status);
                  aplicarFiltro(j,i,pixel,commPadre,flag);
                  
            } 
      }
      MPI_File_close(&imagen);

}
/**
 * 
 * Funcion encargada de aplicar el filtro al pixel leido y mandarlo al proceso principal para que lo 
 * dibuje en pantalla.
 * 
*/
void aplicarFiltro(int x,int y,unsigned char *pixel,MPI_Comm commPadre, char *flag)
{
      int mensaje[5];
      MPI_Status status;
      mensaje[0]=x;
      mensaje[1]=y;
      int i=0;

      switch (*flag)
      {
            case 'R':                 
                  mensaje[2] = (int)pixel[0]; 
                  mensaje[3] = 0;
                  mensaje[4] = 0;
                  break;
            case 'G':
                  mensaje[2] = 0;
                  mensaje[3] = (int)pixel[1];
                  mensaje[4] = 0;
                  break;
            case 'B':
                  mensaje[2] = 0;
                  mensaje[3] = 0;
                  mensaje[4] = (int)pixel[2];
                  break;
            case 'W':
                  mensaje[2] = (int)(pixel[0]*0.2986)+(int)(pixel[1]*0.5870)+(int)(pixel[2]*0.1140);
                  mensaje[3] = (int)(pixel[0]*0.2986)+(int)(pixel[1]*0.5870)+(int)(pixel[2]*0.1140);
                  mensaje[4] = (int)(pixel[0]*0.2986)+(int)(pixel[1]*0.5870)+(int)(pixel[2]*0.1140);
                  break;
            case 'S':
                  mensaje[2] = (int)(pixel[0]*0.393)+(int)(pixel[1]*0.769)+(int)(pixel[2]*0.189);
                  mensaje[3] = (int)(pixel[0]*0.349)+(int)(pixel[1]*0.686)+(int)(pixel[2]*0.168);
                  mensaje[4] = (int)(pixel[0]*0.272)+(int)(pixel[1]*0.534)+(int)(pixel[2]*0.131);
                  break;
            case 'N':
                  mensaje[2] = 255-(int)pixel[0];
                  mensaje[3] = 255-(int)pixel[1];
                  mensaje[4] = 255-(int)pixel[2];
                  break;
            default:
                  mensaje[2] = (int)pixel[0];
                  mensaje[3] = (int)pixel[1];
                  mensaje[4] = (int)pixel[2];
                  break;
      }
      /* comprobamos que los pixeles no se hayan pasado de valor */
      for(i=2;i<=4;i++)
      {
            if(mensaje[i]>255)
            {
                 mensaje[i] = 255; 
            }
            else if (mensaje[i]<0)
            {
                  mensaje[i] = 0;
            }
      }

      MPI_Bsend(&mensaje,5,MPI_INT,0,1,commPadre);

}
/**
 * 
 * Funcion que es llamada por el proceso principal, se encarga de recibir todos los puntos de los 
 * demas procesos y llamar a la funcion dibujaPunto para pintarlo
 * 
*/
void esperarPuntos(MPI_Comm intercomm)
{
      int buff[5];
      int i;
      long size_imagen = FILA_IMAGEN*FILA_IMAGEN;
      MPI_Status status;
      for(i=0;i<size_imagen;i++)
      {
            MPI_Recv(&buff,5,MPI_INT,MPI_ANY_SOURCE,1,intercomm,&status);
            dibujaPunto(buff[0],buff[1],buff[2],buff[3],buff[4]);
      }
}

/**
 * 
 * Cantidad de lineas de la imagen que le toca a cada proceso
 * 
*/
int *RepartirTarea(int rank,int size){
      /* Reparto */
     int filaReparto = FILA_IMAGEN/NUM;
     static int reparto[2];
     reparto[INICIO] = rank * filaReparto;
     reparto[FINAL] = (rank+1) * filaReparto;
     if(rank==size-1)
     {
           reparto[FINAL] = FILA_IMAGEN;
     }
    

      return reparto;
}