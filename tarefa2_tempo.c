/* Sistemas Distribuídios 2022/2
   Autor: Maria Clara de C. Salik
   Data da última atualização: 04/12/22*/
/* Funcionalidade: tempo.c -> os processos simplesmente contam o tempo */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

/* vamos definir os eventos*/
#define test 1
#define fault 2
#define recovery 3

/* vamos definir um processo, o descritor do processo */
typedef struct {
    int id; // identificador do processo, que é a "entidade" sendo simulada, no SMPL "facility"
    // outras variáveis locais que cada processo mantém são declaradas aqui
} TipoProcesso;

TipoProcesso *processo;

/* Corpo principal do programa */

int main(int argc, char *argv[]) {
    static int N;     // número de processos do sistema distribuído
    static int token; // indica o processo que está sendo executado
    static int event, r, i;

    static char fa_name[5];

    if (argc != 2) {
        puts("Uso correto: tempo <número de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Um Exemplo de Simulação");
    reset();
    stream(1);

    /* vamos inicializar os processos */
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    /* vamos agora fazer o escalonamento inicial de eventos */
    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i);
    }

    schedule(fault, 31.0, 1); // o 1 falha
    schedule(recovery, 61.0, 1); // o 1 recupera

    //schedule(fault, 31.0, 2);  // o 2 vai falhar nesse tempo
    //schedule(recovery, 61.0, 2); // o 2 vai recuperar nesse tempo

    /* agora vem o loop principal do simulador */
    puts("Boa noite, sou o simulador SMPL tempo.c");
    // puts(... autor, data..., outras informações...)

    while (time() < 150.0) {
        cause(&event, &token);

        switch (event) {
        case test:
            if (status(processo[token].id) != 0) // processo falho não testa
                break;
            printf("O processo %d vai testar no tempo %4.1f\n", token, time());
            int t =(token+1)%N; // o próximo considerando que é um teste circular
            while (status(processo[t].id) != 0 && t != token){ // enquanto não acha um correto e testou todos
                printf("O processo %d testou o processo %d falho no tempo %4.1f\n", token, t, time());
                t =(t+1)%N; // o próximo considerando que é um teste circular
            }
            printf("O processo %d testou o processo %d correto no tempo %4.1f\n", token, t, time()); 
            schedule(test, 30.0, token);
            break;

        case fault:
            r = request(processo[token].id, token, 0);
            printf("O processo %d falhou no tempo %4.1f\n", token, time());
            break;

        case recovery:
            release(processo[token].id, token);
            printf("O processo %d recuperou no tempo %4.1f\n", token, time());
            break;

        default:
            break;
        }
    }
}
