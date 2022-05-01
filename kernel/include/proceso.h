#ifndef PROCESO_H_
#define PROCESO_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel_utils.h>
#include<socket/protocolo.h>

void iniciar_proceso();
void generar_estructura_PCB(pcb *, t_list *, int);
void recibir_lista_intrucciones(int, t_list *);

#endif