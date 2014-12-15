//*****************************************************************
//            Projeto Integrador FTEC 2011/2
//    
//    componentes: Sidney dos Santos Souza, Constantino Lopes
//
//*****************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>

//*****************************************************************
//  Funções de portabilidade
//*****************************************************************

#ifdef _WIN32
#include <windows.h>
void cls(void){ system("CLS");}
void setColor(char* str){ system(str); }
#else
void cls(void){printf("\e[H\e[2J");}
void setColor(char*){};
#endif


//*****************************************************************
// Constantes Globais 
//*****************************************************************

#define ESQUERDA 1
#define CENTRO 2
#define DIREITA 3
#define QTD_Vei 5
#define QTD_Vaga 26


//*****************************************************************
// Variáveis Usadas para controle de pesquisa
//*****************************************************************

enum{
    TUDO,
    CLIENTES,
    VEICULOS,
};

//*****************************************************************
// Estruturas do Sistema
//*****************************************************************

typedef struct{
    char modelo[50];
    char placa[15];
    int tipo;
    int excluido;
}Veiculo;

typedef struct{    
    char nome[80];
    char endereco[80];
    char telefone[15];
    char cpf[14];
    int codigo;
    int excluido;    
    int numVagas;
    int contVei;
    int mesPago;
    int diaPago;
    Veiculo veiculo[QTD_Vei];
}Cliente;

typedef struct{
    char tipoCliente[20];
    char placa[15];
    int status;
    int entrada;
    int tipo;   
}Vaga;

typedef struct{
    int wdia_abertura;
    int mdia_abertura;
    int mes_abertura;
    int contDays;
    float fatDiario;
    float fatSemanal;
    float fatMensal;
}Caixa;

typedef struct{
    char nome[80];
    char Senha[20]; 
    Caixa caixa;
    float pernoite;
    float diaria;
    float mensalista;
    float meiaHora;
    float vHora;
    float vMoto;
    int firstStart;
    int contVagas;
    //int contVei; 
    int vagasMotos;
    int vagasMensalistas;
    Vaga vagas[QTD_Vaga];
}Sistema;

//*****************************************************************
// Variaveis Globais 
//*****************************************************************

//Variáveis responsáveis pelo controle do tempo //
struct tm* local; //Declaração da variavel 'local' que recebera um ponteiro do tipo 'tm'
time_t t;         // Variavel do tipo time

// Fim das variáveis de configuração do tempo //

/**
 * Tentei usar a struct tm, mas como ela dificultou os calculos de acumulo de horas, acabei optando por usar 
 * variáveis do tipo time_t salvas como inteiro, pois desse modo eu teria a diferença entre a entrada e saida
 * dada em segundos, possibilitando uma fácil conversão para minutos e horas.
 */

FILE* arqC;             // Ponteiro do arquivo de clientes
FILE* arqS;             // Ponteiro do arquivo do sistema
int retorno;            // Variavel usada para controle da gravação dos arquivos, caso ela seja nula é por que a gravação falhou
Sistema sistema;        // Variavel do Sistema, usada para guardar as informações utilizadas pelo mesmo
Vaga* sistema_veiculo;  // Ponteiro do tipo vaga, usado para acessar as vagas com o nome sistema_veiculo
Cliente cliente;        // Variavel de clientes, usada para armazenar os clientes mensalista.
time_t entrada, saida;  // Variaveis de controle de tempo

//*****************************************************************
// Comandos usados para controle de tela.
//*****************************************************************

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); 
CONSOLE_SCREEN_BUFFER_INFO SBInfo; // Pega informações da tela do console

int screen_x; //Valor da largura da tela
int screen_y; //Valor da altura da tela



//*****************************************************************
//  Inicio Protótipos das funções    
//*****************************************************************


//     A prototipação das funções são comumente usadas para que se possa escrever a sua implementação abaixo da função principal(main).
//Como a linguagem C é estruturada (lê o programa linha a linha) ele precisa reconhecer as funções usadas no main para então usa-las.
//A prototipação faz com que o compilador reconheça a função, compile-a e depois use-a posteriormente no main, mesmo que essas funções
//sejam escritas depois da função principal.


void gotoxy(int, int);               // Função usada para deslocar o ponteiro do console, recebe por parametro a COLUNA e LINHA
void alinhar(int,int, char*);        // Função usada para alinhar o texto na tela, recebe por parametro a LINHA a ser impressa na tela, POSIÇÃO (ESQUERDA, DIREITA, CENTRO), FRASE A SER ESCRITA.
void printCabecalho(void);           // Função usada para escrever o cabeçalho com o nome do estabelecimento
void clearLine(void);                // Função usada para limpar caracteres inscritos em uma linha na tela
void formDados(void);                // Função responsavel pelo formulario do cadastro de clientes
void coletaDados(void);              // Função responsavel pela coleta de dados do formulario acima
void CarIn(void);                    // Função usada para realizar a entrada do carro no estacionamento.
int CarOut(void);                    // Função usada para realizar a saida do carro do estacionamento
void Relatorios(void);               // Função responsavel por gerar relatórios de caixa.
void System_Conf(void);              // Função usada para fazer as configurações do sistema.
void cadastraCliente(void);          // Função para Cadastro de clientes mensalistas
void listaClientes(void);            // Função usada para listar todos os clientes cadastrados no sistema
int pesquisaGeral(int);              // Função que realiza as pesquisa de clientes e carros
void excluiCliente(int);             // Função usada para excluir clientes
void pagamentoCliente(int);          // Função usada para efetuar o pagamento do cliente
void cadastraCarro(int, int);        // Função usada para cadastrar carros no sistema
void editarRegistroCliente(int);     // Função para editar registros dos clientes
void editarRegistroCarros(int);      // Função para editar registro dos Carros
void getInformationClients(void);    // Função para Carrega informações dos clientes no sistema
void getInformationSystem(void);     // Função para Carregas informações do proprio sistema
void FirstStart(void);               // Função que testa se é a primeira inicialização do sistema
void listaVagas(void);               // Função que Lista todas as Vagas.

//*****************************************************************
// Funções para abrir arquivos leitura
//*****************************************************************
void abreArquivos(char*);
void abreArquivoCliente(char*);
void abreArquivoSistema(char*);

//*****************************************************************
// Funções para Fechar arquivos
//*****************************************************************
void fechaArquivos(int);
void fechaArquivoCliente(int); // Fecha arquivo com posição definida //
void fechaArquivoSistema();

// Fim das funções de manipulação dos arquivos//
//*************************************************************************
void relatorios(void);
void fecharAplicacao(void);



