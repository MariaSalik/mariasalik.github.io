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
    int *State;
    // outras variáveis locais que cada processo mantém são declaradas aqui
} TipoProcesso;

TipoProcesso *processo;

/* Corpo principal do programa */

int main(int argc, char *argv[]) {
    static int N;     // número de processos do sistema distribuído
    static int token; // indica o processo que está sendo executado
    static int event, r, i;

    static char fa_name[5]; // nome da facility, o processo que esta sendo executado

    if (argc != 2) {
        puts("Uso correto: tempo <numero de processos>");
        exit(1);  // caiu fora
    }

    N = atoi(argv[1]);  // converte o nº de processos para inteiro

    smpl(0, "Um Exemplo de Simulação");  // inicia simulação
    reset();
    stream(1); // só tenho uma linha de execução

    /* vamos inicializar os processos */
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i); // escreve o id sequencial
        processo[i].id = facility(fa_name, 1);
    }

    for (i = 0; i < N; i++) {  // iniciando vetor State para cada processo
        processo[i].State = malloc(sizeof(int)*N);
        for (int j=0; j<N; j++) // cada posiçao do state
            processo[i].State[j] = -1; // começam com -1
        processo[i].State[i] = 0; // ele mesmo começa com 0
    }

    /* vamos agora fazer o escalonamento inicial de eventos */
    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i); // escalona o evento para acontecer nesse tempo
    }

    schedule(fault, 31.0, 1);  // vai falhar nesse tempo
    schedule(recovery, 61.0, 1); // vai recuperar nesse tempo

    //schedule(fault, 31.0, 2);  // vai falhar nesse tempo
    //schedule(recovery, 61.0, 2); // vai recuperar nesse tempo

    /* agora vem o loop principal do simulador */
    puts("Boa noite, sou o simulador SMPL tempo.c");
    // puts(... autor, data..., outras informações...)

    while (time() < 150.0) {
        cause(&event, &token); // verifica para cada processo se tem um evento para ele

        switch (event) {
        case test:
            if (status(processo[token].id) != 0) // processo falho não testa
                break;
            printf("O processo %d vai testar no tempo %4.1f\n", token, time());
            int t = (token+1)%N; // já que é um teste circular faço mod N

            while (status(processo[t].id) != 0 && t != token){  // enquanto não acha um correto e não testou todos
                printf("O processo %d testou o processo %d falho no tempo %4.1f\n", token, t, time());
                // tratando eventos
                if (processo[token].State[t] == -1) // se em estado unknown
                    processo[token].State[t] = 1; // atualiza para estado falho
                else if (processo[token].State[t]%2 == 0) // se par (Era correto e agora é falho -> evento)
                    processo[token].State[t]++; // incrementa
                t = (t+1)%N; // vai para o proximo processo considerando que é um teste circular            
            }       
            
            // tratando eventos
            if (processo[token].State[t] == -1) // se em estado unknown
                processo[token].State[t] = 0; // atualiza para estado correto
            else if (processo[token].State[t]%2 == 1) // se ímpar (Era falho e agora é correto -> evento)      
                processo[token].State[t]++; // incrementa
            printf("O processo %d testou o processo %d correto no tempo %4.1f\n", token, t, time());               
            printf("Vetor State resultante do processo %d: ", token); // mostra o vetor State
            for(int j=0; j<N; j++){
                printf("%d ", processo[token].State[j]);
            }
            printf("\n");

            schedule(test, 30.0, token); // um novo teste daqui a esse tempo
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
