#include <consola_utils.h>

int crear_conexion_con_kernel(void)
{
	return crear_conexion_con_servidor("127.0.0.1", "5000");
}

void liberar_conexion_con_kernel(int socketConsola)
{
	liberar_conexion_con_servidor(socketConsola);
}

void leer_lineas_codigo(FILE *archivoCodigo, t_linea_codigo *lineaCodigo, t_list *listaLineasCodigo)
{
	char *linea = leer_linea(archivoCodigo);
	char **tokens = obtener_tokens(linea);

	int cantidadTokens = cantidad_de_tokens(linea);
	lineaCodigo->parametros[0] = -1;
	lineaCodigo->parametros[1] = -1;

	if (cantidadTokens > 0)
		lineaCodigo->identificador = tokens[0];
	if (cantidadTokens > 1)
	{
		lineaCodigo->parametros[0] = atoi(tokens[1]);

		if (strcmp(tokens[0], "NO_OP") == 0)
		{
			lineaCodigo->parametros[0] = -1;

			for (int i = 0; i < atoi(tokens[1]) - 1; i++)
				list_add(listaLineasCodigo, lineaCodigo);
		}
	}

	if (cantidadTokens > 2)
		lineaCodigo->parametros[1] = atoi(tokens[2]);

	list_add(listaLineasCodigo, lineaCodigo);
}

void eliminar_salto_de_linea(char *text)
{
	if (text[string_length(text) - 1] == '\n')
		text[string_length(text) - 1] = '\0';
}

char *leer_linea(FILE *archivo)
{
	char *linea = string_new();
	unsigned int tamanioBuffer = 0;
	getline(&linea, &tamanioBuffer, archivo);

	eliminar_salto_de_linea(linea);

	return linea;
}

int apariciones(char *cadena, char caracter)
{
	int contador = 0;

	for (int i = 0; i < string_length(cadena); i++)
		if (cadena[i] == caracter)
			contador++;

	return contador;
}

int cantidad_de_tokens(char *string)
{
	return apariciones(string, ' ') + 1;
}

char **obtener_tokens(char *string)
{
	return string_n_split(string, cantidad_de_tokens(string), " ");
}

void terminar_parseo(FILE *file, t_list *list)
{
	list_destroy(list);
	txt_close_file(file);
}