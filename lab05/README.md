# Implementação do Padrão Leitores/Escritores com Prioridade para Escrita

## Introdução

Este projeto implementa o padrão leitores/escritores com prioridade para operações de escrita, onde as operações de leitura não podem iniciar enquanto uma operação de escrita estiver em andamento ou enquanto houver escritores esperando.

O código consiste em:
- Um mutex para controle de acesso ao recurso compartilhado.
- Condições de espera para leitores e escritores.
- Variáveis que contabilizam leitores e escritores ativos, além de escritores que desejam escrever e leitores que estão esperando.

## Análise de Prioridade de Escrita

Durante a execução do programa, observamos a seguinte dinâmica:

- **Mensagens de Log**: A implementação inclui mensagens de log detalhadas que indicam quando leitores estão esperando e quando escritores são ativados. Por exemplo:
  - Ao entrar, um leitor imprime:
    ```
    Novo leitor!
    -------
    Leitores ativos: 1
    Escritores ativos: 0
    Escritores em espera: 0
    -------
    ```
  - Quando um escritor está aguardando, a saída inclui:
    ```
    Escritor em espera!
    -------
    Leitores ativos: 1
    Escritores ativos: 0
    Escritores em espera: 1
    -------
    ```
- **Comportamento Observado**: Ao executar o programa, foi observado que novos leitores não puderam iniciar suas operações enquanto um escritor estava em espera, confirmando que a prioridade de escrita está sendo respeitada.
## Conclusão


## Execução do Programa

Para executar o programa, use o seguinte comando:

```bash
gcc list_int.c rwlock.c -o programa -Wall
./programa <numero_de_threads>
```

