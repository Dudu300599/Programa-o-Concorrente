#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define N 500
#define BUFFER_SIZE 1000

// Buffers e variáveis de controle
char buffer1[N];
char buffer2[BUFFER_SIZE];
char *aux;

// Semáforos
sem_t buffer1_vazio;
sem_t buffer1_pronto;
sem_t buffer2_vazio;
sem_t buffer2_pronto;

void *thread1(void *arg) {
    FILE *file = fopen("entrada.txt", "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        pthread_exit(NULL);
    }
    while (1) {
        sem_wait(&buffer1_vazio); // Aguarda espaço no buffer1
        aux = fgets(buffer1, N - 1, file); 
        if (aux == NULL) {
            buffer1[0] = '\0'; // Adiciona terminador para buffer1
            sem_post(&buffer1_pronto); // Indica que buffer1 está pronto
            break; // Finaliza a leitura do arquivo
        }
        sem_post(&buffer1_pronto); // Indica que buffer1 está pronto
    }
    fclose(file);
    pthread_exit(NULL);
}

void *thread2(void *arg) {
    int input_index, output_index; // Contadores para os buffers
    int buffer1_size; // Tamanho da linha lida do buffer 1
    int block_count = 0; // Contador para o tamanho do bloco
    int char_count; // Contador de caracteres copiados

    while (1) {
        sem_wait(&buffer1_pronto); // Espera o Buffer 1 estar pronto
        sem_wait(&buffer2_vazio); // Espera o Buffer 2 estar vazio 
        buffer1_size = strlen(buffer1);

        if (buffer1_size == 0) { // Se buffer1 está vazio, termina o processamento
            buffer2[0] = '$'; // Indica que a Thread 3 pode finalizar
            sem_post(&buffer2_pronto); // Sinaliza que o Buffer 2 está cheio
            break; // finaliza
        }

        input_index = 0, output_index= 0, char_count = 0, block_count = 0;

     while (input_index < buffer1_size) {
                int current_block_size = (block_count <= 10) ? (2 * block_count + 1) : 10;

                // Copiar caracteres do buffer1 para buffer2
                for (; char_count < current_block_size && input_index < buffer1_size; char_count++) {
                    if (output_index < BUFFER_SIZE - 1) { // Evita estouro de buffer
                        buffer2[output_index++] = buffer1[input_index++];  // Copia os caracteres para Buffer2
                    } else {
                        break; // Para se o buffer2 estiver cheio
                    }
                }

                // Adiciona '\n' após o bloco
                if (char_count == current_block_size) {
                    if (output_index < BUFFER_SIZE - 1) {
                        buffer2[output_index++] = '\n';  // Adiciona '\n' após o bloco
                    }
                    char_count = 0;
                    block_count++;  // Incrementa o valor do bloco
                }
            }

            buffer2[output_index] = '\0'; // Adiciona o terminador de string
            sem_post(&buffer1_vazio); // Sinaliza que o Buffer 1 está vazio (para a Thread 1)
            sem_post(&buffer2_pronto); // Sinaliza que o Buffer 2 está cheio (para a Thread 3)
        }
        pthread_exit(NULL);
    }

void *thread3(void *arg) {
    while (1) {
        sem_wait(&buffer2_pronto); // Aguarda dados em buffer2
        if (buffer2[0] == '$') {  // Verifica se o buffer2 contém o delimitador
            break;    // Termina o loop; 
        } else {
            printf("%s", buffer2); // Imprime os dados de buffer2
        }
        sem_post(&buffer2_vazio); // Libera espaço no buffer2
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t thread_leitura, thread_processamento, thread_impressao;

    // Inicializa semáforos
    sem_init(&buffer1_vazio, 0, 1); // Um espaço vazio no buffer1
    sem_init(&buffer1_pronto, 0, 0); // Nenhum dado pronto no buffer1
    sem_init(&buffer2_vazio, 0, 1); // Um espaço vazio no buffer2
    sem_init(&buffer2_pronto, 0, 0); // Nenhum dado pronto no buffer2

    // Cria threads
    pthread_create(&thread_leitura, NULL, thread1, NULL);
    pthread_create(&thread_processamento, NULL, thread2, NULL);
    pthread_create(&thread_impressao, NULL, thread3, NULL);

    // Aguarda a finalização das threads
    pthread_join(thread_leitura, NULL);
    pthread_join(thread_processamento, NULL);
    pthread_join(thread_impressao, NULL);

    // Destrói semáforos
    sem_destroy(&buffer1_vazio);
    sem_destroy(&buffer1_pronto);
    sem_destroy(&buffer2_vazio);
    sem_destroy(&buffer2_pronto);

    return 0;
}
