#ifndef MAIN_H
#define MAIN_H

#include <main.c>

typedef struct
{
    // uint32_t entradas; //(uint32)
    int presencia;
    int uso;
    int modificado;
    // Entrada_tp entrada_del_marco;
} Marco; // frames

typedef struct
{
    int *nroMarco;

} TablaSegundoNivel;

typedef struct
{
    TablaSegundoNivel *TablaSegundoNivel;
    int idProcesoDuenio;
} EntradaTablaPrimerNivel;

typedef struct
{
    uint32_t id;
    uint32_t pagina;
    uint32_t marco;
    bool presencia; // demuestra que esta actualizada la entrada
    bool esta_vacia;
} Entrada_tp;

t_list *crearTablaPrimerNivel()
{
    return list_create();
}

void agregarEntradaTabLaPrimerNivel(t_list *tablaPrimerNivel, EntradaTablaPrimerNivel entrada)
{
    list_add(tablaPrimerNivel, entrada);
}

#endif