//*****************************************************************
// Função principal, local onde tudo acontece de fato.
//*****************************************************************
int main(){
    GetConsoleScreenBufferInfo(hOut,&SBInfo); // Pegas as configuraçãoes de tamanho e largura da tela do console    
    screen_x = SBInfo.srWindow.Right; 
    screen_y = SBInfo.srWindow.Bottom;
    
    sistema_veiculo = sistema.vagas;
    
    t = time(NULL);   // Zera as informações da variavel 't'
    local=localtime(&t); // atribui a 'local' o endereço de 't', posibilitando o uso da struct tm
        
    setColor("COLOR 3F"); //Função usada para setar a cor da letra e do fundo do console
    char resp;            // Var que recebe a resposta do user para perguntas to tipo [S/n]
    int op, op2, respInt; // 'Op' usada para saber qual operação executar. 'respInt', usada em perguntas de resultado inteiro [1, 2, 4, 0]
    int posicao;          // Usada para retorno das posições  de veiculo e cliente .
    
    getInformationClients(); // Função que carrega informações dos clientes e carros
    getInformationSystem();  // Função que carrega as informações do sistema
    
    if(sistema.firstStart == 0){
        FirstStart();
    }
    
    do{       
        cls();
        printCabecalho();
        alinhar(4, CENTRO,"Seu carro seguro e vc despreocupado!!");
        alinhar(10, CENTRO,"Entrada Veiculo..................[1]");
        alinhar(11, CENTRO,"Saida Veiculo....................[2]");
        alinhar(12, CENTRO,"Opcoes Mensalistas...............[3]");
        alinhar(13, CENTRO,"Listar Vagas ....................[4]");
        alinhar(14, CENTRO,"Relatorios.......................[5]");
        alinhar(15, CENTRO,"Configuracao do Sistema..........[6]");
        alinhar(16, CENTRO,"Sair.............................[0]");
        alinhar(19, CENTRO,"Escolha uma opcao:                  ");
        gotoxy((screen_x/2)+1,19); scanf("%d", &op); fflush(stdin);
        
        switch (op){
             case 1:
                    do{
                        cls();
                        printCabecalho();
                        alinhar(4, CENTRO, "Entrada Veiculo");
                        CarIn();
                        gotoxy(3,23); clearLine();
                        gotoxy(3,23); printf("Continuar [S/n]: ");
                        resp = getchar(); fflush(stdin);
                    }while(resp == 'S' || resp == 's');
                break;
             case 2:
                    do{
                        cls();
                        printCabecalho();
                        alinhar(4, CENTRO, "Saida Veiculo");
                        if(CarOut() != 1){
                            gotoxy(3,21); printf("Veiculo nao encontrado");
                        }
                        gotoxy(3,23); clearLine();
                        gotoxy(3,23); printf("Continuar [S/n]: ");
                        resp = getchar(); fflush(stdin);
                    }while(resp == 'S' || resp == 's');
                break;
             case 3:
                    do{
                        cls();
                        printCabecalho();
                        alinhar(4, CENTRO,"Opcoes Mensalistas");
                        alinhar(10, CENTRO,"Cadastrar Clientes...............[1]");
                        alinhar(11, CENTRO,"Listar Clientes..................[2]");
                        alinhar(12, CENTRO,"Pesquisa/Alterar Cliente.........[3]");
                        alinhar(13, CENTRO,"Excluir Clientes.................[4]");
                        alinhar(14, CENTRO,"Pagamento........................[5]");
                        alinhar(15, CENTRO,"Voltar ao menu anterior..........[0]");
                        alinhar(19, CENTRO,"Escolha uma opcao:                  ");
                        
                        gotoxy((screen_x/2)+1,19); scanf("%d", &op2); fflush(stdin);
                        switch (op2){
                            case 1:
                                    do{
                                        cls();
                                        printCabecalho();
                                        alinhar(4, CENTRO, "Cadastro de Cliente");
                                        cadastraCliente();
                                        fflush(stdin);
                                        gotoxy(3,23);clearLine();
                                        gotoxy(3,23); printf("Deseja cadastrar outro Cliente? [S/n]: ");
                                        resp = getchar();
                                        
                                    }while(resp == 'S' || resp == 's');
                                break;
                            case 2:
                                    cls();
                                    printCabecalho();
                                    alinhar(4, CENTRO, "Listagem de Cliente\n\n\n");
                                    listaClientes();
                                break;
                            case 3:
                                    cls();
                                    printCabecalho();
                                    alinhar(4, CENTRO, "Pesquisa/Altera Cliente");
                                    posicao = pesquisaGeral(TUDO);
                                    if(posicao >= 0){
                                        printf("\n\n   Digite [1]Editar Cliente, [2]Editar/Adicionar Veiculo, [0]Voltar: ");
                                        scanf("%d", &respInt);fflush(stdin);
                                        
                                        switch(respInt){
                                                    
                                            case 1:
                                                    cls();
                                                    printCabecalho();
                                                    alinhar(4, CENTRO, "Pesquisa/Altera Cliente");
                                                    editarRegistroCliente(posicao);
                                                break;
                                            
                                           case 2:
                                                    do{    
                                                        cls();
                                                        printCabecalho();
                                                        alinhar(4, CENTRO, "Pesquisa/Altera Cliente");                                                    
                                                        editarRegistroCarros(posicao);                                                    
                                                        gotoxy(3,23); printf("Deseja alterar outro veiculo? [S/n]: ");
                                                        resp = getchar(); fflush(stdin);
                                                        gotoxy(3,23); clearLine();                                                    
                                                     }while (resp == 'S' || resp == 's');
                                                break;
                                        } 
                                    }
                                break;
                            case 4:
                                    cls();
                                    printCabecalho();
                                    alinhar(4, CENTRO, "Exclui Cliente");
                                    posicao = pesquisaGeral(CLIENTES);
                                    getch();
                                    excluiCliente(posicao);
                                break;
                            case 5:
                                    cls();
                                    printCabecalho();
                                    alinhar(4, CENTRO, "Pagamentos");
                                    posicao = pesquisaGeral(CLIENTES);
                                    getch();
                                    pagamentoCliente(posicao);
                                
                                break;
                        }
                    }while(op2 != 0);
                    
                break;
             case 4:
                    cls();
                    printCabecalho();
                    alinhar(4, CENTRO, "Listar Vagas");
                    listaVagas();
                break;
             case 5:
                    cls();
                    printCabecalho();
                    alinhar(4, CENTRO, "Relatorios");
                    relatorios();
                    getch();
                break;
             case 6:
                    cls();
                    printCabecalho();
                    alinhar(4, CENTRO, "Configuracao do Sistema");
                    System_Conf();                
                break;
             case 0:
                    fecharAplicacao();
                    op = -1;
                break;
        }
    }while(op != -1);
    return 0;
}

//*****************************************************************
//  Funções adicionais para ações diversas
//*****************************************************************

void gotoxy(int x, int y){
     COORD coord;
     coord.X=(short)x;
     coord.Y=(short)y;
     SetConsoleCursorPosition(hOut,coord);
}

void alinhar(int LINHA, int modo, char *MENSAGEM){
    
    int COLUNA;
    if( modo == 1)
        COLUNA=0;
    
    if(modo == 2)
        COLUNA = ((screen_x - strlen(MENSAGEM))/2);
    
    if(modo == 3)
        COLUNA = screen_x - strlen(MENSAGEM);

    gotoxy(COLUNA,LINHA); printf(MENSAGEM);

}
void formDados(){
    gotoxy(3,7); printf("Codigo Cliente..........: %d", cliente.codigo);
    gotoxy(3,8); printf("Digite o Nome do Cliente: ");
    gotoxy(3,9); printf("Digite o CPF do Cliente.: ");
    gotoxy(3,10); printf("Digite o Endereco.......: ");
    gotoxy(3,11); printf("Digite o Telefone.......: ");
    gotoxy(3,12); printf("Digite o Numero de Vagas: ");
}

void coletaDados(){
    gotoxy(29,8); scanf("%[^\n]", cliente.nome);fflush(stdin);
    gotoxy(29,9); scanf("%s", cliente.cpf);fflush(stdin);
    gotoxy(29,10); scanf("%[^\n]", cliente.endereco);fflush(stdin);
    gotoxy(29,11); scanf("%[^\n]", cliente.telefone);fflush(stdin);
    gotoxy(29,12); scanf("%d", &cliente.numVagas);fflush(stdin);
}

