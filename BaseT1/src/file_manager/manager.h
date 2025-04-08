// Tells the compiler to compile this file once
#pragma once

// Define compile-time constants
#define MAX_SPLIT 255
#define BUFFER_SIZE 4096

// Define the struct
typedef struct inputfile {
  int len;
  char*** lines;  // This is an array of arrays of strings
} InputFile;

typedef struct process {
  char* name;
  int pid;
  char* state;                    // ∈ {RUNNING, READY, WAITING, FINISHED}
  double t_inicio;                //para ver el tiempo que ha transcurrido en el estado actual. se reinicia con cada cabio de estado
  double t_cpu_burst;             //es el tiempo que dura una ráfaga de ejecución en la CPU
  int n_burts;                    //es la cantidad de ráfagas de ejecución en la CPU que tiene el proceso
  int IO_wait;                    //tiempo de espera I/O entre ráfagas
  int priority;                   //entre 1 y 30. Sirven para decidir la cola a la que pertenecen 
  struct process* adjacentes[3];  //cuando es low, se ocupa solo dos casilla (la primera para el anterior en la lista ligada y el segundo para el siguiente), en las otras se ocupa hasta 3 casillas (la primera representa al padre y las otras dos a los hijos)
} Process;

typedef struct queue {  //de tipo heap para colas high y medium y de tipo FiFO en (lista ligada) para la cola low. observación: opto no trabajar con arreglos (y calloc) porque el número de procesos en una cola es arbitrario
  Process* head;        //tanto para heap como para lista ligada. en heap el de mayor prioridad es la cabeza
  Process* tail;        //en el heap es algun nodo sin hijos. en la lista ligada el de mayor prioridad es la cola
  int quantum;          //tiempo acomulado máximo que un proceso puede ejecutarse CPU estando en esta cola
  int number_of_process;//puede que  en el futuro me sirva saber cuántos procesos tengo en una cola, quién sabe
} Queue;

// Declare functions
InputFile* read_file(char* filename);
void input_file_destroy(InputFile* input_file);


//made-by-us functions
void manejador_senal(int senal);
void Swap(Process *padre, Process *hijo_cambio);               //intercambia de posición dos procesos en el heap
Process *mayor_prioridad(Process* proceso1, Process* proceso2);//dado dos procesos entregados, devuelve el que tiene mayor prioridad
void swapUp(Process * proceso, Process * padre, Queue cola);   //sube recursivamente un proceso en el heap hasta quedar en la posición correcta
void swapDown(Process * proceso, Queue cola);                  //baja recursivamente un proceso en el heap hasta quedar en la posición correcta
void ingresar_a_cola(Queue cola, Process* proceso);            //ingresa un proceso a la cola
Process* extraer_de_cola(Queue cola);                          //saca un proceso de la cola
void free_colas();                                              //con ayuda de free_proceso, libera todos los calloc de proceso
void free_procesos(Process* proceso);                           //se llama recursivamente para liberar todos los calloc de procesos pertenecientes a una cola

//global variables
extern Queue *High;        //  1<=prioridad<=10
extern Queue *Medium;      // 11<=prioridad<=20
extern Queue *Low;         // 21<=prioridad<=30
extern int q;              // para definir quantum de cada cola
extern int n;              // para definir la cantidad de ticks antes de cambio de priorioridad
extern double valor_alarma;// para definir el tiempo de espera entre alarma y alarma. Corresponde a n/ticks_per_second

