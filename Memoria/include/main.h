#ifndef MAIN_H
#define MAIN_H

typedef struct Pagina Pagina;

typedef struct
{
    int idProceso;
    int numeroMarco;
    Pagina *paginaActual; // NULL = marco vacio
} Marco;

struct Pagina
{
    int numeroPagina;
    bool paginaVacia;
    bool uso;
    bool modificado;
    Marco *marcoAsignado; //NULL = presencia 0
};

typedef struct
{
    Lista *entradas; 
    int numeroTablaSegundoNivel;
} TablaSegundoNivel;

typedef struct
{
    Lista *entradas; 
    int numeroTablaPrimerNivel;
} TablaPrimerNivel;

typedef struct
{
    int idProceso;
    int tamanio; //TODO: Ver donde usar este campo, es necesario?
    TablaPrimerNivel *tablaPrimerNivel;
    FILE *archivoSwap;
    Lista *paginasAsignadas;
    int posicionDelPunteroDeSustitucion;
} Proceso;

#endif