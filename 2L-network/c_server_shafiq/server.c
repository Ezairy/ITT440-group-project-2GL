#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 5002
#define DB_HOST "db_container"
#define DB_USER "root"
#define DB_PASS "2GL@345"
#define DB_NAME "project_db"
#define USER_NAME "c_server_user1"

// Thread latar belakang untuk kemas kini mata (points) setiap 30 saat [cite: 41]
void *update_points_loop(void *arg) {
    while (1) {
        MYSQL *conn = mysql_init(NULL);
        if (conn == NULL) {
            fprintf(stderr, "[Cron Error] mysql_init() failed\n");
            sleep(30);
            continue;
        }

        if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
            fprintf(stderr, "[Cron Error] mysql_real_connect() failed: %s\n", mysql_error(conn));
            mysql_close(conn);
            sleep(30);
            continue;
        }

        char query[256];
        snprintf(query, sizeof(query), "UPDATE user_points SET points = points + 1 WHERE user = '%s'", USER_NAME);

        if (mysql_query(conn, query)) {
            fprintf(stderr, "[Cron Error] UPDATE failed: %s\n", mysql_error(conn));
        } else {
            printf("[Cron] Points updated for %s\n", USER_NAME);
        }

        mysql_close(conn);
        sleep(30); // Ulang setiap 30 saat [cite: 41]
    }
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Jalankan thread latar belakang untuk kemas kini pangkalan data
    pthread_t cron_thread;
    if (pthread_create(&cron_thread, NULL, update_points_loop, NULL) != 0) {
        perror("Failed to create cron thread");
        exit(EXIT_FAILURE);
    }

    // Membuka soket TCP [cite: 38]
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[Server] C Socket Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 1024);

        if (strncmp(buffer, "GET_LATEST", 10) == 0) {
            // Mengakses pangkalan data sebelum menghantar data kepada klien 
            MYSQL *conn = mysql_init(NULL);
            char response[1024] = {0};

            if (conn && mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
                char query[256];
                snprintf(query, sizeof(query), "SELECT user, points, datetime_stamp FROM user_points WHERE user = '%s'", USER_NAME);

                if (mysql_query(conn, query) == 0) {
                    MYSQL_RES *result = mysql_store_result(conn);
                    MYSQL_ROW row = mysql_fetch_row(result);

                    if (row) {
                        snprintf(response, sizeof(response), "User: %s | Points: %s | Last Update: %s", row[0], row[1], row[2]);
                    } else {
                        strcpy(response, "User data not found.");
                    }
                    mysql_free_result(result);
                } else {
                    snprintf(response, sizeof(response), "Database query error.");
                }
                mysql_close(conn);
            } else {
                strcpy(response, "Database connection failed.");
            }

            send(new_socket, response, strlen(response), 0);
        }
        close(new_socket);
    }

    return 0;
}