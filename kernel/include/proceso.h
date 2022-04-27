#ifndef PROCESO_H_
#define PROCESO_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel_utils.h>

typedef struct
{
	uint32_t pid;
	uint32_t tamanio;
	t_list *lista_instrucciones;
	uint32_t proxima_instruccion;
	uint32_t tabla_de_paginas;
	float estimacion_rafaga;

} pcb;

void iniciar_proceso();
void generar_estructura_PCB(pcb *, t_list *, int);
void recibir_lista_intrucciones(int, t_list *);

#endif