//*****************************************************************
// Função usadas para carregar informações no sistema
//*****************************************************************

void getInformationClients(){ 
    if(!(arqC = fopen("clientes.bin", "rb"))){
        if(!(arqC = fopen("clientes.bin", "w+"))){
            printf("Erro ao abrir arquivo de clientes....\n");
            getch();
            return;
        }
        fclose(arqC);
        return;
    }
        
    rewind(arqC);    
    while(fread(&cliente, sizeof(cliente), 1,arqC) == 1);
    fclose(arqC);
}

void getInformationSystem(){
    if(!(arqS = fopen("sistema.bin","rb"))){
        if(!(arqS = fopen("sistema.bin","w+"))){
            printf("Erro ao abrir arquivo do sistema....\n");
            getch();
            return;

        }
        fclose(arqS);
        return;
    }
    
    rewind(arqS);
    fread(&sistema,sizeof(sistema),1,arqS);
    fclose(arqS);
}

//*****************************************************************
// Funções para abrir arquivos leitura
//*****************************************************************
void abreArquivos(char* modo_abertura){
    abreArquivoCliente(modo_abertura);
    abreArquivoSistema(modo_abertura);
}

void abreArquivoCliente(char* modo_abertura){
    arqC = fopen("clientes.bin", modo_abertura);
    if(!arqC){
        printf("\nErro ao abrir arquivo de clientes....\n");   
        getch();
        return;
    }
}

void abreArquivoSistema(char* modo_abertura){
    arqS = fopen("sistema.bin", modo_abertura);
    if(!arqS){
        printf("\nErro ao abrir arquivo do sistema....\n");
        getch();
        return;
    }
}

//*****************************************************************
// Funções para Fechar arquivos
//*****************************************************************
void fechaArquivos(int posicao){
    fechaArquivoCliente(posicao);
    fechaArquivoSistema();
}

void fechaArquivoCliente(int posicao){
    rewind(arqC);
    fseek(arqC, (sizeof(cliente)*posicao), SEEK_CUR);
    retorno = fwrite(&cliente, sizeof(cliente), 1,arqC); // escreve no binario a struct q tiver na memoria 
    
    if(!retorno){  // trata o retorno do comando de escrita, se ele retornar null 
        cls();
        printf("\nErro ao gravar arquivo se clientes....\n");      // imprime um erro de gravação do arquivo 
        getch();
    }
    fclose(arqC);
}

void fechaArquivoSistema(){
    rewind(arqS);
    retorno = fwrite(&sistema, sizeof(sistema), 1,arqS);
    
    if(!retorno){
        cls();
        printf("\nErro ao gravar arquivo do sistema....\n");
        getch();
    }

    fclose(arqS);
}

//*****************************************************************
// Função usada para mostrar mensagem na primeira inicialização
//*****************************************************************

void FirstStart(){
    
    cls();
    alinhar(5,CENTRO,"******************************************");
    alinhar(6,CENTRO,"*                                        *");
    alinhar(7,CENTRO,"*    SEJA BEM VINDO, ESPERAMOS QUE GOSTE *");
    alinhar(8,CENTRO,"* DO NOSSO PRODUTO.                      *");
    alinhar(9,CENTRO,"*    PEDIMOS QUE CONFIGURE-O CONFORME A  *");
    alinhar(10,CENTRO,"* SUA NECESSIDADE.                       *");
    alinhar(11,CENTRO,"*    PARA TAL, ENTRE NO MENU RERENTE AS  *");
    alinhar(12,CENTRO,"* CONFIGURACOES DO SISTEMA E FACA AS DE- *");
    alinhar(13,CENTRO,"* VIDAS ALTERACOES.                      *");
    alinhar(14,CENTRO,"*                                        *");
    alinhar(15,CENTRO,"******************************************");
    
    gotoxy(3,23); printf("Pressione <enter> para continuar");
    getch();

}

//*****************************************************************
//   Funções Utilizadas para Estruturação dos Menus
//*****************************************************************

void printCabecalho(){
    int i;
    
    for(i = 0; i <= screen_x; i++)
        printf("*");
 
    alinhar(2, CENTRO, sistema.nome);
}

void clearLine(){
    puts("                                                                            ");
}

//*****************************************************************
// Funções para controle de entrada dos carros
//*****************************************************************

