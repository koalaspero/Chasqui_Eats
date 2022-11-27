#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>// Booleanos
#include <limits.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#define MAX 10 /*Numero de elementos en el arreglo*/

int distance(int x1, int y1, int x2, int y2);
int randProb (int chance);
bool tossCoin();

int numRestaurantes;
int motorizadosDisponibles;
int motorizadosRetirados;
int numMotorizados;
int numClientes;
int clientesAtendidos;
int clientesNoAtendidos;

int *params;
int dimension;
int maxDist;


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
		bool unavailableMot = !mot.busy || !mot.left; 
		if(dist < maxDist){
			minValue = dist;
			// printf("\n %s y %s : %d \n", mot.name, clie.name, dist);
			if(unavailableMot){
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

void envio_motorizado(struct ElementoGrilla cliente, int indexList[]){
	
	int motoIndex = indexList[0];
	int restIndex = indexList[1];

	struct ElementoGrilla mot = motorizados[motoIndex];
	struct ElementoGrilla rest = restaurantes[restIndex];

	printf("\n %s (%d,%d) , transaccion aceptada, %s (%d,%d) %s (%d,%d)\n",cliente.name,cliente.posX,cliente.posY,mot.name,mot.posX,mot.posY,rest.name,rest.posX,rest.posY);
	
	motorizados[motoIndex].busy = true;
	motorizados[motoIndex].envios++;

	int toRestaurant = distance(mot.posX,mot.posY,rest.posX,rest.posY);
	motorizados[motoIndex].recorrido += toRestaurant;
	
	printf(" %s, en espera por comida\n", mot.name);
	printf(" %s, Comida lista\n", cliente.name);
	printf(" %s, Comida retirada\n", mot.name);
	
	int toClient = distance(rest.posX,rest.posY,cliente.posX,cliente.posY);
	motorizados[motoIndex].recorrido += toClient;
	
	printf(" %s, Comida entregada\n", mot.name);
	
	motorizados[motoIndex].posX = cliente.posX;
	motorizados[motoIndex].posY = cliente.posY;
	motorizados[motoIndex].busy = false;
	
	printf(" %s, En espera por nuevo pedido\n", mot.name);
	
	if(motorizados[motoIndex].envios > 2){
		if(tossCoin()){
			motorizados[motoIndex].left = true;
			motorizados[motoIndex].busy = true;
			printf(" %s %d, Me retiro a mi casa\n", mot.name, mot.envios);
			motorizadosDisponibles--;
			motorizadosRetirados++;
			// printf("\n %d \n",motorizadosRetirados);
		}
	}
}



void* cargar_cliente(){

	struct ElementoGrilla cliente;
	char *count;

	asprintf(&count, "%d", ((clientesAtendidos+clientesNoAtendidos)+1));
	strcat(strcpy(cliente.name, "Cliente-"), count);
	free(count);

	cliente.posX =  rand() % dimension + 1;
    cliente.posY =  rand() % dimension + 1;

    int restIndex = rand() % (numRestaurantes-1) + 1;
    struct ElementoGrilla rest = restaurantes[restIndex];

    int motoIndex = nearestRepartidor(rest);
    clientesNoAtendidos++;

    if(motoIndex == -1){
    	pthread_exit(NULL);
    }

    double orderVal = 100 * ((double)rand() / (double)RAND_MAX);
    bool cond = randProb(70);

    if(cond && (motoIndex!= -1)){
    	clientesNoAtendidos--;
    	int indexList[] = {motoIndex,restIndex};
    	envio_motorizado(cliente, indexList);
    	clientesAtendidos++;
    	// pthread_exit(NULL);
    }

}

void cargar_datos(struct ElementoGrilla *elementos, int numElementos,char tipoElemento){

	char *count;

	for(int i = 0; i < numElementos; i++){

		struct ElementoGrilla elemento;
		asprintf(&count, "%d", (i+1));
		if(tipoElemento == 'M'){
			strcat(strcpy(elemento.name, "Motorizado-"), count);
			elemento.envios = 0;
			elemento.recorrido = 0;
			elemento.left = false;
			elemento.busy = false;
		}else if(tipoElemento == 'R'){
			strcat(strcpy(elemento.name, "Restaurante-"), count);
		}
		free(count);

	    elemento.posX =  rand() % dimension + 1;
	    elemento.posY =  rand() % dimension + 1;

	    elementos[i] = elemento;
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
		motorizadosDisponibles = params[3];
		maxDist = params[4];
		motorizadosRetirados = 0;

		double interval = params[2] / 1000; 

		motorizados = (struct ElementoGrilla *) malloc(numMotorizados * sizeof(struct ElementoGrilla));
		restaurantes = (struct ElementoGrilla *) malloc(numRestaurantes * sizeof(struct ElementoGrilla));

		cargar_datos(motorizados, numMotorizados,'M');
		cargar_datos(restaurantes, numRestaurantes,'R');

		while(motorizadosDisponibles>0){
			sleep(interval);
			pthread_t client_t;
			
			if(tossCoin()){
				cargar_cliente();
				pthread_create(&client_t, NULL, &cargar_cliente, NULL);
				pthread_join(client_t, NULL);
			}

		}

		free(params);
		free(motorizados);
		free(restaurantes);
	}
}
