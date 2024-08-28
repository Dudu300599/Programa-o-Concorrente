#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 // valor máximo de um elemento do vetor

int main(int argc, char *argv[]) {
  float *vetor_a, *vetor_b;     // vetores que serão gerados
  long int n;                   // quantidade de elementos dos vetores
  double produto_interno = 0.0; // resultado do produto interno
  FILE *descritorArquivo;       // descritor do arquivo de saída

  // Recebe os argumentos de entrada
  if (argc < 3) {
    fprintf(stderr, "Uso: %s <dimensao> <arquivo_saida>\n", argv[0]);
    return 1;
  }
  n = atol(argv[1]);

  // Aloca memória para os vetores
  vetor_a = (float *)malloc(sizeof(float) * n);
  vetor_b = (float *)malloc(sizeof(float) * n);
  if (!vetor_a || !vetor_b) {
    fprintf(stderr, "Erro de alocação de memória\n");
    return 2;
  }

  // Preenche os vetores com valores float aleatórios e calcula o produto interno
  srand(time(NULL));
  for (long int i = 0; i < n; i++) {
    vetor_a[i] = (float)(rand() % MAX) / 3.0f;
    vetor_b[i] = (float)(rand() % MAX) / 3.0f;
    produto_interno += vetor_a[i] * vetor_b[i];
  }

  // Abre o arquivo para escrita binária
  descritorArquivo = fopen(argv[2], "wb");
  if (!descritorArquivo) {
    fprintf(stderr, "Erro ao abrir o arquivo para escrita\n");
    free(vetor_a);
    free(vetor_b);
    return 3;
  }

  // Escreve a dimensão no arquivo
  if (fwrite(&n, sizeof(long int), 1, descritorArquivo) != 1) {
    fprintf(stderr, "Erro ao escrever a dimensão no arquivo\n");
    fclose(descritorArquivo);
    free(vetor_a);
    free(vetor_b);
    return 4;
  }

  // Escreve os vetores no arquivo
  if (fwrite(vetor_a, sizeof(float), n, descritorArquivo) != n ||
      fwrite(vetor_b, sizeof(float), n, descritorArquivo) != n) {
    fprintf(stderr, "Erro ao escrever os vetores no arquivo\n");
    fclose(descritorArquivo);
    free(vetor_a);
    free(vetor_b);
    return 5;
  }

  // Escreve o produto interno no arquivo
  if (fwrite(&produto_interno, sizeof(double), 1, descritorArquivo) != 1) {
    fprintf(stderr, "Erro ao escrever o produto interno no arquivo\n");
    fclose(descritorArquivo);
    free(vetor_a);
    free(vetor_b);
    return 6;
  }

  // Finaliza o uso das variáveis
  fclose(descritorArquivo);
  free(vetor_a);
  free(vetor_b);

  printf("Arquivo '%s' gerado com sucesso.\n", argv[2]);
  return 0;
}
