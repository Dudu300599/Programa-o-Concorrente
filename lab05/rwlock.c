#include "list_int.h"
#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define QTDE_OPS 10000 // quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100  // quantidade de insercoes iniciais na lista
#define MAX_VALUE 100 // valor maximo a ser inserido

// lista compartilhada iniciada
struct list_node_s *head_p = NULL;

// quantidade de threads no programa
int nthreads;

// variáveis para controle de prioridade de escrita
pthread_mutex_t mutex;    // mutex para controle de acesso
pthread_cond_t cond_leit; // condição para leitores
pthread_cond_t cond_escr; // condição para escritores
int leit = 0;             // contagem de leitores ativos
int escr = 0;             // contagem de escritores ativos
int querEscrever = 0;     // contagem de escritores que querem escrever

// função para entrada de leitura
void EntraLeitura() {
  pthread_mutex_lock(&mutex);
  while (escr > 0 ||
         querEscrever > 0) { // espera se houver escritores ativos ou esperando
    printf("Leitor em espera!\n");
    pthread_cond_wait(&cond_leit, &mutex);
  }
  leit++;
  printf("Leitor começou a ler!\n-------\nLeitores ativos:%d\nEscritores "
         "ativos:%d\nEscritores em "
         "espera:%d\n-------\n",
         leit, escr, querEscrever);
  pthread_mutex_unlock(&mutex);
}

// função para saída de leitura
void SaiLeitura() {
  pthread_mutex_lock(&mutex);
  leit--;
  printf("Leitor finalizou!\n-------\nLeitores ativos:%d\nEscritores "
         "ativos:%d\nEscritores "
         "em espera:%d\n-------\n",
         leit, escr, querEscrever);
  if (leit == 0) {
    pthread_cond_signal(
        &cond_escr); // notifica escritores se não houver leitores ativos
  }
  pthread_mutex_unlock(&mutex);
}

// função para entrada de escrita
void EntraEscrita() {
  pthread_mutex_lock(&mutex);
  printf("Novo Escritor\n");
  querEscrever++;
  while (leit > 0 ||
         escr > 0) { // espera enquanto há leitores ou escritores ativos
    printf("Escritor em espera!\n-------\nLeitores ativos:%d\nEscritores "
           "ativos:%d\nEscritores "
           "em espera:%d\n-------\n",
           leit, escr, querEscrever);
    pthread_cond_wait(&cond_escr, &mutex);
  }
  escr++;
  querEscrever--;
  printf("Escritor começou a escrever!\n-------\nLeitores "
         "ativos:%d\nEscritores ativos:%d\nEscritores em "
         "espera:%d\n-------\n",
         leit, escr, querEscrever);
  pthread_mutex_unlock(&mutex);
}

// função para saída de escrita
void SaiEscrita() {
  pthread_mutex_lock(&mutex);
  escr--;
  printf("Escritor finalizou!\n-------\nLeitores ativos:%d\nEscritores "
         "ativos:%d\nEscritores "
         "em espera:%d\n-------\n",
         leit, escr, querEscrever);
  if (querEscrever > 0) {
    pthread_cond_signal(&cond_escr); // sinaliza o próximo escritor na fila
  } else {
    pthread_cond_broadcast(
        &cond_leit); // sinaliza leitores se não houver mais escritores
  }
  pthread_mutex_unlock(&mutex);
}

// tarefa das threads
void *tarefa(void *arg) {
  long int id = (long int)arg;
  int op, in = 0, out = 0, read = 0;

  for (long int i = id; i < QTDE_OPS; i += nthreads) {
    op = rand() % 100;
    if (op < 60) { // operação de leitura
      EntraLeitura();
      Member(i % MAX_VALUE, head_p); // ignora valor de retorno
      SaiLeitura();
      read++;
    } else if (op >= 60 && op < 90) { // operação de inserção (escrita)
      EntraEscrita();
      Insert(i % MAX_VALUE, &head_p); // ignora valor de retorno
      SaiEscrita();
      in++;
    } else if (op >= 90) { // operação de remoção (escrita)
      EntraEscrita();
      Delete(i % MAX_VALUE, &head_p); // ignora valor de retorno
      SaiEscrita();
      out++;
    }
  }

  // registra a quantidade de operações realizadas por tipo
  printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t *tid;
  double ini, fim, delta;

  // verifica se o número de threads foi passado na linha de comando
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  // insere os primeiros elementos na lista
  for (int i = 0; i < QTDE_INI; i++) {
    Insert(i % MAX_VALUE, &head_p); // ignora valor de retorno
  }

  // aloca espaço de memória para o vetor de identificadores de threads no
  // sistema
  tid = malloc(sizeof(pthread_t) * nthreads);
  if (tid == NULL) {
    printf("--ERRO: malloc()\n");
    return 2;
  }

  // inicializa mutex e variáveis condicionais
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_escr, NULL);
  pthread_cond_init(&cond_leit, NULL);

  // tomada de tempo inicial
  GET_TIME(ini);

  // cria as threads
  for (long int i = 0; i < nthreads; i++) {
    if (pthread_create(tid + i, NULL, tarefa, (void *)i)) {
      printf("--ERRO: pthread_create()\n");
      return 3;
    }
  }

  // aguarda as threads terminarem
  for (int i = 0; i < nthreads; i++) {
    if (pthread_join(*(tid + i), NULL)) {
      printf("--ERRO: pthread_join()\n");
      return 4;
    }
  }

  // tomada de tempo final
  GET_TIME(fim);
  delta = fim - ini;
  printf("Tempo: %lf\n", delta);

  // libera recursos
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_escr);
  pthread_cond_destroy(&cond_leit);

  // libera o espaço de memória do vetor de threads
  free(tid);

  // libera o espaço de memória da lista
  Free_list(&head_p);

  return 0;
}
