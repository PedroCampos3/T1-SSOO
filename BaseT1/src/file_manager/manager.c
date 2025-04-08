// Import used global libraries
#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <string.h> // strtok, strcpy, etc.
#include <stdlib.h> // malloc, calloc, free, etc.
#include <signal.h> //para el uso de señales
#include <unistd.h> //para el uso de alarm()
#include <sys/time.h>   // Para gettimeofday
#include <assert.h>     // Para assert



// Import the header file of this module
#include "manager.h"


void manejador_senal(int senal){
  if(senal == SIGALRM){
    
    alarm(valor_alarma);
  }
}

void Swap(Process *padre, Process *hijo_cambio){

  Process *abuelo = padre->adjacentes[0];
  if(abuelo){//cambia uno de los punteros del abuelo
    if(abuelo->adjacentes[1] == padre) abuelo->adjacentes[1] = hijo_cambio;
    else abuelo->adjacentes[2] = hijo_cambio;
  } 

  Process *nietos[3];
  nietos[1] = hijo_cambio->adjacentes[1];
  nietos[2] = hijo_cambio->adjacentes[2];
  for(int i=0;i<3;i++){//cambia los punteros de los nietos (hijos del hijo_cambio)
    if(nietos[i]) nietos[i]->adjacentes[0] = padre;
  }

  //quiero saber cuál es el hijo que no cambia de lugar
  Process* otro_hijo;
  int posicion_otro_hijo; 
  if(padre->adjacentes[1] == hijo_cambio){otro_hijo = padre->adjacentes[2]; posicion_otro_hijo = 2;}
  else{otro_hijo = padre->adjacentes[1]; posicion_otro_hijo = 1;}
  int posicion_hijo_cambio = 3 - posicion_otro_hijo;

  if(otro_hijo){//si hay otro hijo, a parte del que se intercambia con el padre, hay que cambiar uno de sus punteros
    otro_hijo->adjacentes[0] = hijo_cambio;
  }

  Process* adjacentes_hijo_cambio[3]; //para heredar punteros del hijo_cambio al padre
  adjacentes_hijo_cambio[1] = hijo_cambio->adjacentes[1];
  adjacentes_hijo_cambio[2] = hijo_cambio->adjacentes[2];

  Process* adjacentes_padre[3]; //para heredar los punteros del padre al hijo cambio
  adjacentes_padre[0] = padre->adjacentes[0];
  adjacentes_padre[posicion_otro_hijo] = padre->adjacentes[posicion_otro_hijo];

  hijo_cambio->adjacentes[posicion_hijo_cambio] = hijo_cambio->adjacentes[0]; //su hijo pasa a su padre
  padre->adjacentes[0] = padre->adjacentes[posicion_hijo_cambio];//su padre pasa a ser su hijo

  for(int i=0;i<3;i++){//se heredan los punteros correspondientes
    if(padre->adjacentes[i]) padre->adjacentes[i] = adjacentes_hijo_cambio[i];//le paso al padre los anteriores hijos del hijo cambio
    if(hijo_cambio->adjacentes[i])hijo_cambio->adjacentes[i] = adjacentes_padre[i];//le paso al hijo cambio el anterior padre y otro hijo de su padre
  }
}

Process *mayor_prioridad(Process* proceso1, Process* proceso2){//el proceso1 gana si: 1) la prioridad del proceso con el que estamos comparando es mayor o si 2) tienen igual prioridad pero e id del otro proceso es menor
  if ((proceso1->priority > proceso2->priority) || ((proceso1->priority == proceso2->priority) && (proceso1->pid > proceso2->pid))) return proceso1;
  return proceso2;
}

void swapUp(Process * proceso, Process * padre, Queue cola){
  if(mayor_prioridad(proceso, padre) == proceso){//hago swap hacia arriba si corresponde
    if(cola.tail == proceso) cola.tail = padre;//recordar que podríamos tener que actualizar la cola (en el primer swap)
    Swap(padre, proceso);
  }//cambió la cola o la cabeza?
  if(!padre->adjacentes[0]) cola.head = proceso; // el proceso se convirtió en la nueva cabeza: anoto el cambio
  else{//si siguen habiendo nodos hacia arriba
    proceso = padre;
    padre = padre->adjacentes[0];
    swapUp(proceso, padre, cola);
  }
}

void swapDown(Process * proceso, Queue cola){
  Process* hijo_izquierdo = proceso->adjacentes[1];
  Process* hijo_derecho = proceso->adjacentes[2];
  Process* hijo_cambio;
  if(hijo_derecho){//si hay dos hijos, encontrar el de mayor prioridad
    hijo_izquierdo = proceso->adjacentes[1];
    hijo_derecho = proceso->adjacentes[2];
    if(mayor_prioridad(hijo_izquierdo, hijo_derecho) == hijo_izquierdo) hijo_cambio = hijo_izquierdo;
    else hijo_cambio = hijo_derecho;
  }
  else if(hijo_izquierdo) hijo_cambio = proceso->adjacentes[1];
  //hacer swap de ser necesario
  if(mayor_prioridad(hijo_cambio, proceso) == hijo_cambio){//hago swap hacia abajo si corresponde
    if(cola.head == proceso) cola.head = hijo_cambio;//recordar que podríamos tener que actualizar la cabeza (en el primer swap)
    Swap(proceso, hijo_cambio);
  }//cambió la cola o la cabeza?
  if(hijo_cambio == cola.tail) cola.tail = proceso; // el proceso se convirtió en la nueva cola: anoto el cambio
  else{//si siguen habiendo nodos hacia abajo
    swapDown(proceso, cola);
  }
}

