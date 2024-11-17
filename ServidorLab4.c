#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8080
#define MAX_CLIENTS 10

std::mutex clients_mutex;
std::map<int, std::string> clients;

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    std::string message;

    while (true) {
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Cliente desconectado\n";
            clients_mutex.lock();
            clients.erase(client_socket);
            clients_mutex.unlock();
            close(client_socket);
            break;
        }

        message = buffer;
        std::cout << "Received: " << message << "\n";
        
    
        clients_mutex.lock();
        for (auto& client : clients) {
            if (client.first != client_socket) {
                send(client.first, message.c_str(), message.size(), 0);
            }
        }
        clients_mutex.unlock();
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("escuchar");
        exit(EXIT_FAILURE);
    }

    std::cout << "el servidor esta escuchando y esta conectado al puerto  " << PORT << "\n";

    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Aceptar");
            exit(EXIT_FAILURE);
        }

        std::cout << "Nuevo cliente conectado\n";
        
        clients_mutex.lock();
        clients[client_socket] = "cliente_" + std::to_string(client_socket);
        clients_mutex.unlock();

        std::thread(handle_client, client_socket).detach();
    }

    return 0;
}
