#ifndef MAIN_H
#define MAIN_H

// estructuras

typedef struct Pagina Pagina;

typedef struct
{
    int idProceso;
    Pagina *paginaActual; // NULL = vacio
} Marco;

struct Pagina
{
    int nroPagina;
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
    int nroTablaPrimerNivel;
} TablaPrimerNivel;

typedef struct
{
    int idProceso;
    int tamanio;
    TablaPrimerNivel *tablaPrimerNivel;
} Proceso;

#endif
