// Repositorio: https://github.com/joaramburu15/Tp-Final-Info-I.git //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

struct memory {
  char *response;
  size_t size;
};

static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = clientp;

  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if(!ptr)
    return 0;  /* out of memory */

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  return realsize;
}
void convertir_nombre(char *nombre) {
	char *origen[] = {
		"\\u00e1", "\\u00e9", "\\u00ed",
		"\\u00f3", "\\u00fa", "\\u00f1",
		"\\u00c1", "\\u00c9", "\\u00cd",
		"\\u00d3", "\\u00da", "\\u00d1"
	};
	
	char *nuevo[] = {
		"%C3%A1", "%C3%A9", "%C3%AD",
		"%C3%B3", "%C3%BA", "%C3%B1",
		"%C3%81", "%C3%89", "%C3%8D",
		"%C3%93", "%C3%9A", "%C3%91"
	};
	
	for (int i = 0; i < 12; i++) {
		char *p = nombre;
		
		while ((p = strstr(p, origen[i])) != NULL) {
			memcpy(p, nuevo[i], 6);
			p += 6;
		}
	}
}
	void convertir_especiales(char *nombre){
		char *origen[] = {
			"\\u00e1", "\\u00e9", "\\u00ed",
			"\\u00f3", "\\u00fa", "\\u00f1",
			"\\u00c1", "\\u00c9", "\\u00cd",
			"\\u00d3", "\\u00da", "\\u00d1"
		};
		
		char *nuevo[] = {
			"\xC3\xA1", "\xC3\xA9", "\xC3\xAD",
			"\xC3\xB3", "\xC3\xBA", "\xC3\xB1",
			"\xC3\x81", "\xC3\x89", "\xC3\x8D",
			"\xC3\x93", "\xC3\x9A", "\xC3\x91"
		};
		for(int i = 0; i < 12; i++){
			char *p = nombre;
			while ((p = strstr(p, origen[i])) != NULL) {
				memmove(p + 2, p + 6, strlen(p + 6) + 1);
				memcpy(p, nuevo[i], 2);
				p += 2;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Error: indique el archivo del token.\n");
		return 1;
	}
	
	FILE *archivo = fopen(argv[1], "r");
	
	if (archivo == NULL) {
		printf("Error al abrir el archivo del token.\n");
		return 1;
	}
	
	char token[250];
	
	if (fscanf(archivo, "Token_telegram = %249s", token) != 1) {
		printf("Error al leer el token.\n");
		fclose(archivo);
		return 1;
	}
	
	fclose(archivo);
	printf("Token leido correctamente.\n");
  char api_url[500];
  char tinicial[] = "https://api.telegram.org/bot";
  char gupd[] = "/getUpdates";
  
  strcpy(api_url,tinicial);
  strcat(api_url,token);
  strcat(api_url,gupd);
	
  CURLcode res;
  CURL *curl = curl_easy_init();
  struct memory chunk = {0};

  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, api_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	while(1){
    res = curl_easy_perform(curl);
    if (res != 0)
      printf("Error Código: %d\n", res);
	
	if(chunk.response != NULL){
	
	char *upd = strstr(chunk.response, "update_id");
	char *fecha = strstr(chunk.response, "\"date\":");
	long tiempo = 0;
	if(fecha != NULL){
		tiempo = atol(fecha + 7);
	}
	

	if (upd != NULL) {
		int numero = atoi(upd + 11);
		int offset = numero + 1;
		char texto[20];
		sprintf(texto, "%d", offset);
		char t_gu[] = "?offset=";
	
		strcpy(api_url,tinicial);
		strcat(api_url,token);
		strcat(api_url,gupd);
		strcat(api_url,t_gu);
		strcat(api_url,texto);
		curl_easy_setopt(curl, CURLOPT_URL, api_url);
		
		char *chat_id = strstr(chunk.response,"chat");
		if(chat_id != NULL){
			chat_id += 12;
			long numero_chat = atol(chat_id);
			char nchat[20];
			sprintf(nchat,"%ld",numero_chat);
			char url_mensaje[500];
			strcpy(url_mensaje,tinicial);
			strcat(url_mensaje,token);
			char sndmes[] = "/sendMessage?chat_id=";
			strcat(url_mensaje, sndmes);
			strcat(url_mensaje, nchat);
			
			char *texto_mensaje = strstr(chunk.response, "text");
			char mensaje[200] = "";
			char mensaje_original[200] = "";
			
			if (texto_mensaje != NULL) {
				texto_mensaje += 7;
				
				char *fin = strchr(texto_mensaje, '"');
				
				if (fin != NULL) {
					int cantidad = fin - texto_mensaje;
					
					if (cantidad >= 0 && cantidad < 200) {
						strncpy(mensaje, texto_mensaje, cantidad);
						mensaje[cantidad] = '\0';
						
						
						strcpy(mensaje_original, mensaje);
						
						for (int i = 0; mensaje[i] != '\0'; i++) {
							if (mensaje[i] >= 'A' && mensaje[i] <= 'Z') {
								mensaje[i] += 32;
							}
						}
					}
				}
			}
			char *nombre = strstr(chat_id, "first_name");
			
			
			if (nombre != NULL) {
				nombre += 13;
				char *n = strchr(nombre, '"');
				
				if (n != NULL) {
					*n = '\0';
					char nombre_txt[200];
					strcpy(nombre_txt,nombre);
					convertir_especiales(nombre_txt);
					if (mensaje[0] != '\0') {
						FILE *registro = fopen("registro.txt", "a");
						
						if (registro != NULL) {
							fprintf(registro, "Tiempo: %ld | Nombre: %s | Mensaje: %s\n",tiempo, nombre_txt, mensaje_original);
							fclose(registro);
						}
					} 
					convertir_nombre(nombre);
					
					char respuesta[200] = "";
					
					if (strstr(mensaje, "hola") != NULL) {
						strcat(url_mensaje, "&text=Hola,%20");
						strcat(url_mensaje, nombre);
						strcpy(respuesta, "Hola, ");
						strcat(respuesta, nombre_txt);
					}
					else if (strstr(mensaje, "chau") != NULL) {
						strcat(url_mensaje, "&text=Chau,%20");
						strcat(url_mensaje, nombre);
						strcpy(respuesta, "Chau, ");
						strcat(respuesta, nombre_txt);
					}
					
					if (strstr(url_mensaje, "&text=") != NULL) {
						free(chunk.response);
						chunk.response = NULL;
						chunk.size = 0;
						
						curl_easy_setopt(curl, CURLOPT_URL, url_mensaje);
						curl_easy_perform(curl);
						curl_easy_setopt(curl, CURLOPT_URL, api_url);
						if (chunk.response != NULL){
						char *nombre_bot = strstr(chunk.response, "first_name");
						if (nombre_bot != NULL){
							nombre_bot += 13;
							char *n_bot = strchr(nombre_bot, '"');
							char *fecha_bot = strstr(chunk.response, "\"date\":");
							long tiempo_bot = 0;
							if(fecha_bot != NULL){
								tiempo_bot = atol(fecha_bot + 7);
							}
							if (n_bot != NULL) {
								*n_bot = '\0';
								convertir_especiales(nombre_bot);
						FILE *reg_bot = fopen("registro.txt", "a");
						if (reg_bot != NULL){
						fprintf(reg_bot, "Tiempo: %ld | Nombre: %s | Mensaje: %s\n", tiempo_bot, nombre_bot, respuesta);
						fclose(reg_bot);
										}
									}
								}
							}
						}
					}
				}
			}
		}
}
    free(chunk.response);
	chunk.response = NULL;
	chunk.size = 0;
	sleep(2);
	}
    curl_easy_cleanup(curl);
  }
	return 0;
}
