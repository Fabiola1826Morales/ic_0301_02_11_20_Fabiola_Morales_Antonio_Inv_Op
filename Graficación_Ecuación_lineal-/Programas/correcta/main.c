/**
*@file      main.c
*@brief     Menu principal del usuario
*@author    Fabiola Morales Antonio
*@date      27-11-2020
*/
#include "gnuplot_ii.h"
#include <stdio.h>
#include <stdlib.h>

/**
  *@brief Guarda los datos ingresados por el usuario, he inicaliza las funciones de gnuplot.
  *@brief El unico que se creo fue main.c, las demas estan por defecto de gnuplot para que su funcionamiento fuera ejecutado con eficiencia.
  *@param ran1 Se coloca el rango izquierdo. 
  *@param ran2 Se coloca el rango derecho.
  *@param m   Guarda el dato "m" de la funcion.
  *@param b   Guarda el dato "r" de la funcion.
  */
  
int main() {
  
	int ran1,ran2,m,b;
	
	gnuplot_ctrl * ino;
    ino = gnuplot_init();
    
    printf("GRAFICACIÓN DE UNA ECUACION LINEAL\n");
    printf("Formula: (y = mx +- b)\n\n");
    
    printf("Dame el valor m: ");
	scanf("%d", &m);
	
	printf("Dame el valor b: ");
	scanf("%d", &b);
	
	printf("Digite el rango inicial: ");
	scanf("%d", &ran1);
	
	printf("Digite el rango final: ");
	scanf("%d", &ran2);
	
/**
  *\details  gnuplot_setstyle() es para elegir el aspecto.
  */
	gnuplot_setstyle(ino, "lines");
	
/**
  *\details  gnuplot_set_xlabel() y gnuplot_set_ylabel() asignan un nombre en las posiciones "x" y "y".
  */
	gnuplot_set_xlabel (ino, "Eje X");
    
	gnuplot_set_ylabel (ino, "Eje Y");
/**
  *\details  gnuplot_cmd() y gnuplot_cmd()  generan los ejes "x" y "y".
  */
    
	gnuplot_cmd(ino,"set xzeroaxis lt -1");
	
	gnuplot_cmd(ino,"set yzeroaxis lt -1");
	
/**
  *\details  gnuplot_cmd() y gnuplot_cmd() establecen los rangos de "x" y "y"
  */
	gnuplot_cmd(ino,"set xrange [%d:%d]",ran1, ran2);
    
	gnuplot_cmd(ino,"set yrange [%d:%d]",ran1, ran2);
	
/**
  *\details  gnuplot_plot_slope(); crea el grafico.
  */
	gnuplot_plot_slope(ino,m,b, "Funcion de la forma y=mx+b");
    

    //sleep(30);
	
	//Finalización
	gnuplot_close (ino);
	
	return 0;
}
