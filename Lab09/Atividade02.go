package main

import (
	"fmt"
)

func tarefa(str chan string) {
	// goroutine recebe a primeira mensagem do main
	msg := <-str
	fmt.Println("[Goroutine]", msg)

	// goroutine responde ao main
	str <- "Oi Main, bom dia, tudo bem?"

	// goroutine recebe a segunda mensagem do main
	msg = <-str
	fmt.Println("[Goroutine]", msg)

	// goroutine envia resposta final para o main
	str <- "Certo, entendido."
	msg = <-str
	// goroutine finaliza
	fmt.Println("[Goroutine] finalizando")
	str <- "finalizado"
}

func main() {
	// cria um canal de comunicação não-bufferizado
	str := make(chan string)

	// inicia a goroutine que executará a função 'tarefa'
	go tarefa(str)

	// main envia a primeira mensagem
	str <- "Olá, Goroutine, bom dia!"

	// main recebe a resposta da goroutine e imprime
	msg := <-str
	fmt.Println("[Main]", msg)

	// main envia a segunda mensagem
	str <- "Tudo bem! Vou terminar tá?"

	// main recebe a resposta final da goroutine e imprime
	msg = <-str
	fmt.Println("[Main]", msg)

	str <- "finalizado"
	msg = <-str

	// Main finaliza
	fmt.Println("[Main] finalizando")
}

