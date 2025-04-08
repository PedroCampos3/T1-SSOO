#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"
#include <signal.h>//para envío de senales
#include <unistd.h>//para clicks_per_second

Queue* High;
Queue* Medium;
Queue* Low;
int q;
int n;
double valor_alarma;


int main(int argc, char const *argv[])
{
	signal(SIGALRM, manejador_senal); //cada cierto tiempo revisa que los procesos que se ejecutan
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	//darle valor a algunas variables globales
	long clicks_per_second = sysconf(_SC_CLK_TCK);//define la cantidad de clicks de reloj por segundo
	q = *argv[3]; 
	n = *argv[4];
	valor_alarma = (double)n/clicks_per_second;

	/*Mostramos el archivo de input en consola*/
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");
	for (int i = 0; i < input_file->len; ++i){
		//impresión proceso
		printf("  NOMBRE_PROCESO: %s\n", input_file->lines[i][0]);
		printf("  PID: %s\n", input_file->lines[i][1]);
		printf("  T_INICIO: %s\n", input_file->lines[i][2]);
		printf("  T_CPU_BURST: %s\n", input_file->lines[i][3]);
		printf("  N_BURSTS: %s\n", input_file->lines[i][4]);
		printf("  IO_WAIT: %s\n", input_file->lines[i][5]);
		printf("  PRIORITY: %s\n", input_file->lines[i][6]);
		printf("\n");
		//crear proceso e instanciar atributos
		Process* proceso = calloc(1, sizeof(Process));
		proceso->name = input_file->lines[i][0];
		proceso->pid = atoi(input_file->lines[i][1]);
		proceso->t_inicio =  atoi(input_file->lines[i][2]);
		proceso->t_cpu_burst = atoi(input_file->lines[i][3]);
		proceso->n_burts = atoi(input_file->lines[i][4]);
		proceso->IO_wait = atoi(input_file->lines[i][5]);
		proceso->priority = atoi(input_file->lines[i][6]);
		proceso->state = "READY";
		//agregar proceso a una cola
		if(proceso->priority <= 10) ingresar_a_cola(*Low, proceso);
		else if(proceso->priority <= 20) ingresar_a_cola(*Medium, proceso);
		else ingresar_a_cola(*High, proceso);
	}
	//free_colas();
	input_file_destroy(input_file);
}