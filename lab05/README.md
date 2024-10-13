# Implementação do Padrão Leitores/Escritores com Prioridade para Escrita

## Introdução

Este projeto implementa o padrão leitores/escritores com prioridade para operações de escrita, onde as operações de leitura não podem iniciar enquanto uma operação de escrita estiver em andamento ou enquanto houver escritores esperando. Essa abordagem visa evitar a fome de escritores, garantindo que eles tenham acesso garantido ao recurso compartilhado.

## Estrutura do Código

O código consiste em:
- Um mutex para controle de acesso ao recurso compartilhado.
- Condições de espera para leitores e escritores.
- Variáveis que contabilizam leitores e escritores ativos, além de escritores que desejam escrever e leitores que estão esperando.

## Como Funciona

1. **Leitores**:
   - Um leitor incrementa o contador de leitores ativos e imprime uma mensagem de log ao entrar.
   - Se houver escritores ativos ou esperando, o leitor aguardará até que os escritores possam acessar o recurso. Esta abordagem garante que a prioridade de escrita seja respeitada.
   - Ao sair, o contador de leitores ativos é decrementado e, se não houver mais leitores, um escritor aguardando é sinalizado para que ele possa prosseguir.

2. **Escritores**:
   - Um escritor incrementa um contador de "quer escritores" ao tentar entrar.
   - Se houver leitores ativos ou outros escritores ativos, o escritor aguarda até que esses escritores possam concluir suas operações.
   - Quando um escritor obtém acesso, ele incrementa o contador de escritores ativos e imprime uma mensagem de log. Ao sair, decrementa o contador e sinaliza os próximos escritores ou leitores.

## Análise de Prioridade de Escrita

Durante a execução do programa, observamos a seguinte dinâmica:

- **Mensagens de Log**: A implementação inclui mensagens de log detalhadas que indicam quando leitores estão esperando e quando escritores são ativados. Por exemplo:
  - Ao entrar, um leitor imprime:
    ```
    Novo leitor!
    -------
    Leitores: 1
    Escritores: 0
    Escritores em espera: 0
    Leitores em espera: 0
    -------
    ```
  - Quando um escritor está aguardando, a saída inclui:
    ```
    Escritor em espera!
    -------
    Leitores: 1
    Escritores: 0
    Escritores em espera: 1
    Leitores em espera: 0
    -------
    ```
- **Comportamento Observado**: Ao executar o programa, foi observado que novos leitores não puderam iniciar suas operações enquanto um escritor estava em espera, confirmando que a prioridade de escrita está sendo respeitada. A contagem de `esperaLeitores` também evidencia quantos leitores estão aguardando, ilustrando a situação atual do acesso ao recurso compartilhado.

## Conclusão

A implementação deste padrão com prioridade para operações de escrita mostrou-se eficaz na proteção contra a fome de escritores. Através da análise das mensagens de log, é possível verificar que o sistema respeita a prioridade de escrita, permitindo que escritores recebam acesso ao recurso mesmo na presença de múltiplos leitores.

## Execução do Programa

Para executar o programa, use o seguinte comando:

```bash
gcc -o programa leitor_escritor.c -lpthread
./programa <numero_de_threads>
```

## Licença

Este projeto está licenciado sob a MIT License - consulte o arquivo LICENSE para mais detalhes.

