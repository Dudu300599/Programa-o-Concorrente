package main

import (
	"fmt"
	"math"
	"sync"
)

func ehPrimo(n int64) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := int64(3); i <= int64(math.Sqrt(float64(n))); i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func encontrarPrimos(start, end int64, wg *sync.WaitGroup, resultados chan<- int64) {
	defer wg.Done() //indica que esta goroutine terminou

	for i := start; i <= end; i++ {
		if ehPrimo(i) {
			resultados <- i // envia o número primo encontrado para o canal
		}
	}
}

func main() {
	var N int64 = 100 // defina o valor de N
	var M int = 4     // número de goroutines

	var wg sync.WaitGroup
	resultados := make(chan int64)

	// divida o trabalho entre as goroutines
	intervalo := N / int64(M)
	for i := 0; i < M; i++ {
		start := int64(i) * intervalo + 1
		end := start + intervalo - 1
		if i == M-1 { // Garantir que o último intervalo vá até N
			end = N
		}
		wg.Add(1)
		go encontrarPrimos(start, end, &wg, resultados)
	}

	go func() {
		wg.Wait()
		close(resultados) // fecha o canal quando todas as goroutines terminam
	}()

	// contador de primos encontrados
	contador := 0
	for primo := range resultados {
		fmt.Println(primo) // imprime o número primo encontrado
		contador++
	}

	fmt.Printf("Total de números primos encontrados: %d\n", contador)
}