void ingresar_a_cola(Queue cola, Process* proceso){
  //es heap o lista ligada?
  if(cola.quantum){//es heap
    Process* padre = cola.tail;
    if(proceso->priority < padre->priority) proceso->adjacentes[0] = padre; //si está bien en la posicion en la que está, solo guardo el padre del nuevo proceso
    else swapUp(proceso, padre, cola);
  }
  else{//es lista ligada
    cola.head->adjacentes[0] = proceso; //escribo el anterior de la cabeza: el nuevo proceso
    proceso->adjacentes[1] = cola.head; //escribo el next del nuevo proceso: la cabeza
    cola.head = proceso; //el nuevo se convierte en la nueva cabeza
  }
  cola.number_of_process ++; //el número de procesos en la cola aumentó
}

Process* extraer_de_cola(Queue cola){
  //es heap o lista ligada?
  Process* proceso;
  if(cola.quantum){//es heap
    proceso = cola.head;//este es el que voy a retornar
    Process *proceso_swap = cola.tail;
    //traemos un proceso desde abajo reemplazar la cabeza extraída
    cola.head = proceso_swap;//actualizo el puntero de la cabeza
    cola.tail = proceso_swap->adjacentes[0];//actualizo el puntero de la cola
    for(int i=0;i<3;i++){
      proceso_swap->adjacentes[i] = proceso->adjacentes[i];//pasar punteros del proceso extraído a la nueva cabeza
      proceso->adjacentes[i] = NULL;//limpiar punteros del proceso extraído
    }
  }
  else{//es lista ligada
    proceso = cola.tail;//este es el que voy a retornar
    Process *nueva_cola = proceso->adjacentes[0];
    cola.tail = nueva_cola;//el proceso de la izquierda al extraído se convierte en la nueva cola
    nueva_cola->adjacentes[1] = NULL;//la cola no puede tener un next
    for(int i=0;i<3;i++) proceso->adjacentes[i] = NULL;//limpiar punteros del proceso extraído
  }
  cola.number_of_process --; //el número de procesos en la cola decreció
  return proceso;
}

void free_colas(){
  free_procesos(High->head);
  free_procesos(Medium->head);
  free_procesos(Low->head);
}

void free_procesos(Process* proceso){
  if(proceso->priority > 10){//hago free de un nodo del heap
    if(proceso->adjacentes[1]) free_procesos(proceso->adjacentes[1]);//llamo a liberar hijo izquierdo
    if(proceso->adjacentes[2]) free_procesos(proceso->adjacentes[2]);//llamo a liberar hijo derecho
  }
  else{
    if(proceso->adjacentes[1]) free_procesos(proceso->adjacentes[1]);//llamo a liberar a next
  }
  free(proceso);
}

void runningToWaiting(Process* proceso){//lleva a cabo las cosas que hay que hacer al pasar del estado RUNNING a WAITING
}

void waitingToReady(Process* proceso){//lleva a cabo las cosas que hay que hacer al pasar del estado WAITING a READY
}

void ReadyToRunning(Process* proceso){//lleva a cabo las cosas que hay que hacer al pasar del estado READY a RUNNING

}

double GetTime() {//para obtener el tiempo actual
  struct timeval t;
  int rc = gettimeofday(&t, NULL);
  assert(rc == 0);
  return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

/*
* Splits a string "str" by a separator "sep", returns an array with The
* resulting strings. Equivalent to Python's str.split(sep).
*/
static char** split_by_sep(char* str, char* sep)
{
  char** new_str = calloc(MAX_SPLIT, sizeof(char*));
  int index = 0;

  char* token = strtok(str, sep);
  while (token != NULL) {
    new_str[index] = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(new_str[index++], token);
    token = strtok(NULL, sep);
  }
  return new_str;
}

/*
* Reads a generic file with the following structure:
*
* 1. n
* 2. data,data,data,...
* ...
* n+1. data,data,data,...
*
* Where "n" is the amount of data lines where each of data lines are
* comma-separated. The file is returned as a InputFile struct.
*/
InputFile* read_file(char* filename)
{
  // Read the file
  FILE* file_pointer = fopen(filename, "r");

  // Declare a buffer and read the first line
  char buffer[BUFFER_SIZE];
  fgets(buffer, BUFFER_SIZE, file_pointer);

  // Define the struct and prepare its attributes
  InputFile* input_file = malloc(sizeof(InputFile));
  input_file->lines = calloc(atoi(buffer), sizeof(char**));
  input_file->len = atoi(buffer);

  // Parse each line
  int index = 0;
  while (fgets(buffer, BUFFER_SIZE, file_pointer)) {
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;
    input_file->lines[index++] = split_by_sep(buffer, " ");
  }

  // Close the file and return the struct
  fclose(file_pointer);
  return input_file;
}

/*
* Frees all the memory used by "input_file".
*/
void input_file_destroy(InputFile* input_file)
{
  // Free each separated data line and its contents
  for (int i = 0; i < input_file->len; i++) {
    for (int j = 0; j < MAX_SPLIT; j++) {
      free(input_file->lines[i][j]);
    }
    free(input_file->lines[i]);
  }

  // Free the lines array
  free(input_file->lines);

  // Free the input_file itself
  free(input_file);
}