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
    long long private_key = 0 + std::rand() % (modulus - 0);

    long long public_key = computing(base, private_key, modulus);
    
    std::cout << "Client: Public key = " << public_key << std::endl;
    std::cout << "Client: Private key = " << private_key << std::endl;
    
    return public_key;
}

void encryptMessage(char* message, long long shared_secret) {
    for (int i = 0; i < strlen(message); ++i) {
        message[i] = message[i] ^ shared_secret;
    }
}

void decryptMessage(char* message, long long shared_secret) {
    for (int i = 0; i < strlen(message); ++i) {
        message[i] = message[i] ^ shared_secret;
    }
}

int main() {
    const long long modulus = MODULOS;
    const long long base = BASE;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Fail to create socket!" << std::endl;
        return -1;
    }
    
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Failed to connect to server!" << std::endl;
        close(client_socket);
        return -1;
    }

    std::cout << "Connection between Server and client is successful." << std::endl;

    long long client_public_key = generatingKeysDiffieHellman(base, modulus);

    send(client_socket, &client_public_key, sizeof(client_public_key), 0);

    long long server_public_key;
    std::cout << "Client: Waiting for the public key from the client..." << std::endl;
    recv(client_socket, &server_public_key, sizeof(server_public_key), 0);

    long long shared_secret = computing(server_public_key, client_public_key, modulus);

    char message_to_send[] = "Ahoj David";
    std::cout << "Client: Sending an encrypted message to the server: " << message_to_send << std::endl;

    encryptMessage(message_to_send, shared_secret);
    send(client_socket, message_to_send, sizeof(message_to_send), 0);

    char received_message[50];
    recv(client_socket, received_message, sizeof(received_message), 0);
    std::cout << "Client: Encrypted message received from server: " << received_message << std::endl;

    decryptMessage(received_message, shared_secret);
    std::cout << "Client: Decrypted message from the server: " << received_message << std::endl;

    close(client_socket);

    return 0;
}