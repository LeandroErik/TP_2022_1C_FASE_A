#include <socket/protocolo.h>

int recibir_operacion(int socket_cliente)
{
    int cod_op;
    if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

void *recibir_buffer(int *size, int socket_cliente)
{
    void *buffer;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
}

void recibir_mensaje(int socket_cliente)
{
    int size;
    char *buffer = recibir_buffer(&size, socket_cliente);
    // log_info(logger, "Me llego el mensaje %s", buffer);
    free(buffer);
}

t_list *recibir_paquete(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = recibir_buffer(&size, socket_cliente);
    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}

// Cliente

void *serializar_paquete(t_paquete *paquete, int bytes)
{
    void *magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigoOperacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;

    return magic;
}

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(int cod_operacion)
{
    t_paquete *paquete = (t_paquete *)malloc(sizeof(t_paquete));
    paquete->codigoOperacion = cod_operacion;
    crear_buffer(paquete);
    return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

void liberar_conexion(int socket_cliente)
{
    close(socket_cliente);
}

pcb *deserializar_pcb_p(t_list *listaRecibida)
{
    pcb *proceso = malloc(sizeof(pcb));

    proceso->pid = *(int *)list_get(listaRecibida, 0);
    proceso->tamanio = *(int *)list_get(listaRecibida, 1);
    proceso->proxima_instruccion = *(int *)list_get(listaRecibida, 2);
    proceso->tabla_de_paginas = *(int *)list_get(listaRecibida, 3);
    proceso->estimacion_rafaga = *(float *)list_get(listaRecibida, 4);
    proceso->estado = *(int *)list_get(listaRecibida, 5);

    agregar_instrucciones_a_pcb_p(proceso, listaRecibida);
    return proceso;
}

void agregar_instrucciones_a_pcb_p(pcb *proceso, t_list *listaRecibida)
{
    int tamanio_lista = *(int *)list_get(listaRecibida, 6);

    proceso->lista_instrucciones = list_create();

    for (int i = 0; i < tamanio_lista; i++)
    {
        char *instruccion = string_new();
        instruccion = (char *)list_get(listaRecibida, 7 + i);
        list_add(proceso->lista_instrucciones, instruccion);
    }
}