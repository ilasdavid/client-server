#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 80;
const int MODULOS = 12;
const int BASE = 23;

long long computing(long long base, long long exponent, long long mod) {
    if (exponent == 1)
        return base;
    else
        return (((long long int)pow(base, exponent)) % mod);
}

long long generatingKeysDiffieHellman(long long base, long long modulus) {

    std::srand(std::time(0));
    long long private_key = 2 + std::rand() % (modulus - 2);

    long long public_key = computing(base, private_key, modulus);
    
    std::cout << "Server: Public key = " << public_key << std::endl;
    std::cout << "Server: Private key = " << private_key << std::endl;

    return public_key;
}

void encryptSendedMessage(char* message, long long shared_secret) {
    for (int i = 0; i < strlen(message); ++i) {
        message[i] = message[i] ^ shared_secret;
    }
}

void decryptRecievedMessage(char* message, long long shared_secret) {
    for (int i = 0; i < strlen(message); ++i) {
        message[i] = message[i] ^ shared_secret;
    }
}

int main() {
    const long int modulus = MODULOS;
    const long int base = BASE;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error: Fail to create socket!" << std::endl;
        return -1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Fail to bind socket!" << std::endl;
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 1) == -1) {
        std::cerr << "Error: Fail to recieve socket!" << std::endl;
        close(server_socket);
        return -1;
    }

    std::cout << "Server waiting for client connecton..." << std::endl;

    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
        std::cerr << "Error: Fail to accept connection. " << std::endl;
        close(server_socket);
        return -1;
    }

    std::cout << "Connection successful!" << std::endl;

    long long server_public_key = generatingKeysDiffieHellman(base, modulus);

    long long client_public_key;
    std::cout << "Server: Waiting for the public key from the client..." << std::endl;
    recv(client_socket, &client_public_key, sizeof(client_public_key), 0);

    send(client_socket, &server_public_key, sizeof(server_public_key), 0);

    long long shared_secret = computing(client_public_key, server_public_key, modulus);

    char received_message[50];
    recv(client_socket, received_message, sizeof(received_message), 0);
    std::cout << "Server: Encrypted message received from client: " << received_message << std::endl;

    decryptRecievedMessage(received_message, shared_secret);
    std::cout << "Server: Decrypted message from the client: " << received_message << std::endl;

    char message_to_send[] = "Ahoj ja som server!";
    std::cout << "Server: Sending a message to the client: " << message_to_send << std::endl;

    encryptSendedMessage(message_to_send, shared_secret);
    send(client_socket, message_to_send, sizeof(message_to_send), 0);

    close(client_socket);
    close(server_socket);

    return 0;
}