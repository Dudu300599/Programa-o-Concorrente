#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex rw_mutex;           // Mutex principal para controlar o acesso
std::condition_variable cond_var;  // Variável de condição para sincronização
int readers = 0;               // Contador de leitores
bool writer_waiting = false;   // Indicador se um escritor está esperando
bool writer_active = false;    // Indicador se um escritor está escrevendo

void start_reading() {
    std::unique_lock<std::mutex> lock(rw_mutex);
    cond_var.wait(lock, [] { return !writer_active && !writer_waiting; });
    readers++;
    std::cout << "Leitor começou a ler.\n";
}

void stop_reading() {
    std::unique_lock<std::mutex> lock(rw_mutex);
    readers--;
    if (readers == 0) {
        cond_var.notify_all();
    }
    std::cout << "Leitor terminou de ler.\n";
}

void start_writing() {
    std::unique_lock<std::mutex> lock(rw_mutex);
    writer_waiting = true;
    cond_var.wait(lock, [] { return readers == 0 && !writer_active; });
    writer_waiting = false;
    writer_active = true;
    std::cout << "Escritor começou a escrever.\n";
}

void stop_writing() {
    std::unique_lock<std::mutex> lock(rw_mutex);
    writer_active = false;
    cond_var.notify_all();
    std::cout << "Escritor terminou de escrever.\n";
}

// Funções para threads de leitura e escrita
void reader() {
    start_reading();
    // Simula a leitura
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop_reading();
}

void writer() {
    start_writing();
    // Simula a escrita
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    stop_writing();
}

int main() {
    // Exemplo de uso com várias threads leitoras e escritoras
    std::thread r1(reader), r2(reader), r3(reader);
    std::thread w1(writer), w2(writer);

    r1.join();
    r2.join();
    r3.join();
    w1.join();
    w2.join();

    return 0;
}

