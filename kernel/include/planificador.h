#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <commons/log.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint32_t pid;
	uint32_t tamanio;
	t_list lista_instrucciones;
	uint32_t proxima_instruccion;
	uint32_t tabla_de_paginas;
	float estimacion_rafaga;
} pcb;

#endif