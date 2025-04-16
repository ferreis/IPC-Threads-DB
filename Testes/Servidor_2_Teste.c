#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include "banco.h"

#define PIPE_ENTRADA "pipe_bd"
#define PIPE_RESPOSTA "pipe_resposta"

pthread_mutex_t mutex_banco;
Registro banco[MAX_REGISTROS];
int num_registros = 0;

int gerar_id_unico() {
    for (int novo_id = 1; novo_id < 100000; novo_id++) {
        int existe = 0;
        for (int i = 0; i < num_registros; i++) {
            if (banco[i].id == novo_id) {
                existe = 1;
                break;
            }
        }
        if (!existe) return novo_id;
    }
    return -1;
}

void *processar_requisicao(void *arg) {
    char *requisicao = (char *)arg;
    char operacao[10], parametro1[50] = "", parametro2[50] = "";
    int id = -1;

    FILE *resposta = fopen(PIPE_RESPOSTA, "w");
    if (!resposta) {
        perror("Erro ao abrir o pipe de resposta");
        pthread_mutex_unlock(&mutex_banco);
        free(arg);
        return NULL;
    }    

    sscanf(requisicao, "%s %49s %49[^\n]", operacao, parametro1, parametro2);

    pthread_mutex_lock(&mutex_banco);

    if (strcmp(operacao, "INSERT") == 0) {
        if (strcmp(parametro1, "AUTO") == 0) {
            id = gerar_id_unico();
            strncpy(parametro1, parametro2, sizeof(parametro1));
        } else {
            id = atoi(parametro1);
        }

        if (buscar_registro(id)) {
            fprintf(resposta, "ERRO INSERT: ID %d ja existe\n", id);
        } else {
            int r = inserir_registro(id, parametro2);
            fprintf(resposta, r == 0 ? "INSERT OK: %d - %s\n" : "ERRO INSERT\n", id, parametro2);
        }

    } else if (strcmp(operacao, "DELETE") == 0) {
        id = atoi(parametro1);
        int r = deletar_registro(id);
        fprintf(resposta, r == 0 ? "DELETE OK: %d\n" : "ERRO DELETE: ID nao encontrado\n", id);

    } else if (strcmp(operacao, "SELECT") == 0) {
        if (isdigit(parametro1[0])) {
            id = atoi(parametro1);
            Registro *r = buscar_registro(id);
            if (r) {
                fprintf(resposta, "SELECT ID: %d - %s\n", r->id, r->nome);
            } else {
                fprintf(resposta, "SELECT: ID nao encontrado\n");
            }
        } else {
            int encontrou = 0;
            for (int i = 0; i < num_registros; i++) {
                if (strcasecmp(banco[i].nome, parametro1) == 0){
                    fprintf(resposta, "ID: %d Nome: %s\n", banco[i].id, banco[i].nome);
                    encontrou = 1;
                }
            }
            if (!encontrou) {
                fprintf(resposta, "SELECT: Nome nao encontrado\n");
            } else {
                fprintf(resposta, "FIM\n");
            }
        }

    } else if (strcmp(operacao, "UPDATE") == 0) {
        id = atoi(parametro1);
        Registro *r = buscar_registro(id);
        if (r  != NULL) {
            strncpy(r->nome, parametro2, 50);
            salvar_banco();
            fprintf(resposta, "UPDATE OK: %d - %s\n", r->id, r->nome);
        } else {
            fprintf(resposta, "ERRO UPDATE: ID nao encontrado\n");
        }

    } else {
        fprintf(resposta, "Operacao desconhecida\n");
    }

    pthread_mutex_unlock(&mutex_banco);
    fprintf(resposta, "FIM\n");
    fclose(resposta);
    free(arg);
    return NULL;
}

void inicializar_banco() {
    FILE *f = fopen("banco.json", "r");
    if (!f) {
        num_registros = 0;
        return;
    }

    char buffer[4096];
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    num_registros = 0;

    char *ptr = buffer;
    while ((ptr = strstr(ptr, "{\"id\":")) && num_registros < MAX_REGISTROS) {
        int id;
        char nome[50];

        if (sscanf(ptr, "{\"id\":%d,\"nome\":\"%49[^\"]\"}", &id, nome) == 2) {
            banco[num_registros].id = id;
            strncpy(banco[num_registros].nome, nome, 50);
            num_registros++;
        }
        ptr++;
    }
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&mutex_banco, NULL);
    mkfifo(PIPE_ENTRADA, 0666);
    mkfifo(PIPE_RESPOSTA, 0666);

    inicializar_banco();

    FILE *pipe = fopen(PIPE_ENTRADA, "r");
    if (!pipe) {
        perror("Erro ao abrir pipe de entrada");
        return 1;
    }

    printf("Servidor rodando e aguardando requisicoes...\n");

    char buffer[100];
    while (1) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL) {
            clearerr(pipe); // limpa o erro (EOF) e continua esperando
            continue;
        }

        pthread_t thread;
        char *requisicao = strdup(buffer);
        if (strncmp(requisicao, "exit", 4) == 0) {
            free(requisicao);
            break; // só sai do loop se alguém enviar "exit"
        }
        pthread_create(&thread, NULL, processar_requisicao, requisicao);
        pthread_detach(thread);
        
    }


    fclose(pipe);
    pthread_mutex_destroy(&mutex_banco);
    return 0;
}

// Funções auxiliares
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