void CarIn(){
    char timeStr [9]; // String que recebe o time
    char placa[10] = ""; // Placa Temporaria
    char resp = {0};
    int i = 0; // Variavel de controle do for
    int j = 0;  // Variavel de inicialização do i, usada para controle das vagas: caso seja moto vagas [i - j], case ja carro vagas [j- QTD_Vaga];
    int parada = 0; // Recebera a o valor de parada do for//
    int tempTipoV = 0; // Tipo de Veiculo Temporario
    int indiceVei = 0; // Utilizada para acessar as posições ao vetor de veiculos//
    int achou = 0; // Flag para controle do cliente, se ele for mensalista ela recebe o valor 1
    int respInt; // Resposta para perguntas que retornam um inteiro como resultado
    int posicao =-1; // Foi iniciado com -1 pois ao incrementa-la eu terei o resultado = || cliente posição 1, 
                    //deve iniciar a escrita na posição ZERO para que seja feita de maneira correta;
                    
    abreArquivos("rb+");
    
    gotoxy(3,10); printf("Digite a placa do veiculo:");
    gotoxy(3,11); printf("Cod veiculo..............:");
    gotoxy(3,12); printf("Modelo veiculo...........:");
    gotoxy(3,13); printf("Tipo do veiculo..........:");
    gotoxy(3,14); printf("Hora de entrada..........:");
    gotoxy(3,15); printf("Tipo do Cliente..........:");
    gotoxy(30,14); printf("%s",_strtime(timeStr)); //printa o horario na tela usando a string
    
    do{
        gotoxy(30,10); scanf("%s", placa); fflush(stdin);
         if((strlen(placa) != 7) && (strlen(placa) != 8)){
            gotoxy(3,21); printf("Placa incorreta, tente novamente...");
            gotoxy(30,10); printf("         ");
        }
    }while((strlen(placa) != 7) && (strlen(placa) != 8));
    
    gotoxy(0,21);clearLine();
    
    rewind(arqS);
    fread(&sistema, sizeof(sistema), 1,arqS);
    
    //Vare o arquivo de clientes em procura do carro a ser estacionado
    rewind(arqC);    
    while(fread(&cliente, sizeof(cliente), 1,arqC) == 1){
        if (cliente.excluido == 0){
            for (i=1;i <= cliente.contVei;i++){
                if (cliente.veiculo[i].excluido == 0){
                   if(strcmp(placa, cliente.veiculo[i].placa)==0){
                        gotoxy(30,11); printf("%d", i); //Codigo
                        gotoxy(30,12); printf("%s", cliente.veiculo[i].modelo); //Modelo
                        achou=1; // Flag para saber se o cliente é mensalista e/ou o carro esta cadastrado
                        indiceVei=i;
                        break;
                    }
                }
            }
        }
        posicao++; // Posicao do cliente no arquivo
    }
    
    if(achou != 1){  // Se o carro não foi encontrado é por que não esta cadastrado 
        gotoxy(3,21); printf("Veiculo nao cadastrado");
    }
    
    if(achou == 1){ //Se o cliente for mensalista executa os codigos abaixo
        gotoxy(30,15); printf("Mensalista");
        gotoxy(30,13); printf((cliente.veiculo[indiceVei].tipo == 1)? "Carro":"Moto");
        
        if((cliente.diaPago  < local->tm_mday)&&(((cliente.mesPago == 12)? 0:cliente.mesPago) < (local->tm_mon+1))){
            gotoxy(3,21); clearLine();
            gotoxy(3,21); printf("Cliente com pagamento em atraso...");
            return;
        }

    }
    else{ // Se o cliente não for mensalista executa os comandos abaixo afim de coletar as informações necessarias para estacionar
        gotoxy(3,13); printf("Tipo? [1]Carro [2]Moto...:"); 
        do {
            gotoxy(30,13); scanf("%d", &tempTipoV); fflush(stdin);
            if ((tempTipoV != 1) && (tempTipoV != 2)){ // Seta o tipo do veiculo
                gotoxy(30,13); printf("      ");
            }
        }while((tempTipoV != 1)&&(tempTipoV != 2));
        gotoxy(30,13); printf((tempTipoV == 1)? "Carro":"Moto");
        
        do{
            gotoxy(3,22); printf("Qual o tipo do Cliente? [1]Diaria,[2]Pernoite,[3]Rotativo: "); // Seta o tipo de cliente
            scanf("%d", &respInt);fflush(stdin);
            if ((respInt != 1) && (respInt != 2) && (respInt != 3)){
                gotoxy(61,22); printf("      ");
            }
        }while((respInt != 1) && (respInt != 2) && (respInt != 3));
        gotoxy(0,22); clearLine();
        
        switch(respInt){
            case 1:
                    gotoxy(30,15); printf("Diaria");
                break;
            case 2:
                    gotoxy(30,15); printf("Pernoite");
                break;
            case 3:
                    gotoxy(30,15); printf("Rotativo");
                break;
                
        }
    }

	for (i= 1; i < QTD_Vaga;i++){        
		if(strcmp(placa,sistema.vagas[i].placa)==0){ // Se o veiculo ja estiver estacionado, coisa q é impossivel de acontecer, mostra a mensagem abaixo, para controlar erros de digitação
    		gotoxy(3,21); printf("Veiculo ja se encontra estacionado na Vaga %d", i);
    		return; // Sai da função caso cheque até aqui
    	}
    }	


//        Trecho usado para controlar a vaga na qual o veiculo sera estacionado.
//    caso ele seja uma moto recebera as vagas [ 1 até vagasMotos ], caso seja carro não cadastrado recebera as vagas de [ (vagasMotos+1) até (Vagas - vagasMensalistas)]
//    caso seja mensalista recebera as vagas de [ (vagas - mensalistas + 1) até (vagas)]    

    if(tempTipoV == 2){
         j = 1;
         parada = sistema.vagasMotos;
    }
    else{
        j = sistema.vagasMotos+1;
        parada = (QTD_Vaga-1) - sistema.vagasMensalistas;
    }
    
    if (achou == 1){
        j = (QTD_Vaga - sistema.vagasMensalistas);
        parada = QTD_Vaga-1;
    }
    
    //Varre as vagas para saber quais estão ocupadas, quais veiculos estão nas vagas e quais as vagas livres

    for (i=j;i <= parada;i++){        
        if(sistema.vagas[i].status == 0){  // Testa se a vaga esta ou não livre, se estiver mostra disponibiliza para estacionamento
            gotoxy(3,16); printf("Vaga Destino.............: %d", i);
            
            // Comandos para mensagens de aviso 
            if((i == sistema.vagasMotos)&&(achou == 0)&&(tempTipoV == 2)){ // caso não hava vaga de motos
                gotoxy(3,21); printf("Ultima vaga para motos");
            }
            else if((i == parada)&&(achou == 0)&&(sistema.vagas[i].status == 0)){
                gotoxy(3,21); printf("Ultima vaga para Carros"); // caso não haja vaga de carros
            }
            
            gotoxy(3,23); printf("Confirmar entrada [S/n]: "); // confirma a entrada do veiculo no estacionamento
            resp = getchar(); fflush(stdin);
            if(resp == 'S' || resp == 's'){
                sistema.contVagas++; //Incrementa a quantidade de vagas ocupadas no estacionamento
                if(achou == 1){ // Se for mensalista executa os comandos abaixo
                    time(&entrada); // Pega a data de entrada do veiculo, baseado na hora do sistema operacional 
                    sistema.vagas[i].entrada = entrada; // Seta hora de entrada na estrutura
                    sistema.vagas[i].status = 1; // altera o status da vaga para ocupada
                    strcpy(sistema.vagas[i].tipoCliente,"Mensalista"); // Seta o tipo de cliente na vaga para mensalista
                    strcpy(sistema.vagas[i].placa,placa); // Seta a placa do carro na vaga
                    
                    if(cliente.numVagas > 0){ // Se a quantidade de vaga for maior que Zero, ele decrementa a quantidade disponivel para o cliente
                        cliente.numVagas--;
                    }
                    else{ // Se a quantidade de vagas for zero, diz q ele excedeu o numero de vagas
                        gotoxy(0,21);clearLine();
                        gotoxy(3,21); printf("Cliente excedeu o numero de vagas");
                        return;
                    }
                }
                else{ // Se o cliente não for mensalista executa os comandos abaixo
                    time(&entrada);
                    sistema_veiculo[i].tipo = tempTipoV;
                    sistema_veiculo[i].entrada = entrada;
                    sistema.vagas[i].status=1;
                    strcpy(sistema.vagas[i].placa,placa);

                    switch(respInt){
                    case 1:
                            strcpy(sistema.vagas[i].tipoCliente,"Diaria");
                        break;
                    case 2:
                            strcpy(sistema.vagas[i].tipoCliente,"Pernoite");
                        break;
                    case 3:
                            strcpy(sistema.vagas[i].tipoCliente,"Rotativo");
                        break;
                        
                    }
                }
            }            
            break;            
        }
    }
    
    // Caso a quantidade de vagas p/ motos e de mensalistas seja = ele não entrara for, passando direto sem dar nenhuma mensagem
    // os comandos abaixo são apenas uma mensagem de aviso;
    
    // Trecho ainda em faze de testes        
    // Se não ouver vagas livres mostra uma das mensagens abaixo 
        
    if((i >= sistema.vagasMotos)&&(sistema_veiculo[i].tipo == 2)){ // caso não hava vaga de motos
        gotoxy(3,21); printf("Nao temos mais vagas para motos");
    }
    else if(((j > parada) || (i > parada)) &&(achou == 0)&&(sistema.vagas[i].status == 0)){
        gotoxy(3,21); printf("Nao temos mais vagas para Carros"); // caso não haja vaga de carros
    }

    if(sistema.contVagas > QTD_Vaga-1){ // Testa se ainda há vagas disponiveis no estacionamento
        gotoxy(0,21);clearLine();
        gotoxy(3,21); printf("Nao temos mais vagas disponiveis");
        return; // Caso não tenha vagas, sai da função sem gravar nada.
    }       
    
    if(achou == 1){ // Se o cliente foi encontrado, salva as alterações no arquivo de clientes e fecha-o.
        fechaArquivoCliente(posicao);
    }
    else{ // Se o cliente não for encontrado, simplesmente fecha o arquivo de clientes.
        fclose(arqC);        
    }
    
    fechaArquivoSistema();
}

