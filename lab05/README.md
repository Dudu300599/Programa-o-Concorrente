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

