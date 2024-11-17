#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    std::string message;
    char buffer[1024] = {0};

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir la dirección IPv4 e IPv6 de texto a binario
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "direcion no valida\n";
        return -1;
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection failed\n";
        return -1;
    }

    std::cout << "Conectado al servidor\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.size(), 0);
        
        int valread = read(sock, buffer, 1024);
        std::cout << "Server: " << buffer << "\n";
    }

    close(sock);
    return 0;
}
