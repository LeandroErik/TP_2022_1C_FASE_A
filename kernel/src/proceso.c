#include <proceso.h>

void recibir_lista_intrucciones(int socketConsola, t_list *listaInstrucciones)
{
    list_add(listaInstrucciones, "Instruccion recibida por socket");
}

void iniciar_proceso(t_list *listaInstrucciones)
{
    pcb *nuevo_proceso = malloc(sizeof(pcb));
    generar_estructura_PCB(nuevo_proceso, listaInstrucciones, 1000);
}

void generar_estructura_PCB(pcb *nuevo_pcb, t_list *listaInstrucciones, int tamanioProceso)
{
    nuevo_pcb->pid = id_proceso_total;
    nuevo_pcb->tamanio = tamanioProceso;
    nuevo_pcb->lista_instrucciones = listaInstrucciones;
    nuevo_pcb->proxima_instruccion = 1;
    nuevo_pcb->tabla_de_paginas = 1;
    nuevo_pcb->estimacion_rafaga = 1.1;

    id_proceso_total++;
}