#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <socket/servidor.h>
#include <socket/cliente.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <proceso.h>
#include <socket/protocolo.h>

typedef struct
{
    char *ip_kernel;
    char *ip_cpu;
    char *ip_memoria;
    char *puerto_memoria;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *puerto_escucha;
    char *algoritmo_planificacion;
    int estimacion_inicial;
    double alfa;
    int grado_multiprogramacion;
    int tiempo_maximo_bloqueado;

} kernel_config;

kernel_config valores_config;

t_log *logger;

int id_proceso_total;

/**
 * @brief Carga datos del archivo config en un struct valores_config.
 *
 */
void cargar_configuracion(void);

/**
 * @brief Iniciar el servidor del módulo Kernel para conectar el Módulo de Consola (como cliente).
 *
 * @param logger Logger de Kernel.
 *
 * @return Socket del servidor Kernel (int).
 */
int iniciar_servidor_kernel(t_log *logger);

/**
 * @brief Obtener el socket del Módulo de Consola.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 *
 * @return Socket de Consola (int).
 */
int obtener_socket_consola(int socketKernel, t_log *logger);

/**
 * @brief Apagar el servidor de Kernel.
 *
 * @param socketKernel Socket del servidor Kernel (int).
 * @param logger Logger de Kernel.
 */
void apagar_servidor_kernel(int socketKernel, t_log *logger);

/**
 * @brief Conectar Kernel con el servidor de CPU en el puerto dispatch.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_cpu_dispatch(void);

/**
 * @brief Conectar Kernel con el servidor de CPU en el puerto interrupt.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_cpu_interrupt(void);

/**
 * @brief Liberar la conexión con el servidor del CPU.
 *
 * @param socketKernel Socket del Kernel (int).
 */
void liberar_conexion_con_cpu(int socketKernel);

/**
 * @brief Conectar Kernel con el servidor de Memoria.
 *
 * @return Socket de Kernel.
 */
int crear_conexion_con_memoria(void);

/**
 * @brief escucha mensajes de un determinado socket.
 *
 * @param socket Socket del cual recibe mensajes (int).
 */
void *recibir_mensajes(int);

/**
 * @brief escucha peticiones de conexion.
 *
 * @param socketServidor Socket del servidor para aceptar clientes (int).
 */
void *iniciar_escucha(int);

/**
 * @brief conexion con cpu
 *
 */
void conectar_cpu(void);

/**
 * @brief conexion con memoria
 *
 */
void conectar_memoria(void);

#endif
