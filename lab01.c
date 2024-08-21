#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//inicializar o vetor
void inicializaVetor(int* vetor, int n){
    for (int i=0; i<n; i++){
        vetor[i] = 0;
    }
}

//passa os argumentos para as threads
typedef struct{
    int *vetor;
    int inicio;
    int fim;
    int id;
} Args;

//funcao executada pelas threads
void* incrementa(void* args){
    Args* argumentos = (Args*) args;
    
    printf("Thread %d processando elementos de %d a %d\n", argumentos->id, argumentos->inicio, argumentos->fim);
    
    for (int i = argumentos->inicio; i < argumentos->fim; i++){
        argumentos->vetor[i] += 1;
    }
    
    return NULL;
}



//verifica se o vetor foi incrementado corretamente
void verificaVetor(int* vetor, int n){
    for (int i=0; i<n; i++){
        if (vetor[i] != 1){
            printf("Erro");
            return;
        }
    }
    printf("Os valores estão corretos\n");
}

//funcao main
int main(int argc, char* argv[]){

    //recebe os args
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    int *vetor = (int*) malloc(n * sizeof(int));
    pthread_t threads[m];
    Args args[m];

    int elementosThread = n / m;
    int resto = n % m;
    int inicio = 0;
    inicializaVetor(vetor, n);

    for (int i=0; i<m; i++){
        Args* argumentos = (Args*)malloc(sizeof(Args));

        int fim = inicio + elementosThread;
        if (i < resto){
            fim += 1;
        }
        args[i].vetor = vetor;
        args[i].inicio = inicio;
        args[i].fim = fim;
        args[i].id = i;

        pthread_create(&threads[i], NULL, incrementa, &args[i]);

        inicio = fim;
    }

    //espera as threads finalizarem 
    for (int i=0; i<m; i++){
        pthread_join(threads[i], NULL);
    }

    verificaVetor(vetor, n);

    return 0;
}
