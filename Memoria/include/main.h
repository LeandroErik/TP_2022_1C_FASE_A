#ifndef MAIN_H
#define MAIN_H

typedef struct Pagina Pagina;

typedef struct
{
    int idProceso;
    int numeroMarco;
    Pagina *paginaActual; // NULL = vacio
} Marco;

struct Pagina
{
    int numeroPagina;
    bool uso;
    bool modificado;
    Marco *marcoAsignado;
};

typedef struct
{
    t_list *entradas; // puntero a paginas
} TablaSegundoNivel;

typedef struct
{
    t_list *entradas; // cada entrada es puntero a tabla 2 nivel
    int numeroTablaPrimerNivel;
} TablaPrimerNivel;

typedef struct
{
    int idProceso;
    int tamanio;
    TablaPrimerNivel *tablaPrimerNivel;
    FILE* archivoSwap;
} Proceso;

#endif