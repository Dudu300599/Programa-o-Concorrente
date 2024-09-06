#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

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

// Função sequencial para multiplicar as matrizes
void multiplicar_matrizes_sequencial(float *matrizA, float *matrizB,
                                     float *matrizC, int linhasA, int colunasA,
                                     int colunasB) {
  for (int i = 0; i < linhasA; i++) {
    for (int j = 0; j < colunasB; j++) {
      matrizC[i * colunasB + j] = 0;
      for (int k = 0; k < colunasA; k++) {
        matrizC[i * colunasB + j] +=
            matrizA[i * colunasA + k] * matrizB[k * colunasB + j];
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Uso: %s <arquivoA> <arquivoB> <arquivoSaida>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int N, M, P;
  float *A, *B, *C;
  double start, finish, elapsed;

  // Carregar as matrizes
  GET_TIME(start);
  A = carregar_matriz(argv[1], &N, &M);
  B = carregar_matriz(argv[2], &M, &P);
  C = (float *)malloc(N * P * sizeof(float));
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Tempo de inicialização: %lf segundos\n", elapsed);

  // Multiplicação sequencial
  GET_TIME(start);
  multiplicar_matrizes_sequencial(A, B, C, N, M, P);
  GET_TIME(finish);
  elapsed = finish - start;
  printf("Tempo de processamento: %lf segundos\n", elapsed);

  // Salvar a matriz de saída
  GET_TIME(start);
  salvar_matriz(argv[3], C, N, P);
  GET_TIME(finish)
  elapsed = finish - start;
  printf("Tempo de finalização: %lf segundos\n", elapsed);

  free(A);
  free(B);
  free(C);

  return 0;
}
