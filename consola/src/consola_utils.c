#include <consola_utils.h>

int crear_conexion_con_kernel(void)
{
	return crear_conexion_con_servidor("127.0.0.1", "5000");
}

void liberar_conexion_con_kernel(int socketConsola)
{
	liberar_conexion_con_servidor(socketConsola);
}

t_linea_codigo *parser_archivo_codigo(char *rutaArchivo)
{
	FILE *archivoCodigo = fopen(rutaArchivo, "r");
	puts("reading file...");
	t_list *lineasCodigo = list_create();

	while (!feof(archivoCodigo))
	{
		char *linea = leerLinea(archivoCodigo);
		char **tokens = obtenerTokens(linea);

		t_linea_codigo *lineaCodigo = malloc(sizeof(t_linea_codigo *));
		lineaCodigo->identificador = string_new();
		lineaCodigo->parametros[0] = -1;
		lineaCodigo->parametros[1] = -1;

		if (cantidadTokens(linea) > 0)
			lineaCodigo->identificador = tokens[0];

		if (cantidadTokens(linea) > 1)
			lineaCodigo->parametros[0] = atoi(tokens[1]);

		if (cantidadTokens(linea) > 2)
			lineaCodigo->parametros[1] = atoi(tokens[2]);

		list_add(lineasCodigo, lineaCodigo);
	}

	return lineasCodigo;
}

void eliminarSaltoDeLinea(char *text)
{
	if (text[string_length(text) - 1] == '\n')
		text[string_length(text) - 1] = '\0';
}

char *leerLinea(FILE *archivo)
{
	char *linea = string_new();
	int tamanioBuffer = 0;
	getline(&linea, &tamanioBuffer, archivo);

	eliminarSaltoDeLinea(linea);

	return linea;
}

int vecesQueAparece(char *cadena, char caracter)
{
	int contador = 0;

	for (int i = 0; i < string_length(cadena); i++)
		if (cadena[i] == caracter)
			contador++;

	return contador;
}

int cantidadTokens(char *string)
{
	return vecesQueAparece(string, ' ') + 1;
}

char **obtenerTokens(char *string)
{
	return string_n_split(string, cantidadTokens(string), " ");
}

void terminateProgram(FILE *file, t_list *list)
{
	list_destroy(list);
	txt_close_file(file);
}