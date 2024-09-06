#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int inicio, fim;
  float *A, *B, *C;
  int N, M, P;
} ThreadArgs;

// Função para carregar a matriz de um arquivo binário
float *carregar_matriz(const char *nome_arquivo, int *linhas, int *colunas) {
  FILE *descritorArquivo;
  size_t ret;
  long long int tam;
  float *matriz;

  // Abre o arquivo binário
  descritorArquivo = fopen(nome_arquivo, "rb");
  if (!descritorArquivo) {
    fprintf(stderr, "Erro de abertura do arquivo\n");
    exit(2);
  }

  // Lê as dimensões da matriz
  ret = fread(linhas, sizeof(int), 1, descritorArquivo);
  ret = fread(colunas, sizeof(int), 1, descritorArquivo);

  tam = (*linhas) * (*colunas);

  // Aloca memória para a matriz
  matriz = (float *)malloc(sizeof(float) * tam);
  if (!matriz) {
    fprintf(stderr, "Erro de alocação da memória da matriz\n");
    exit(3);
  }

  // Lê os elementos da matriz do arquivo binário
  ret = fread(matriz, sizeof(float), tam, descritorArquivo);
  if (ret < tam) {
    fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
    exit(4);
  }

  fclose(descritorArquivo);
  return matriz;
}

// Função para salvar a matriz em um arquivo binário
void salvar_matriz(const char *nome_arquivo, float *matriz, int linhas,
                   int colunas) {
  FILE *descritorArquivo = fopen(nome_arquivo, "wb");
  if (!descritorArquivo) {
    fprintf(stderr, "Erro ao abrir o arquivo para escrita\n");
    exit(5);
  }

  fwrite(&linhas, sizeof(int), 1, descritorArquivo);
  fwrite(&colunas, sizeof(int), 1, descritorArquivo);
  fwrite(matriz, sizeof(float), linhas * colunas, descritorArquivo);

  fclose(descritorArquivo);
}

// Função para multiplicação parcial feita por cada thread
void *multiplicar_parcial(void *args) {
  ThreadArgs *dados = (ThreadArgs *)args;
  int inicio = dados->inicio;
  int fim = dados->fim;

  for (int i = inicio; i < fim; i++) {
    for (int j = 0; j < dados->P; j++) {
      dados->C[i * dados->P + j] = 0;
      for (int k = 0; k < dados->M; k++) {
        dados->C[i * dados->P + j] +=
            dados->A[i * dados->M + k] * dados->B[k * dados->P + j];
      }
    }
  }

  return NULL;
}

// Função concorrente para multiplicar as matrizes
void multiplicar_matrizes_concorrente(float *A, float *B, float *C, int N,
                                      int M, int P, int num_threads) {
  pthread_t threads[num_threads];
  ThreadArgs args[num_threads];
  int chunk_size = N / num_threads;

  for (int i = 0; i < num_threads; i++) {
    args[i].inicio = i * chunk_size;
    args[i].fim = (i == num_threads - 1) ? N : (i + 1) * chunk_size;
    args[i].A = A;
    args[i].B = B;
    args[i].C = C;
    args[i].N = N;
    args[i].M = M;
    args[i].P = P;

    pthread_create(&threads[i], NULL, multiplicar_parcial, &args[i]);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    printf("Uso: %s <arquivoA> <arquivoB> <arquivoSaida> <num_threads>\n",
           argv[0]);
    return EXIT_FAILURE;
  }

  int N, M, P, num_threads;
  float *A, *B, *C;
  double start, finish, elapsed;

  num_threads = atoi(argv[4]);

  // Carregar as matrizes
  GET_TIME(start);
  A = carregar_matriz(argv[1], &N, &M);
  B = carregar_matriz(argv[2], &M, &P);
  C = (float *)malloc(N * P * sizeof(float));
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Tempo de inicialização: %lf segundos\n", elapsed);

  // Multiplicação concorrente
  GET_TIME(start);
  multiplicar_matrizes_concorrente(A, B, C, N, M, P, num_threads);
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Tempo de multiplicação concorrente: %lf segundos\n", elapsed);

  // Salvar a matriz resultante
  GET_TIME(start);
  salvar_matriz(argv[3], C, N, P);
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Tempo de finalização: %lf segundos\n", elapsed);

  // Liberar memória
  free(A);
  free(B);
  free(C);

  return 0;
}