int CarOut(){    
    char placa[10], resp;
    int i;
    int indiceVei = 0;
    int achou = 0;
    int respInt = 0;
    int posicao =-1;
    int hora_s = 0; // Recebe o total de horas que o veiculo ficou estacionado
    int min_s = 0; // Recebe o totqal de minutos que o veiculo ficou estacionado 
    int dif_Entrada_Saida = 0; // Diferença, em minutos, entre as horas de entrada e saida do veiculo da vaga;
    int encontrouCarro = 0; // Flag usada para controlar se o carro foi ou não encontrado nas vagas do estacionamento
    float pagar = 0;
    
    abreArquivos("rb+");
    
    gotoxy(3,10); printf("Digite a placa do Veiculo:");
    gotoxy(3,11); printf("Cod Veiculo..............:");
    gotoxy(3,12); printf("Modelo Veiculo...........:");
    gotoxy(3,13); printf("Tipo Cliente.............:");
    gotoxy(3,14); printf("Tempo permanencia........:");
    gotoxy(3,15); printf("Total a pagar............:");    
    
    do{
        gotoxy(30,10); scanf("%s", placa); fflush(stdin);
        if((strlen(placa) != 7) && (strlen(placa) != 8)){
            gotoxy(3,21); printf("Placa incorreta, tente novamente...");
            gotoxy(30,10); printf("         ");
        }
    }while((strlen(placa) != 7) && (strlen(placa) != 8));
    
    gotoxy(0,21);clearLine();
    
    rewind(arqS);
    fread(&sistema, sizeof(sistema), 1,arqS);
        
    rewind(arqC);    
    while(fread(&cliente, sizeof(cliente), 1,arqC) == 1){            
        if (cliente.excluido == 0){
            for (i=1;i <= cliente.contVei;i++){
                if (cliente.veiculo[i].excluido == 0){
                   if(strcmp(placa, cliente.veiculo[i].placa)==0){
                        achou=1;
                        indiceVei=i;
                        break;
                    }
                }
            }
        }
        posicao++;
    }
    
    for (i=1;i < QTD_Vaga;i++){// Varre o vetor de vagas a procura da placa digitada.
        if(strcmp(placa, sistema.vagas[i].placa)==0){ // Se encontrar, executa os comandos baixo            
            if(achou == 1){ // Se o cliente for mensalista 
                gotoxy(30,11); printf("%d", indiceVei); // Mostra o codigo do 'veiculo'
                gotoxy(30,12); printf("%s", cliente.veiculo[indiceVei].modelo); // mostra o modelo do veiculo
                gotoxy(30,13); printf("%s", sistema.vagas[i].tipoCliente);
                time(&saida); 
                dif_Entrada_Saida = saida - sistema.vagas[i].entrada; // Diferença entre a hora de entrada e a hora de saida
                hora_s = (dif_Entrada_Saida/3600); // Seta quantas horas o cliente deixou o veiculo estacionado
                min_s = ((dif_Entrada_Saida%3600)/60); // Seta quantos minutos o cliente deixou o veiculo estacionado
                
                gotoxy(30,14); printf("%d hora(as): %d minuto(os)", hora_s, min_s); 
            }
            else{// Se o cliente não for mensalista, é incrementado a variavel para acessar cada posição do vetor de veiculos do sistema
                
                indiceVei++;			
                gotoxy(30,13); printf("%s", sistema.vagas[i].tipoCliente);
                time(&saida);
                dif_Entrada_Saida = saida - sistema.vagas[i].entrada; 
                hora_s = (dif_Entrada_Saida/3600);
                min_s = ((dif_Entrada_Saida%3600)/60);
                gotoxy(30,14); printf("%d hora(as): %d minuto(os)", hora_s, min_s); 
                
            }

        	if((sistema_veiculo[i].tipo != 1) && (achou == 0)){ // Se o veiculo for uma moto, ele diz que o total a pagar é o preço da moto.
        		gotoxy(30,15); printf("%.2f reais", sistema.vMoto);
        		pagar = sistema.vMoto;
        	}
        	else{ // Se for um carro ele efetua os calculos afim de definir o valor a pagar
        	
        		if(achou == 1){// se for cliente mensalista o total é ZERO, pois ele pagara tudo no final do mes
        			gotoxy(30,15); printf("0 reais"); 
        		}
        		else{// Se não for mensalista, calcula o preço a ser pago        			        				
        			if(strcmp(sistema.vagas[i].tipoCliente, "Rotativo") == 0){                
        				pagar = sistema.vHora; // Caso o cliente seja rotativo, pagará no minimo o valor integral da hora
                        
                        //Teste usados para calcular o acrescimo caso o cliente execeda o tempo contratado                                                
                        if((min_s > 0)&&(min_s <= 30)&&(hora_s > 0)){
        					 pagar+= 1*sistema.meiaHora;
        					 min_s = 1;
                        }
        			    else if((min_s > 30)&&(min_s < 60)&&(hora_s > 0)){
        					 pagar+= 2*sistema.meiaHora;
        					 min_s = 2;
        			    }
        				
        				// Caso o cliente exceda 8 horas de estadia no estacionamento sera cobrado uma diaria do mesmo, caso ele exceda esse tempo sera cobrado ainda 1 real a cada meia hora        				
        				if((hora_s > 1) && (hora_s < 8)){
        					pagar = sistema.vHora+((hora_s - 1)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        				else if((hora_s >= 8 ) && (hora_s < 10)){
        					pagar = sistema.diaria+((hora_s-8)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        				else if(hora_s >= 10){
        					pagar = sistema.diaria+((hora_s-10)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        			}
        			
        			if(strcmp(sistema.vagas[i].tipoCliente, "Diaria") == 0){
        				pagar = sistema.diaria;
        				
                        if((min_s > 0)&&(min_s <= 30)&&(hora_s > 8)){
        					 pagar+= sistema.meiaHora;
        					 min_s = 1;
                        }
        			    else if((min_s > 30)&&(min_s < 60)&&(hora_s > 8)){
        					 pagar+= 2*sistema.meiaHora;
        					 min_s = 2;
        			    }
        				
        				if((hora_s >= 8 ) && (hora_s < 10)){
        					pagar = sistema.diaria+((hora_s-8)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        				else if (hora_s >= 10){
        					pagar = sistema.diaria+((hora_s-10)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        			}
        			
        			if(strcmp(sistema.vagas[i].tipoCliente, "Pernoite") == 0){
        				pagar = sistema.pernoite;
        				
        				if((min_s > 0)&&(min_s <= 30)&&(hora_s > 10)){
        					 pagar+= sistema.meiaHora;
        					 min_s = 1;
                        }
        			    else if((min_s > 30)&&(min_s < 60)&&(hora_s > 10)){
        					 pagar+= 2*sistema.meiaHora;
        					 min_s = 2;
        			    }
        			    
        				if(hora_s > 10){
        					pagar = sistema.pernoite+((hora_s-10)*2.0*sistema.meiaHora)+(sistema.meiaHora*min_s);
        				}
        			}
        			
        			gotoxy(30,15); printf("%.2f reais", pagar);                
        		}
        	}	
        	
            gotoxy(3,23); printf("Confirmar Saida [S/n]: ");
            resp = getchar(); fflush(stdin);
            if(resp == 'S' || resp == 's'){
                sistema.contVagas--; //Decrementa a quantidade de vagas ocupadas no estacionamento        
                if(achou == 1){
        			cliente.numVagas++;// Incrementa as vagas pois ele esta retirando um veiculo do estacionamento;
        		}
        		
                // Libera a vaga no estacionamento
                strcpy(sistema.vagas[i].placa,"");
                sistema.vagas[i].status = 0;
                strcpy(sistema.vagas[i].tipoCliente,"");
                
                //Soma o valor pago ao faturamento diario;
                sistema.caixa.fatDiario += pagar;
                
            }
            encontrouCarro = 1;
            break;
        }
    }
        
    if(achou == 1){
        fechaArquivoCliente(posicao);
    }
    else{
        fclose(arqC);        
    }
    
    fechaArquivoSistema();
    
    return encontrouCarro;
}

//*****************************************************************
// Funções do Cliente
//*****************************************************************
void cadastraCliente(){    
    int retorno; // Variavel usada para alocar o retorno da escrita no arquivo
    char resp; // variavel usada para capturar as respostas do usuario
    cliente.codigo++; // incrementa o codigo do cliente
            
    fflush(stdin); 
    abreArquivoCliente("a+");
    
    formDados(); // Formulario de cadastro do cliente
    coletaDados();
    
    gotoxy(3,23); printf("Deseja cadastrar um veiculo? [S/n]: ");
    resp = getchar();
            
    if (resp == 'S' || resp == 's'){
        cliente.contVei = 0; // Var usada para contar a quantidade de veiculos, começa em UM por que eu não qro ter um veiculo de codigo ZERO
        do {
            fflush(stdin);            
            cliente.contVei++; // incrementa codigo do veiculo
            cadastraCarro(cliente.contVei, 1);
            gotoxy(3,23); printf("Deseja cadastrar outro veiculo? [S/n]: ");
            resp = getchar();
         }while (resp == 'S' || resp == 's');
    }    
    
    if(cliente.codigo != 0)
    retorno = fwrite(&cliente, sizeof(cliente), 1,arqC); // escreve no binario a struct q tiver na memoria 
    
    if(!retorno){ // trata o retorno do comando de escrita, se ele retornar null
        printf("Erro ao gravar arquivo de clientes...."); // imprime um erro de gravação do arquivo 
        getch();
    }
    fclose(arqC);
}

void cadastraCarro(int indice, int num){
    if(num == 1)
    alinhar(14, CENTRO, "Cadastro de Veiculos");
    // Inicio Comandos para limpar a tela do cadastro de veiculos 
    gotoxy(3,16); clearLine();
    gotoxy(3,17); clearLine();
    gotoxy(3,18); clearLine();
    gotoxy(3,19); clearLine();
    gotoxy(3,23);clearLine();
    // Fim Comandos para limpar a tela do cadastro de veiculos 
    
    gotoxy(3,16); printf("Codigo do Veiculo..........: %d\n", indice);
    gotoxy(3,17); printf("Digite o Modelo do Veiculo.: ");
    gotoxy(3,18); printf("Tipo: [1]Carro, [2]Moto....: ");
    gotoxy(3,19); printf("Digite a Placa.............: ");
    gotoxy(32,17); scanf("%[^\n]",cliente.veiculo[indice].modelo); fflush(stdin);
    
    do {
        gotoxy(32,18); scanf("%d",&cliente.veiculo[indice].tipo); fflush(stdin);
        if ((cliente.veiculo[indice].tipo != 1) && (cliente.veiculo[indice].tipo != 2)){
            gotoxy(32,18); 
            printf("      ");
        }
    }while((cliente.veiculo[indice].tipo != 1)&&(cliente.veiculo[indice].tipo != 2));
    
    gotoxy(32,19); scanf("%[^\n]",cliente.veiculo[indice].placa);fflush(stdin);    
}
  
void listaClientes(){
    int i;
    int contClient=0; // Var usada para saber se o cliente a ser impresso é ou não o primeiro cliente. 
    
    abreArquivoCliente("rb");
    
    rewind(arqC);
    
    while(fread(&cliente, sizeof(cliente), 1,arqC) == 1){        
        if (cliente.excluido == 0){
            if (contClient > 0) // Testa se o cliente é o primeiro e se ele não for imprime duas quebras de linha (uso apenas estetico) 
               printf("\n\n");           
            
            printf("Codigo Cliente......: %d\n", cliente.codigo);
            printf("Nome do Cliente.....: %s\n", cliente.nome);
            printf("CPF do Cliente......: %s\n", cliente.cpf);
            printf("Endereco do Cliente.: %s\n", cliente.endereco);
            printf("Telefone do Cliente.: %s\n", cliente.telefone);
            printf("Numero de Vagas.....: %d\n", cliente.numVagas);
            printf("\nVeiculo(os)\n-----------------------");
            for (i=1;i <= cliente.contVei;i++){
                if (cliente.veiculo[i].excluido == 0){
                    printf("\n\nCod. Veiculo: %d\n",i);
                    printf("Modelo do carro: %s\n", cliente.veiculo[i].modelo);
                    printf("Placa: %s\n", cliente.veiculo[i].placa);
                    printf("Tipo: %s\n", (cliente.veiculo[i].tipo == 1) ? "Carro" : "Moto");
                }
            }
        }
        contClient++;
    }
    fclose(arqC);
    getch();    
}

int pesquisaGeral(int tipo){
    int codigo, i, posicao = 0;
    
    abreArquivoCliente("rb");
    
    if((tipo == TUDO) || (tipo == CLIENTES)){
        gotoxy(3,10); printf("Digite o codigo.....: ");    
        gotoxy(3,11); printf("Nome do Cliente.....: ");
        gotoxy(3,12); printf("CPF do Cliente......: ");
        gotoxy(3,13); printf("Endereco do Cliente.: ");
        gotoxy(3,14); printf("Telefone do Cliente.: ");
        gotoxy(3,15); printf("Status do Cliente...: ");
        gotoxy(25,10); scanf("%d", &codigo);fflush(stdin);
        
        rewind(arqC); // envia o ponteiro do arquivo para o seu inicio 
        posicao = codigo-1; // Defini a posição do cliente no arquivo
        fseek(arqC, (sizeof(cliente)*posicao), SEEK_CUR); // Desloca o ponteiro do arquivo X bytes até o primeiro byte da posição desejada
        fread(&cliente, sizeof(cliente), 1,arqC); // Lê a strutura do cliente
        
        if (cliente.codigo != codigo){ // Testa se o codigo do cliente é igual ou diferente do codigo digitado
            gotoxy(3,23); printf("Cliente nao cadastrado. Aperte <enter> para retornar "); // Resposta caso seja diferente.
            getch();
            return -1;
        }//Fim do if
            
        gotoxy(25,11); printf("%s", cliente.nome);
        gotoxy(25,12); printf("%s", cliente.cpf);
        gotoxy(25,13); printf("%s", cliente.endereco);
        gotoxy(25,14); printf("%s", cliente.telefone);
        gotoxy(25,15); printf("%s", (cliente.excluido == 1)? "Excluido" : "Ativo");
    
    }
    
    if((tipo == TUDO)|| (tipo == VEICULOS)){
        gotoxy(3,17); printf("Veiculo(os)\n   -----------------------\n");
        for (i=1;i <= cliente.contVei;i++){
            if (cliente.veiculo[i].excluido == 0){
                printf("   \n   Cod. Veiculo: %d\n",i);
                printf("   Modelo do carro: %s\n", cliente.veiculo[i].modelo);
                printf("   Placa: %s\n", cliente.veiculo[i].placa);
                printf("   Tipo: %s\n", (cliente.veiculo[i].tipo == 1) ? "Carro" : "Moto");
                printf("   Status: %s\n", (cliente.veiculo[i].excluido == 1) ? "Excluido" : "Ativo");
            }
        }
        
    }
        
    fclose(arqC);
    return posicao;
}

void excluiCliente(int posicao){    
    char resp;
    
    abreArquivoCliente("rb+");
    
    if (posicao >= 0){
        gotoxy(3,23); printf("Deseja mesmo excluir o Cliente? [S/n] ");
        resp = getchar();
        
        if (resp == 'S' || resp == 's'){
            cliente.excluido = 1;        
            gotoxy(3,23); printf("Cliente excluido com sucesso. Aperte <enter> para retornar");
            getch();
        }
        
        fechaArquivoCliente(posicao);
    }
}

void editarRegistroCliente(int posicao){     
    int retorno; // Variavel usada para alocar o retorno da escrita no arquivo
    char resp; // variavel usada para capturar as respostas do usuario
             
    abreArquivoCliente("rb+");
    
    formDados();
    if(cliente.excluido == 1){
        gotoxy(3,13); printf("Excluido, [0] p/ incluir: ");
    }
    
    coletaDados();    
    if(cliente.excluido == 1){
        gotoxy(29,13); scanf("%d", &cliente.excluido);fflush(stdin);
    }
    
    fechaArquivoCliente(posicao);
}

void editarRegistroCarros(int posicao){
    int indice;
    int respInt;
    char resp; // variavel usada para capturar as respostas do usuario
             
    abreArquivoCliente("rb+");
        
     // Clear Screen //
    gotoxy(3,11); clearLine();
    gotoxy(3,12); clearLine();
    gotoxy(3,13); clearLine();
    gotoxy(3,14); clearLine();
    gotoxy(3,15); clearLine();
    
    gotoxy(3,10); printf("Digite o codigo.....: ");
    gotoxy(25,10); printf("            ");
    gotoxy(25,10); scanf("%d", &indice);
    fflush(stdin); 
                   
    if(indice > 0){              
        do{      
            if(indice <= cliente.contVei+1){// Teste usado para cadastrar novos carros aos clientes, e varre-los usando o for posteriormente
                cliente.contVei = indice;
                gotoxy(3,10); clearLine();
                gotoxy(3,11); printf("Codigo do Veiculo..........: %d\n", indice);
                gotoxy(3,12); printf("Digite o Modelo do Veiculo.: %s\n", cliente.veiculo[indice].modelo);
                gotoxy(3,13); printf("Tipo: [1]Carro, [2]Moto....: %s\n", (cliente.veiculo[indice].tipo == 1) ? "Carro" : "Moto");
                gotoxy(3,14); printf("Digite a Placa.............: %s\n", cliente.veiculo[indice].placa);
                if(cliente.veiculo[indice].excluido == 1){
                    gotoxy(3,15); printf("Excluido, [0] para incluir.: ");
                }else{
                    gotoxy(3,15); printf("Excluir? [1]Sim, [0]Nao....: ");
                }
            
                gotoxy(3,23); printf("Qual item Alterar? [1]Modelo,[2]Tipo,[3]Placa,[4]Status,[0]Sair: ");
                scanf("%d", &respInt); fflush(stdin);
                gotoxy(3,23); clearLine();
            
                switch(respInt){
                    
                    case 1:
                            gotoxy(32,12); printf("               ");
                            gotoxy(32,12); scanf("%[^\n]",cliente.veiculo[indice].modelo); fflush(stdin);
                        break;
                    case 2:
                            do {
                                gotoxy(32,13); printf("              ");
                                gotoxy(32,13); scanf("%d",&cliente.veiculo[indice].tipo); fflush(stdin);
                            }while((cliente.veiculo[indice].tipo != 1)&&(cliente.veiculo[indice].tipo != 2));
                        break;
                    case 3:
                            gotoxy(32,14); printf("              ");
                            gotoxy(32,14); scanf("%[^\n]",cliente.veiculo[indice].placa);fflush(stdin);
                        break;
                    case 4:
                            gotoxy(32,15); printf("              ");
                            gotoxy(32,15); scanf("%d", &cliente.veiculo[indice].excluido);fflush(stdin);
                        break;
                    case 5:
                        break;
                }
            }
            else{
                 gotoxy(3,23); printf("O ultimo veiculo tem codigo: %d, para adicionar outro digite cod = %d", cliente.contVei, cliente.contVei+1);
                 getch();
                 gotoxy(3,23); clearLine();
                 gotoxy(3,24); clearLine();
                 return;                 
            }
        }while(respInt !=0);
            
    }
    else{
        gotoxy(3,23); printf("Veiculo Inexistente");
        getch();
    }
    
    fechaArquivoCliente(posicao);
}

void pagamentoCliente(int posicao){
    char resp;
    
    abreArquivos("rb+");
    
    rewind(arqS);    
    fread(&sistema, sizeof(sistema), 1,arqS);
    
    if (posicao >= 0){
        gotoxy(3,23); printf("Deseja mesmo Efetuar o pagamento? [S/n] ");
        resp = getchar();
        
        if (resp == 'S' || resp == 's'){
            sistema.caixa.fatDiario += sistema.mensalista; // Soma o valor do mensalista ao faturamento diario 
            cliente.mesPago = (local->tm_mon+1); // Seta o mes pago atualmente pelo cliente.
            cliente.diaPago = (local->tm_mday); // Seta o dia do mes pago atualmente pelo cliente.
            gotoxy(3,23); printf("Pagamento efetuado com sucesso. Aperte <enter> para retornar");
            getch();
        }
    }
    
    fechaArquivos(posicao);
}

//*****************************************************************
// Função Listagem de vagas
//*****************************************************************
void listaVagas(){
    int i;
    abreArquivoSistema("rb");
    
    rewind(arqS);
    fread(&sistema,sizeof(sistema),1,arqS);
    
    for(i=1;i < QTD_Vaga;i++){
        printf("\n\nNo. vaga.....: %d\n",i);
        printf("Status.......: %s\n", (sistema.vagas[i].status == 0)? "Livre":"Ocupada");
        printf("Tipo Cliente.: %s\n",sistema.vagas[i].tipoCliente);
        printf("Placa Veiculo: %s",sistema.vagas[i].placa);
    }
    
    fclose(arqS);
    getch();    
}

//*****************************************************************
//  Função de relatórios
//*****************************************************************
void relatorios(){
    gotoxy(3,10); printf("Valor do faturamento Diario: %.2f", sistema.caixa.fatDiario);
    gotoxy(3,12); printf("Valor do faturamento Semanal: %.2f", sistema.caixa.fatSemanal);
    gotoxy(3,14); printf("Valor do faturamento Mensal: %.2f", sistema.caixa.fatMensal);
}

//*****************************************************************
// Função configuração do sistema
//*****************************************************************
void System_Conf(){
    char buffer[20] = {0};
    char c;
    int retorno;
    int respInt;
    int pos;
    
    arqS = fopen("sistema.bin","rb+");
    if (!arqS){
         arqS = fopen("sistema.bin","wb+");
         if (!arqS){
             printf("Erro ao abrir arquivo do sistema....");
             getch();
             exit(0);
        }
    }
    
    if(sistema.firstStart == 1){
        pos = 0; // Usada para retornar a posicao ZERO do vetor referente a senha.
        alinhar(6,CENTRO,"******************************************");
        alinhar(7,CENTRO,"*                                        *");
        alinhar(8,CENTRO,"*     PARA ALTERAR AS CONFIGURACOES      *");
        alinhar(9,CENTRO,"*     EH NECESSARIO UMA SENHA.           *");
        alinhar(10,CENTRO,"*                                        *");
        alinhar(11,CENTRO,"*       DIGITE-A PARA CONTINUAR:         *");
        alinhar(12,CENTRO,"*        +--------------------+          *");
        alinhar(13,CENTRO,"*        |                    |          *");
        alinhar(14,CENTRO,"*        +--------------------+          *");
        alinhar(15,CENTRO,"*                                        *");
        alinhar(16,CENTRO,"******************************************");
        gotoxy(34,13);
        do {            
            c = getch();fflush(stdin);
            
            if( isprint(c) ) 
            {
                buffer[ pos++ ] = c;
                printf("%c", '*');
            }
        } while(c != 13);
        
        if(strcmp(buffer,sistema.Senha) !=0){
            gotoxy(3,22); printf("Login invalido!!!!");
            gotoxy(3,23); printf("Pressione <enter> para continuar");
            getch();
            return;
        }
            
        do{
            cls();
            printCabecalho();
            alinhar(4, CENTRO, "Configuracao do Sistema");
            pos = 0;
            
            gotoxy(3,10); printf("[01] Nome do estabelecimento: %s", sistema.nome);    
            gotoxy(3,11); printf("[02] Valor diaria...........: %.2f", sistema.diaria);
            gotoxy(3,12); printf("[03] Valor pernoite.........: %.2f", sistema.pernoite);
            gotoxy(3,13); printf("[04] Valor mensalista.......: %.2f", sistema.mensalista);
            gotoxy(3,14); printf("[05] Valor hora.............: %.2f", sistema.vHora);
            gotoxy(3,15); printf("[06] Valor 1/2 adicional....: %.2f", sistema.meiaHora);
            gotoxy(3,16); printf("[07] Valor moto.............: %.2f", sistema.vMoto);
            gotoxy(3,17); printf("[08] QTD de vagas p/ moto...: %d", sistema.vagasMotos);
            gotoxy(3,18); printf("[09] Vagas p/ Mensalistas...: %d", sistema.vagasMensalistas);
            gotoxy(3,19); printf("[10] Senha de Acesso........:");
            
            gotoxy(5,23); printf("Atencao: Esse software foi projetado para %d vagas logo a soma das vagas de mensalistas e motos nao deve ultrapassar esse numero.", QTD_Vaga-1);
            
            gotoxy(3,21); printf("Digite o item que deseja alterar ou [0]SAIR: ");
            scanf("%d", &respInt); fflush(stdin);


            switch(respInt){
                
                case 1:
                        gotoxy(33,10); printf("                    ");
                        gotoxy(33,10); scanf("%[^\n]", sistema.nome); fflush(stdin);
                    break;
                case 2:
                        gotoxy(33,11); printf("              ");
                        gotoxy(33,11); scanf("%f", &sistema.diaria); fflush(stdin);
                    break;
                case 3:
                        gotoxy(33,12); printf("             ");
                        gotoxy(33,12); scanf("%f", &sistema.pernoite); fflush(stdin);
                    break;
                case 4:
                        gotoxy(33,13); printf("              ");
                        gotoxy(33,13); scanf("%f", &sistema.mensalista); fflush(stdin);
                    break;
                case 5:
                        gotoxy(33,14); printf("              ");
                        gotoxy(33,14); scanf("%f", &sistema.vHora); fflush(stdin);
                    break;
                case 6:
                        gotoxy(33,15); printf("              ");
                        gotoxy(33,15); scanf("%f", &sistema.meiaHora); fflush(stdin);
                    break;
                case 7:
                        gotoxy(33,16); printf("              ");
                        gotoxy(33,16); scanf("%f", &sistema.vMoto); fflush(stdin);
                    break;
                    
                case 8:
                        gotoxy(33,17); printf("              ");
                        gotoxy(33,17); scanf("%d", &sistema.vagasMotos); fflush(stdin);
                    break;
                
                case 9:
                        gotoxy(33,18); printf("              ");
                        gotoxy(33,18); scanf("%d", &sistema.vagasMensalistas);
                    break;
                
                case 10:
                        strcpy(sistema.Senha,"");
                        gotoxy(33,18); printf("              ");
                        gotoxy(33,18);
                        do {            
                            c = getch();fflush(stdin);
                            
                            if( isprint(c) ) 
                            {
                                sistema.Senha[ pos++ ] = c;
                                printf("%c", '*');
                            }
                        } while( c != 13 );
                        
                    break;
            
            }
        }while(respInt !=0);
    }
    else{
        pos = 0;
        
        gotoxy(3,10); printf("Digite o nome do estabelecimento:");    
        gotoxy(3,11); printf("Digite o Valor diaria...........:");
        gotoxy(3,12); printf("Digite o Valor pernoite.........:");
        gotoxy(3,13); printf("Digite o Valor mensalista.......:");
        gotoxy(3,14); printf("Digite o Valor hora.............:");
        gotoxy(3,15); printf("Digite o Valor 1/2 adicional....:");
        gotoxy(3,16); printf("Digite o Valor moto.............:");
        gotoxy(3,17); printf("Digite a QTD de vagas p/ moto...:");
        gotoxy(3,18); printf("Digite as vagas p/ Mensalistas..:");
        gotoxy(3,19); printf("DIGITE A SENHA DE CONTROLE......:");
        
        gotoxy(5,22); printf("Atencao: Esse software foi projetado para %d vagas logo a soma das vagas de mensalistas e motos nao deve ultrapassar esse numero.", QTD_Vaga-1);
        
        gotoxy(37,10); scanf("%[^\n]", sistema.nome); fflush(stdin);
        gotoxy(37,11); scanf("%f", &sistema.diaria); fflush(stdin);
        gotoxy(37,12); scanf("%f", &sistema.pernoite); fflush(stdin);
        gotoxy(37,13); scanf("%f", &sistema.mensalista); fflush(stdin);
        gotoxy(37,14); scanf("%f", &sistema.vHora); fflush(stdin);
        gotoxy(37,15); scanf("%f", &sistema.meiaHora); fflush(stdin);
        gotoxy(37,16); scanf("%f", &sistema.vMoto); fflush(stdin);
        gotoxy(37,17); scanf("%d", &sistema.vagasMotos);
        gotoxy(37,18); scanf("%d", &sistema.vagasMensalistas);
        gotoxy(37,19);
        do {            
            c = getch();fflush(stdin);
            
            if( isprint(c) ) 
            {
                sistema.Senha[ pos++ ] = c;
                printf("%c", '*');
            }
        } while( c != 13 );
        
    }
    sistema.caixa.wdia_abertura = local->tm_wday; // Atribui o dia da semana no qual foi aberto o caixa
    sistema.caixa.mdia_abertura = local->tm_mday; // Atribui o dia do mes no qual foi aberto o caixa
    sistema.caixa.mes_abertura = local->tm_mon+1; // Atribui o mes atual ao mes de abertura do caixa 
    sistema.firstStart = 1;
    
   fechaArquivoSistema();
}

//*****************************************************************
// Função Fecha Aplicação
//*****************************************************************
void fecharAplicacao(){
    abreArquivoSistema("rb+");
    sistema.caixa.fatMensal += sistema.caixa.fatDiario; // Soma o conteudo do faturamento diario ao mensal
    sistema.caixa.fatDiario = 0; // Zera o faturamento diario
    sistema.caixa.fatSemanal = sistema.caixa.fatMensal;
        
	if((sistema.caixa.wdia_abertura == (local->tm_wday+1))){ // Se o dia atual for = ao proximo dia, ZERA o Faturamento Semanal;
		sistema.caixa.fatSemanal = 0;
	}            
	if((sistema.caixa.mdia_abertura == local->tm_mday)&&(sistema.caixa.mes_abertura != (local->tm_mon+1))){
		sistema.caixa.fatMensal = 0;
	}    
    
    fechaArquivoSistema();
}