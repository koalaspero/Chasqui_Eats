#include <math.h>
#include <stdbool.h>// Booleanos
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int distance(int x1, int y1, int x2, int y2){
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1);
}

int randProb (int chance) {
    int random = (rand()%100)+1;

    return (random <= chance);
}

bool tossCoin(){
	return ((rand() % 2) != 0);
}
