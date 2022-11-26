#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>// Booleanos
#include <limits.h>
#include <math.h>
#define MAX 10 /*Numero de elementos en el arreglo*/

int distance(int x1, int y1, int x2, int y2);
int randProb (int chance);
bool tossCoin();

int numRestaurantes;
int numMotorizados;
int numClientes;
int clientesAtendidos;
int clientesNoAtendidos;

int *params;
int dimension;

struct ElementoGrilla{
	char name[15];
    int posX;
    int posY;
    bool busy;
    bool left;
    int envios;
    int recorrido;
};

struct Orden{
	float valor; 
	int restaurante;
	int motorizado;
};

struct ElementoGrilla *motorizados;
struct ElementoGrilla *restaurantes;
struct ElementoGrilla *clientes;


void print_help(){
    printf("Uso:\n\t ./ejecutable D,K,X,N,Z \n\n");
    printf(" D:\tla ubicación de motorizados, clientes y restaurantes, se define en una grilla de tamaño DxD, donde D es un numero entero entero impar.\n");
    printf(" K:\trestaurantes, distribuidos aleatoriamente en la ciudad.\n");
    printf(" X:\tmili-segundos hay un 50 por ciento de probabilidades de que aparezca, en cualquier lugar, un cliente y haga un pedido a cualquier restaurante.\n");
    printf(" N:\tmotorizados \n");
	printf(" Z:\tsi no hay un motorizado a una distancia menor o igual a Z, se rechaza la orden.  \n");
	printf("Opciones:\n");
	printf("-h: Muestra un texto de ayuda con informacion de las opciones\n");
}

int nearestRepartidor(struct ElementoGrilla clie){
	int minValue = INT_MAX;
	int motIndex = 0;
	bool reject = true;

	for(int i = 0; i < numMotorizados; i++){
		struct ElementoGrilla mot = motorizados[i];
		int dist = distance(mot.posX,mot.posY,clie.posX,clie.posY);
		if(dist < minValue){
			minValue = dist;
			printf("\n %s y %s : %d \n", mot.name, clie.name, dist);
			if(!mot.busy || !mot.left){
				reject = false;
				motIndex = i;
			}
		}
	}

	if(reject){
		return -1;
	}

	return motIndex;
}



void cargar_cliente(struct ElementoGrilla *elementos){

	struct ElementoGrilla cliente;
	char *count;

	asprintf(&count, "%d", (clientesAtendidos+1));
	strcat(strcpy(cliente.name, "Cliente-"), count);
	free(count);

	cliente.posX =  rand() % dimension + 1;
    cliente.posY =  rand() % dimension + 1;

    int restIndex = rand() % (numRestaurantes-1) + 1;
    struct ElementoGrilla rest = restaurantes[restIndex];

    int motoIndex = nearestRepartidor(rest);

    double orderVal = 100 * ((double)rand() / (double)RAND_MAX);
    bool cond = randProb(70);

    if(cond && (motoIndex!= -1)){
    	struct ElementoGrilla mot = motorizados[motoIndex];
    	printf("\n %s (%d,%d) , transaccion aceptada, %s (%d,%d) %s (%d,%d)\n",cliente.name,cliente.posX,cliente.posY,mot.name,mot.posX,mot.posY,rest.name,rest.posX,rest.posY);
    	motorizados[motoIndex].envios++;
    	int toRestaurant = distance(mot.posX,mot.posY,rest.posX,rest.posY);
    	int toClient = distance(rest.posX,rest.posY,cliente.posX,cliente.posY);
    	motorizados[motoIndex].recorrido += (toRestaurant + toClient);
  		motorizados[motoIndex].posX = cliente.posX;
  		motorizados[motoIndex].posY = cliente.posY;
    }

}

void cargar_datos(struct ElementoGrilla *elementos, int numElementos,char tipoElemento){

	char *count;	

	for(int i = 0; i < numElementos; i++){

		struct ElementoGrilla elemento;
		asprintf(&count, "%d", (i+1));
		if(tipoElemento == 'M'){
			strcat(strcpy(elemento.name, "Motorizado-"), count);
		}else if(tipoElemento == 'R'){
			strcat(strcpy(elemento.name, "Restaurante-"), count);
		}
		free(count);

	    elemento.posX =  rand() % dimension + 1;
	    elemento.posY =  rand() % dimension + 1;

	    elementos[i] = elemento;

	    // printf("\n %s en la posicion (%d,%d) \n", elemento.name, elemento.posX, elemento.posY);
	}

}

int main(int argc, char **argx){
	int status, opt, numCount = 0; 
	params = (int*) calloc(MAX, sizeof(int));
	char *listNumber;
	size_t n = 0,l = 0;

	while((opt = getopt (argc, argx, ":h")) != -1){ 
		switch(opt){
			case 'h':
				print_help();
				return 0;
			case '?':	
			default:
			    printf("Uso:\n\t ./ejecutable D,K,X,N,Z \n\n");
			    printf(" D: La ubicación de motorizados, clientes y restaurantes, se define en una grilla de tamaño DxD, donde D es un numero entero entero impar.\n");
			    printf(" K: Restaurantes, distribuidos aleatoriamente en la ciudad.\n");
			    printf(" X: Mili-segundos hay un 50 por ciento de probabilidades de que aparezca, en cualquier lugar, un cliente y haga un pedido a cualquier restaurante.\n");
			    printf(" N: Motorizados \n");
				printf(" Z: Si no hay un motorizado a una distancia menor o igual a Z, se rechaza la orden.  \n");
				printf("Opciones:\n");
				printf("-h: Muestra un texto de ayuda con informacion de las opciones\n");
				return 0;
		}
	}

	if(opt==-1){
		listNumber = strtok(argx[1], ",");
		while(listNumber != NULL){
			params[numCount++] = atoi(listNumber);
			listNumber = strtok(NULL, ",");
		}
		printf("\n Grilla de %dx%d, %d restaurantes, intervalo %d milisegundos, %d motorizados, %d kilometros de distancia  \n", params[0],params[0],params[1],params[2],params[3],params[4]);

		numRestaurantes = params[1];
		dimension = params[0];
		numMotorizados = params[3];

		motorizados = (struct ElementoGrilla *) malloc(numMotorizados * sizeof(struct ElementoGrilla));
		restaurantes = (struct ElementoGrilla *) malloc(numRestaurantes * sizeof(struct ElementoGrilla));

		cargar_datos(motorizados, numMotorizados,'M');
		cargar_datos(restaurantes, numRestaurantes,'R');

		// for(int i = 0; i < numRestaurantes; i++){

		//     printf("\n %s en la posicion (%d,%d) \n", restaurantes[i].name, restaurantes[i].posX, restaurantes[i].posY);
		// }


		// for(int i = 0; i < numMotorizados; i++){

		//     printf("\n %s en la posicion (%d,%d) \n", motorizados[i].name, motorizados[i].posX, motorizados[i].posY);
		// }

		if(tossCoin()){
			cargar_cliente(clientes);
		}

		free(params);
		free(motorizados);
		free(restaurantes);
		free(clientes);
	}
}