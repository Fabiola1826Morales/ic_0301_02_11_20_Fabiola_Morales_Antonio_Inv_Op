/** 
 * @file app.c
 * @version 5.11.4.9.2
 * @date 27/11/2020
 * @author Fabiola Morales Antonio
 * @brief El usuario introducen los datos 
 */
 
  /* ----------------------------------------------- ----------------------------
                                 Incluye
 ------------------------------------------------- -------------------------- */
#include "Grafo.h"

int main(){
	/**
	 * @ Declaracion de datos
	 */
	int g[filas][filas];
		int inicio;
		int final;
		printf("\nNodo inicio:  ");
		scanf("%d",&inicio);	fflush(stdin);
		inicio=inicio-1;
		printf("\nNodo final:  ");
		scanf("%d",&final);		fflush(stdin);
		final=final-1;
		
		inicializar(g);
		if(menuDos(g,inicio,final)== 1){
		}else{
			printf("\nNo existe un camino");
		}

}
