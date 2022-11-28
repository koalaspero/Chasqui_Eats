#include <math.h>
#include <stdbool.h>// Booleanos
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Function:  distance 
 * --------------------
 * Recibe 2 pares de coordenadas para calcular la distancia euclidiana.
 *    
 */
int distance(int x1, int y1, int x2, int y2){
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1);
}


/*
 * Function:  randProb 
 * --------------------
 * Calculo para trabajar con la probabilidad de que algo suceda.
 *
 *    
 */
int randProb (int chance) {
    int random = (rand()%100)+1;

    return (random <= chance);
}

/*
 * Function:  tossCoin 
 * --------------------
 * Probabilidad del 50% de que algo suceda.
 *
 *    
 */
bool tossCoin(){
	return ((rand() % 2) != 0);
}

/*
 * Function:  tossCoin 
 * --------------------
 * Probabilidad del 50% de que algo suceda.
 *
 * recorridos: puntero a arreglo de recorridos de los motorizados.
 */
int promedio(int *recorridos){
	int len = sizeof recorridos / sizeof recorridos[0];
	int sum = 0, avg = 0;
	for (int i = 0; i < len; i++) {
		sum += recorridos[i];
	}
	avg = sum / len;
	return avg;
}
