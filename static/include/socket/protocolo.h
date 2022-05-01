#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>
#include <socket/cliente.h>

typedef struct
{
	int pid;
	int tamanio;
	int proxima_instruccion;
	int tabla_de_paginas;
	float estimacion_rafaga;
	t_list *lista_instrucciones;

} pcb;
typedef enum
{
	DESCONEXION_CLIENTE_P = -1,
	MENSAJE_CLIENTE_P,
	IMAGEN_PCB_P,
	INICIAR_PROCESO_P,

} cod_op;

void *recibir_buffer(int *, int);
t_list *recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

//------cliente

t_paquete *crear_paquete(int);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);

#endif