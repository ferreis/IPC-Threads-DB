#include <stdio.h>
#include <string.h>

#define PIPE_ENTRADA "pipe_bd"
#define PIPE_RESPOSTA "pipe_resposta"

int main() {

    FILE *entrada = fopen(PIPE_ENTRADA, "w");
    FILE *resposta = fopen(PIPE_RESPOSTA, "r+");

    if (!entrada || !resposta) {
        perror("Erro ao abrir os pipes");
        return 1;
    }

    char comando[100], retorno[100];
    while (1) {
        printf("Digite uma requisi\303\247\303\243o (INSERT, DELETE, SELECT, UPDATE) ou 'exit':\n");
        fgets(comando, sizeof(comando), stdin);
        if (strncmp(comando, "exit", 4) == 0) break;

        fprintf(entrada, "%s", comando);
        fflush(entrada);

        while (fgets(retorno, sizeof(retorno), resposta)) {
            if (strncmp(retorno, "FIM", 3) == 0) break;
            printf("Servidor respondeu: %s", retorno);
        }

    }

    fclose(entrada);
    fclose(resposta);
    return 0;
}