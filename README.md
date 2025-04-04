# IPC Threads DB

Um mini sistema de gerenciamento de registros usando **threads** e **pipes** em C.  
Simula um servidor de banco de dados que recebe requisições via *named pipe* e processa com múltiplas threads.

---

## 🐧 Requisitos

Este projeto foi desenvolvido para **Linux**, pois depende de:

- `pthread` (threads POSIX)
- `unistd.h` (bibliotecas do sistema Unix)
- `mkfifo` (para criar named pipes)

---

## 🛠️ Compilação

compile manualmente:

```bash
gcc servidor.c -o servidor -lpthread
gcc cliente.c -o cliente -lpthread
```

---

## 📂 Estrutura

- `servidor.c` — Processa as requisições recebidas via pipe, com múltiplas threads.
- `cliente.c` — Envia comandos para o servidor via pipe.
- `banco.json` — Arquivo que armazena os dados (em formato JSON simples).
- `banco.h` — Definições e funções relacionadas ao banco de dados.

---

## 📦 Como rodar

1. Crie o pipe se ainda não existir:

```bash
mkfifo pipe_bd
```

2. Em um terminal, rode o servidor:

```bash
./servidor
```

3. Em outro terminal, envie comandos com o cliente:

```bash
./cliente
```
