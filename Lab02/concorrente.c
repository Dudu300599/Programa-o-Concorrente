#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Variáveis globais
long int n;           // Tamanho dos vetores
float *vet_a, *vet_b; // Vetores de entrada
int nthreads;         // Número de threads

// Função executada pelas threads
// Estratégia de divisão de tarefas: blocos de n/nthreads elementos
void *ProdutoInterno(void *tid) {
  long int id = (long int)tid; // Identificador da thread
  long int ini, fim, bloco;    // Auxiliares para divisão do vetor em blocos
  double *produto_local;       // Produto interno local

  bloco = n / nthreads; // Tamanho do bloco de dados de cada thread
  ini = id * bloco;     // Posição inicial do vetor
  fim = (id == nthreads - 1) ? n : ini + bloco; // Posição final do vetor (tratando elementos restantes)

  // Aloca espaço para o resultado local
  produto_local = (double *)malloc(sizeof(double));
  if (produto_local == NULL) {
    fprintf(stderr, "--ERRO: malloc() thread\n");
    pthread_exit(NULL);
  }
  *produto_local = 0.0;

  // Calcula o produto interno local
  for (long int i = ini; i < fim; i++) {
    *produto_local += vet_a[i] * vet_b[i];
  }

  // Retorna o resultado da soma local
  pthread_exit((void *)produto_local);
}

// Função principal do programa
int main(int argc, char *argv[]) {
  FILE *arq;  // Arquivo de entrada
  size_t ret; // Retorno da função de leitura no arquivo de entrada
  double produto_seq, produto_par_global; // Resultados do produto interno
  double *produto_retorno_threads;        // Auxiliar para retorno das threads
  pthread_t *tid_sistema; // Vetor de identificadores das threads no sistema

  // Valida e recebe os valores de entrada
  if (argc < 3) {
    printf("Uso: %s <arquivo de entrada> <numero de threads>\n", argv[0]);
    exit(-1);
  }

  // Abre o arquivo de entrada com os vetores a serem multiplicados
  arq = fopen(argv[1], "rb");
  if (arq == NULL) {
    printf("--ERRO: fopen()\n");
    exit(-1);
  }

  // Lê o tamanho dos vetores (primeira linha do arquivo)
  ret = fread(&n, sizeof(long int), 1, arq);
  if (!ret) {
    fprintf(stderr, "Erro de leitura das dimensões dos vetores\n");
    return 3;
  }

  // Aloca espaço de memória e carrega os vetores de entrada
  vet_a = (float *)malloc(sizeof(float) * n);
  vet_b = (float *)malloc(sizeof(float) * n);
  if (vet_a == NULL || vet_b == NULL) {
    printf("--ERRO: malloc()\n");
    exit(-1);
  }
  ret = fread(vet_a, sizeof(float), n, arq);
  if (ret < n) {
    fprintf(stderr, "Erro de leitura dos elementos do vetor A\n");
    return 4;
  }
  ret = fread(vet_b, sizeof(float), n, arq);
  if (ret < n) {
    fprintf(stderr, "Erro de leitura dos elementos do vetor B\n");
    return 5;
  }

  // Lê o número de threads da entrada do usuário
  nthreads = atoi(argv[2]);
  // Limita o número de threads ao tamanho do vetor
  if (nthreads > n)
    nthreads = n;

  // Aloca espaço para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
  if (tid_sistema == NULL) {
    printf("--ERRO: malloc()\n");
    exit(-1);
  }

  // Cria as threads
  for (long int i = 0; i < nthreads; i++) {
    if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void *)i)) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  // Produto interno sequencial
  produto_seq = 0.0;
  for (long int i = 0; i < n; i++) {
    produto_seq += vet_a[i] * vet_b[i];
  }

  // Espera todas as threads terminarem e calcula o produto interno total das
  // threads
  produto_par_global = 0.0;
  for (int i = 0; i < nthreads; i++) {
    if (pthread_join(tid_sistema[i], (void *)&produto_retorno_threads)) {
      printf("--ERRO: pthread_join()\n");
      exit(-1);
    }
    produto_par_global += *produto_retorno_threads;
    free(produto_retorno_threads);
  }

  // Imprime os resultados
  printf("\nProduto Interno (sequencial) = %.26f\n", produto_seq);
  printf("Produto Interno (concorrente) = %.26f\n", produto_par_global);

  // Calcula a variação relativa
  double variacao_relativa = (produto_seq - produto_par_global) / produto_seq;
  printf("Variação Relativa = %.26f\n", variacao_relativa);

  // Desaloca os espaços de memória
  free(vet_a);
  free(vet_b);
  free(tid_sistema);
  // Fecha o arquivo
  fclose(arq);

  return 0;
}
