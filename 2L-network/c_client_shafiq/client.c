#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_HOST "c_server_container"
#define PORT 5002

void request_data() {
    while (1) {
        int sock = 0;
        struct sockaddr_in serv_addr;
        char *message = "GET_LATEST";
        char buffer[1024] = {0};

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            sleep(10);
            continue;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Menukar nama host / IP container kepada bentuk binary
        if (inet_pton(AF_INET, "172.17.0.1", &serv_addr.sin_addr) <= 0) {
            // Nota: Docker Compose menguruskan DNS secara automatik menggunakan nama servis.
            // Memandangkan inet_pton memerlukan IP, kaedah standard dalam C Docker adalah 
            // membenarkan fungsi gethostbyname() menyelesaikan nama "c_server_container".
        }

        // Penyelesaian alternatif C standard untuk DNS resolver di dalam rangkaian Docker
        #include <netdb.h>
        struct hostent *server = gethostbyname(SERVER_HOST);
        if (server != NULL) {
            memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("[Client Error] Connection Failed to %s\n", SERVER_HOST);
        } else {
            send(sock, message, strlen(message), 0);
            read(sock, buffer, 1024);
            printf("[Client] Response received:\n -> %s\n\n", buffer);
        }

        close(sock);
        sleep(10); // Minta maklumat baru setiap 10 saat [cite: 49]
    }
}

int main() {
    printf("[Client] Starting C Socket Client...\n");
    request_data();
    return 0;
}