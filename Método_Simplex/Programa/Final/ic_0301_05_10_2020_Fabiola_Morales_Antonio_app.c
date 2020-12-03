/** 
 * @file ic_0301_05_10_2020_Fabiola_Morales_Antonio_app.c
 * @version 5.11.4.9.2
 * @date 27/11/2020
 * @author Fabiola Morales Antonio
 * @brief Colocar los valores 
 */
#include "ic_0301_05_10_2020_Fabiola_Morales_Antonio_simplex.h"

/**
 * @brief Lee los datos ingresados 
 * @param var Son las variables que deseas ingresar
 * @return Un dato entero. 
 */
int leeDatos(int *var){
int rest,c,i;

	
	printf("                   METODO SIMPLEX\n");	
	printf(" Funcion a Optimizar \nNumero de Variables: ");
	scanf("%d",var);
	
	for(c=0;c<*var;c++)
	{ 
		
		printf(" c%d: ", c+1);
		scanf("%lf",&funcion[c]);
		fflush(stdin);
		
		}
		
	
		printf("\nNumero de Restricciones: ");
		scanf("%d",&rest);
		
		fflush(stdin);
		
		for(i=0;i<rest;i++)
		{
			
			printf("Restriccion %d\n",i+1);
			for(c=0;c<*var;c++)
			{
				
				printf(" c%d: ",c+1);
				scanf("%lf",&restricciones[i][c]);
				fflush(stdin);
			}
			printf("Termino independiente ");
			scanf("%lf", &independiente[i]);
			fflush(stdin);
			}
			
			return rest;
			
	}
