Funcionamiento del Schedueler:

1. Formato colas: los procesos se almacenan en las colas de prioridad high, medium y low; donde las colas high y medium son Max-heaps donde la cabeza es el procesos de mayor pioridad (dentro de la misma cola). Mientras tanto la cola Low fue implementada como una doble lista ligada donde los procesos se agregan en la cabeza y las extraxiones se hacen en la cola. todos son structs de tipo Queue, los que cuentan con una cabeza, cola, quantum y número de procesos.

2. Manejo del tiempo para el cambio de prioridad: con el uso de la señal SIGALARM, cada 'n' clicks de reloj (los especificados en el input), se implementa la rotación de procesos en CPU y se revisan los tiempos de ejecución para actualizar los parámetros correspondientes.

3. Variables globales que decidimos implementar:
- Queue *High,  *Medium y *Low para manejar las colas desde el manejador de alarmas y el main
- int q para poder acceder desde todas partes al quantum de cada cola
- int n para tener acceso a la cantidad de ticks antes de cambio de priorioridad
- double valor_alarma, que define el tiempo de espera entre alarma y alarma. Corresponde a n/ticks_per_second y necesitamos poder acceder a él en manejador señal para saber que valor pasarle al siguiente alarm() que se llame

4. Structs procesos: a parte de los atributos pedidos, estos tienen un atributo llamado 'adjacentes', que consiste en un arreglo de tres casillas que almacenan punteros a otros procesos. las tres casillas se ocupan de la siguiente forma:
- en heaps, la casilla cero corresponde al padre, mientras que las casillas 1 y 2 se ocupan para representar al hijo izquierdo y derecho respectivamente. Consideraciones: si el prooceso es la cabeza del heap, la casilla cero de 'adjacentes' va a estar desocupada, y si no tiene hijo derecho, la casilla dos va aestar desocupada y no tiene hijos, tanto la casilla uno como la dos van a estar desocupadas.
- en listas ligadas, la casilla cero representa al proceso anterior (izquierda), mientras que la casilla uno representa al siguiente proceso (derecha). Consideraciones: si el proceso es la cabeza, no va a terner un previo, por lo tanto, su casilla cero de 'adjacentes' va a estar desocupada. Así mismo, si el proceso es la cola, no va a tener un siguiente, por lo que si casilla uno, va a estar desocupada. La casilla dos nunca se ocupa cuando el proceso está en la lista ligada (cola Low).