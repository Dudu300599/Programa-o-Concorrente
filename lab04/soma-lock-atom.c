#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long int soma = 0;     // variavel compartilhada entre as threads
pthread_mutex_t mutex; // variavel de lock para exclusao mutua
pthread_cond_t cond;   // variavel de condicao para sincronizacao
int cont = 0;          // contador de multiplos de 10

// funcao executada pelas threads
void *ExecutaTarefa(void *arg) {
  long int id = (long int)arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 100000; i++) {
    //--entrada na SC
    pthread_mutex_lock(&mutex);
    //--SC (seção critica)

    //--saida da SC
    if (cont >= 20) {
      pthread_mutex_unlock(&mutex);
      break;
    }

    soma++; // incrementa a variavel compartilhada

    // verifica se soom e multiplo de 10 e se e um novo multiplo
    if (soma % 10 == 0 && soma / 10 > cont) {
      pthread_cond_signal(&cond);       // sinaliza a thread extra
      pthread_cond_wait(&cond, &mutex); // espera a thread extra imprimir
    }
    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// funcao executada pela thread de log
void *extra(void *args) {
  printf("Extra : esta executando...\n");
  while (cont < 20) {
    pthread_mutex_lock(&mutex);

    // espera por um multiplo de 10 que ainda nao tenha sido printado
    while (soma % 10 != 0 || soma / 10 <= cont) {
      pthread_cond_wait(&cond, &mutex);
    }
    // imprime o valor de soma quando for multiplo de 10
    printf("%d° = %ld \n", cont + 1, soma);
    cont++;

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char *argv[]) {
  pthread_t *tid; // identificadores das threads no sistema
  int nthreads;   // qtde de threads (passada linha de comando)

  //--le e avalia os parametros de entrada
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  //--aloca as estruturas
  tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL) {
    puts("ERRO--malloc");
    return 2;
  }

  //--inicilaiza o mutex (lock de exclusao mutua)
  pthread_mutex_init(&mutex, NULL);
  //--inicilaiza a variavel de condicao
  pthread_cond_init(&cond, NULL);

  //--cria as threads
  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  //--cria thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  //--espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }

  //--finaliza o mutex
  pthread_mutex_destroy(&mutex);
  //--finaliza a variavel de codicao
  pthread_cond_destroy(&cond);

  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}
