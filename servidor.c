#include <stdio.h>       // entrada e saída (fopen, printf, etc.)
#include <stdlib.h>      // alocação, free, exit
#include <string.h>      // funções como strcpy, strcmp, strdup, etc.
#include <unistd.h>      // para coisas como `access()`, `mkfifo()`, etc. (importante no Linux)
#include <pthread.h>
#include "banco.h"


#define PIPE_NAME "pipe_bd"

pthread_mutex_t mutex_banco;
Registro banco[MAX_REGISTROS];
int num_registros = 0;

void *processar_requisicao(void *arg) {
    char *requisicao = (char *)arg;
    char operacao[10], nome[50];
    int id;

    sscanf(requisicao, "%s %d %49[^\n]", operacao, &id, nome);

    pthread_mutex_lock(&mutex_banco);
    if (strcmp(operacao, "INSERT") == 0) {
        inserir_registro(id, nome);
    } else if (strcmp(operacao, "DELETE") == 0) {
        deletar_registro(id);
    } else if (strcmp(operacao, "SELECT") == 0) { // devo adicionar o WHERE? devo fazer buscar pelo nome?
        Registro *r = buscar_registro(id);
        if (r) printf("Registro encontrado: %d - %s\n", r->id, r->nome);
    } /*else if UPDATE ? como devo fazer essa parte*/

    pthread_mutex_unlock(&mutex_banco);
    free(arg);
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex_banco, NULL);
    FILE *pipe = fopen(PIPE_NAME, "r");
    if (!pipe) {
        perror("Erro ao abrir pipe");
        return 1;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        pthread_t thread;
        char *requisicao = strdup(buffer);
        pthread_create(&thread, NULL, processar_requisicao, requisicao);
        pthread_detach(thread);
    }

    fclose(pipe);
    pthread_mutex_destroy(&mutex_banco);
    return 0;
}

// Implementação das funções do banco
void inicializar_banco() { num_registros = 0; } //devo fazer o banco não zerar?


void salvar_banco() {
    FILE *f = fopen("banco.json", "w");
    fprintf(f, "[");
    for (int i = 0; i < num_registros; i++) {
        fprintf(f, "{\"id\":%d,\"nome\":\"%s\"}%s", banco[i].id, banco[i].nome, (i < num_registros - 1) ? "," : "");
    }
    fprintf(f, "]");
    fclose(f);
}

int inserir_registro(int id, const char *nome) {
    if (num_registros >= MAX_REGISTROS) return -1;
    banco[num_registros].id = id;
    strncpy(banco[num_registros].nome, nome, 50);
    num_registros++;
    salvar_banco();
    return 0;
}

int deletar_registro(int id) {
    for (int i = 0; i < num_registros; i++) {
        if (banco[i].id == id) {
            banco[i] = banco[num_registros - 1];
            num_registros--;
            salvar_banco();
            return 0;
        }
    }
    return -1;
}

Registro* buscar_registro(int id) {
    for (int i = 0; i < num_registros; i++) {
        if (banco[i].id == id) return &banco[i];
    }
    return NULL;
}
