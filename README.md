# Bico LKM - Backdoor e Rootkit com persistencia e ocultação

## Descrição

O **Bico LKM** é um módulo de kernel do Linux (LKM - Loadable Kernel Module) criado como um exercício didático para demonstrar técnicas de escalonamento de privilégios, ocultação de processos e conexões de rede, e como backdoors podem ser implementados a partir do kernel.
Este programa não tem a intenção de ser utilizado em ambientes práticos, e sim servir como uma prova de conceito didática, é um código para quem quer entender as técnicas que um atacante pode usar se esconder e obter um alto controle de um sistema linux, apartir de funções do kernel. ou para quem quer começar seu próprio lkm

## Funcionalidades

### 1. **Hook do `kill` syscall**
O módulo intercepta a syscall `kill`, permitindo que qualquer processo envie um sinal para si mesmo que o conceda privilégios de `root` através do sinal personalizado `ROOT_SIGNAL`.

### 2. **Ocultação de Conexões TCP**
Ao modificar a função de exibição de conexões TCP do kernel (`tcp4_seq_show`), o módulo oculta conexões na porta `4444` para evitar que sejam visíveis em ferramentas como `netstat`.
Esse método é muito mais barulhento e mais antigo, Existe jeito novo de se fazer essa ocultação para ser mais stealth, isso dimunui a compatibilidade, prossegui dessa forma visto que esse programa tem a intenção de ser universal. 

### 3. **Backdoor via Bind Shell**
O código `bico_bind.c` implementa um servidor que escuta na porta `4444`. Quando a senha correta ("bicopass") é fornecida, um shell com privilégios de root é executado.

### 4. **Serviço de Persistência**
O módulo pode ser configurado para reiniciar automaticamente como um serviço após a reinicialização do sistema através do arquivo `bico_bind.service`.

## Como Executar

### Pré-requisitos

- Um sistema GNU/Linux com permissões de administrador (root).
- `GCC` , `make` para compilar o módulo e os arquivos relacionados.
### Passos

1. **Clone o Repositório**

   ```bash
   git clone https://github.com/b3c001/bico_lkm.git
   cd bico_lkm

2. **Compilar o Módulo**

No diretório do projeto, compile o módulo com o seguinte comando:

```bash
make
```

3. **Carregar o Módulo**

Carregue o módulo para o kernel com o comando insmod:

```bash
sudo insmod bico.ko
```

4. **Verificar conexão***

O backdoor estará ativo e escutando na porta 4444.

Você pode tentar se conectar ao servidor e fornecer a senha "bicopass" para obter um shell com privilégios de root.

Utilize netstat para observar se a porta 4444 está oculta.

---

### Como Testar o Backdoor
Compile o arquivo bico_bind.c para criar o servidor de backdoor. Execute o código e tente se conectar a ele com a senha definida. Caso tenha sucesso, um shell com privilégios de root será aberto.

```bash
gcc bico_bind.c -o bico_bind
./bico_bind
```
---
# Termo de Isenção de Responsabilidade
Atenção: Este projeto foi criado **somente para fins educacionais** e **não deve ser utilizado em sistemas de produção ou em sistemas sem permissão expressa.** Seu objetivo é demonstrar técnicas de ataque em sistemas Linux para fins de pesquisa e aprendizado.

Não nos responsabilizamos por qualquer dano, perda de dados ou comprometimento de sistemas decorrentes do uso indevido deste código. O autor do código (b3c001) e os colaboradores deste repositório não são responsáveis por qualquer consequência legal ou técnica resultante da execução deste código fora de um ambiente controlado e autorizado.

# Licença
Este projeto é licenciado sob a Licença GPL-3.0. Para mais informações, consulte o arquivo [LICENSE](LICENSE).

# Contribuições
Contribuições para aprimorar a documentação ou corrigir bugs são bem-vindas. Para contribuições, siga as diretrizes de contribuição do repositório.
