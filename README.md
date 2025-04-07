# 💻 IPC Threads DB — Trabalho de Sistemas Operacionais

Este projeto foi desenvolvido como **trabalho da disciplina de Sistemas Operacionais** da **Universidade do Vale do Itajaí (UNIVALI)**, referente à **avaliação M1**.  
Tem como objetivo aplicar na prática os conceitos de:

- Comunicação entre processos (**IPC**) via *named pipes* (FIFO)
- Uso de **threads com pthreads**
- Controle de concorrência com **mutex**
- Simulação de um banco de dados simples com comandos `INSERT`, `DELETE`, `SELECT` e `UPDATE`

---

## 📌 Requisitos

Este sistema foi feito para rodar no **Linux**, pois utiliza:

- `unistd.h` (funções do sistema Unix/Linux)
- `pthread.h` (para criar threads e mutex)
- `mkfifo` (para criar o pipe do tipo FIFO)

---

## 🗂️ Estrutura do Projeto

```bash
.
├── cliente.c         # Cliente que envia requisições ao servidor via pipe
├── servidor.c        # Servidor que recebe requisições e processa com threads
├── banco.h           # Definições de struct e funções do banco
├── banco.json        # Base de dados simulada (em formato JSON simples)
└── pipe_bd           # FIFO (named pipe) usado para comunicação
```

---

## ⚙️ Compilação

Você pode compilar manualmente com:

```bash
gcc servidor.c -o servidor -lpthread
gcc cliente.c -o cliente -lpthread
```
---

## 🛠️ Como rodar

### 1. Crie o named pipe:

```bash
mkfifo pipe_bd
```

> Isso cria o canal de comunicação entre cliente e servidor.

---

### 2. Em um terminal, rode o servidor:

```bash
./servidor
```

> O servidor ficará ouvindo requisições enviadas pelo pipe.

---

### 3. Em outro terminal, rode o cliente:

```bash
./cliente
```

Digite comandos como:

```txt
INSERT 1 João
SELECT 1
UPDATE 1 Joãozinho
DELETE 1
```

Para sair, digite:

```txt
exit
```

---

## 📄 Comandos suportados

| Comando          | Descrição                                  |
|------------------|---------------------------------------------|
| `INSERT id nome` | Adiciona um novo registro                  |
| `DELETE id`      | Remove o registro com o ID informado       |
| `SELECT id`      | Busca registro pelo ID                     |
| `UPDATE id nome` | Atualiza o nome do registro com o ID dado  |

---

## 💾 Banco de Dados

- Os registros são armazenados no arquivo `banco.json`.
- Ao iniciar, o servidor pode carregar os dados já existentes.
- Toda alteração (INSERT, DELETE, UPDATE) é salva imediatamente no arquivo.

---

## 🧠 Extras

Além do básico (`INSERT` e `DELETE`), este projeto implementa:

- `SELECT` por ID ✅
- `UPDATE` de nome por ID ❎

---

## 👨‍🏫 Sobre o Trabalho

Este projeto faz parte da **Avaliação M1 da disciplina de Sistemas Operacionais** na **UNIVALI (Universidade do Vale do Itajaí)**.  
Foi desenvolvido com foco em demonstrar:

- Comunicação por IPC (pipes)
- Paralelismo com threads
- Controle de concorrência com mutex
- Organização de código modular em C

---

## 🧑‍💻 Autor

Feito por Ferreis, estudante de Ciência da Computação
