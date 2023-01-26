/* Sistemas Distribuídios 2022/2
   Autor: Maria Clara de C. Salik
   Data da última atualização: 26/01/23*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"
#include <math.h>
#include "cisj.h"

/* vamos definir os eventos*/
#define test 1
#define fault 2
#define recovery 3

/* vamos definir um processo, o descritor do processo */
typedef struct {
    int id; // identificador do processo, que é a "entidade" sendo simulada, no SMPL "facility"
    int *state;
    // outras variáveis locais que cada processo mantém são declaradas aqui
} TipoProcesso;

TipoProcesso *processo;


/* Corpo principal do programa */

int main(int argc, char *argv[]) {
    static int N;     // número de processos do sistema distribuído
    static int token; // indica o processo que está sendo executado
    static int event, r, i, j, k, s;
    static int proximo; // o próximo processo que será testado
    static node_set *nodo;
    static int token_status;
      
    static char fa_name[5]; // facility

    if (argc != 2) {
        puts("Uso correto: tempo <número de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Um exemplo de Simulação");
    reset();
    stream(1);

    /* vamos inicializar os processos */
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
        processo[i].state = (int *)malloc(sizeof(int) * N);
        memset(processo[i].state, -1, sizeof(int) * N);
        processo[i].state[i] = 0;
    }

     /* vamos agora fazer o escalonamento inicial de eventos */
    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i);
    }
    // schedule(test, 31.0, 1);
    // schedule(test, 31.0, 2);
    // schedule(test, 31.0, 4);

    // schedule(test, 30.0, 3);
    // schedule(test, 30.0, 5);
    // schedule(test, 30.0, 6);

    // schedule(test, 29.0, 7);
    schedule(fault, 1.0, 2);//processo 2 falha no tempo 1
    schedule(fault, 1.0, 4);
    schedule(fault, 1.0,5);
    // schedule(recovery, 61.0, 1);    
    
    cause(&event, &token);

    puts("  Olá, sou o simulador SMPL vCube\n\n");

    while (time() < 150.0) {  // while(time() < 90.0)

        switch (event) {
        case test:
            for (s=1; s<= log2(N); s++){  //para cada cluster
                nodo = cis(token, s); // recebe o resultado da função cis
                for (i=0; i < nodo->size; i++){
                    proximo = nodo->nodes[i]; // o nodo seguinte
                    if (status(processo[proximo].id) == 0){
                        if (status(processo[token].id) != 0){ 
                            printf("   Processo %d testou o processo %d falho no tempo %4.1f\n", proximo, token, time());
                            if (processo[proximo].state[token] == -1){
                                processo[proximo].state[token] = 1;
                            } 
                            else if (processo[proximo].state[token] % 2 == 0){
                                processo[proximo].state[token] = processo[proximo].state[token] + 1;
                            }
                        }
                        else{
                            printf("   Processo %d testou o processo %d correto no tempo %4.1f\n", proximo, token, time());
                            if (processo[proximo].state[token] == -1) {
                                processo[proximo].state[token] = 0;
                            } 
                            else if (processo[proximo].state[token] % 2 == 1){
                                processo[proximo].state[token] = processo[proximo].state[token] + 1;
                            }

                            // Diagnosticando
                            printf("   Obtendo diagnostico ...\n");
                            for (j = 0; j < N; j++){
                                if (processo[proximo].state[j] < processo[token].state[j]) {
                                    processo[proximo].state[j] = processo[token].state[j];
                                }
                            }

                        }
                        break; // próximo cluster é testado
                    }
                }

                set_free(nodo); // função que libera nodo
            }
            // A cada intervalo de testes, o estado de cada processo é impresso
            if ((int)time() % 30 == 0){
                for (i = 0; i < N; i++){
                    if (status(processo[i].id) == 0){
                        printf("   Vetor de estado state[%d] = [", i,"]");
                        for (j = 0; j < N; j++)
                            printf("%3d", processo[i].state[j]);
                        puts("]");
                    }
                }
            }
            printf("----------------------------------------------------------------\n\n");
            schedule(test, 30.0, token);
            break;

        case fault:
            r = request(processo[token].id, token, 0);
            if(r!=0){
                puts(" Não foi possível falhar o processo....");
                break;
            }
            printf("   O processo %d falhou no tempo %3.1f\n", token, time());
            break;
 
        case recovery:
            release(processo[token].id, token);
            printf("O processo %d recuperou no tempo %3.1f\n", token, time());
            schedule(test, 30.0, token);
            break;
        }
        cause(&event, &token);
    }


    exit(0);

}
