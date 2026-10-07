#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAX_PERGUNTAS 100
#define TAM_PERGUNTA 250
#define TAM_CATEGORIA 50
#define TAM_CURSO 10
#define TAM_RESPOSTA 5

#define ARQUIVO "perguntas.csv"



// STRUCT


typedef struct {

    int id;
    char texto[TAM_PERGUNTA];
    char categoria[TAM_CATEGORIA];
    char curso[TAM_CURSO];
    char resposta[TAM_RESPOSTA];

} Pergunta;


// BANCO DE PERGUNTAS


Pergunta banco[MAX_PERGUNTAS];
int total = 0;


// FUNÇÃO: REMOVER ENTER


void removerEnter(char texto[]) {

    texto[strcspn(texto, "\r\n")] = '\0';
}



// FUNÇÃO: CONVERTER PARA MAIÚSCULO


void converterMaiusculo(char texto[]) {

    int i;

    for (i = 0; texto[i] != '\0'; i++) {

        texto[i] = toupper((unsigned char)texto[i]);
    }
}


// FUNÇÃO: VERIFICAR TEXTO VAZIO

int textoVazio(char texto[]) {

    int i;

    for (i = 0; texto[i] != '\0'; i++) {

        if (!isspace((unsigned char)texto[i])) {

            return 0;
        }
    }

    return 1;
}



// FUNÇÃO: VERIFICAR PONTO E VÍRGULA

int possuiPontoEVirgula(char texto[]) {

    if (strchr(texto, ';') != NULL) {

        return 1;
    }

    return 0;
}



// FUNÇÃO: LER NÚMERO INTEIRO


int lerInteiro() {

    char entrada[50];
    char *fim;
    long numero;

    while (1) {

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {

            printf("Erro ao ler a entrada.\n");
            continue;
        }

        removerEnter(entrada);

        if (textoVazio(entrada)) {

            printf("Digite um numero: ");
            continue;
        }

        numero = strtol(entrada, &fim, 10);

        while (isspace((unsigned char)*fim)) {

            fim++;
        }

        if (*fim == '\0') {

            if (numero >= 0 && numero <= 2147483647) {

                return (int)numero;
            }
        }

        printf("Valor invalido. Digite apenas numeros: ");
    }
}



// FUNÇÃO: VALIDAR ID


int validarID(int id) {

    if (id <= 0) {

        return 0;
    }

    if (id > 2147483647) {

        return 0;
    }

    return 1;
}


// FUNÇÃO: VALIDAR CURSO


int validarCurso(char curso[]) {

    if (strcmp(curso, "CC") == 0 ||
        strcmp(curso, "ES") == 0 ||
        strcmp(curso, "ADS") == 0) {

        return 1;
    }

    return 0;
}


// FUNÇÃO: VALIDAR RESPOSTA


int validarResposta(char resposta[]) {

    if (strcmp(resposta, "SIM") == 0 ||
        strcmp(resposta, "NAO") == 0) {

        return 1;
    }

    return 0;
}


// FUNÇÃO: VALIDAR PERGUNTA COMPLETA


int validarPergunta(Pergunta pergunta) {

    if (!validarID(pergunta.id)) {

        return 0;
    }

    if (textoVazio(pergunta.texto)) {

        return 0;
    }

    if (textoVazio(pergunta.categoria)) {

        return 0;
    }

    if (!validarCurso(pergunta.curso)) {

        return 0;
    }

    if (!validarResposta(pergunta.resposta)) {

        return 0;
    }

    if (possuiPontoEVirgula(pergunta.texto)) {

        return 0;
    }

    if (possuiPontoEVirgula(pergunta.categoria)) {

        return 0;
    }

    if (possuiPontoEVirgula(pergunta.curso)) {

        return 0;
    }

    if (possuiPontoEVirgula(pergunta.resposta)) {

        return 0;
    }

    return 1;
}


// FUNÇÃO: PROCURAR ID


int procurarPorID(int id) {

    int i;

    for (i = 0; i < total; i++) {

        if (banco[i].id == id) {

            return i;
        }
    }

    return -1;
}


// FUNÇÃO: SALVAR CSV


int salvarCSV() {

    FILE *arquivo;
    int i;

    arquivo = fopen(ARQUIVO, "w");

    if (arquivo == NULL) {

        printf("\nERRO: Nao foi possivel abrir o arquivo %s para escrita.\n", ARQUIVO);
        printf("Verifique se o arquivo esta sendo usado por outro programa.\n");

        return 0;
    }


    for (i = 0; i < total; i++) {

        // Nunca grava dados inválidos
        if (!validarPergunta(banco[i])) {

            printf("\nERRO: Foi encontrado um registro invalido no banco.\n");
            printf("O arquivo nao foi atualizado.\n");

            fclose(arquivo);

            return 0;
        }


        // Garante que o ID não esteja duplicado
        if (procurarPorID(banco[i].id) != i) {

            printf("\nERRO: Foi encontrado um ID duplicado.\n");
            printf("O arquivo nao foi atualizado.\n");

            fclose(arquivo);

            return 0;
        }


        if (fprintf(
                arquivo,
                "%d;%s;%s;%s;%s\n",
                banco[i].id,
                banco[i].texto,
                banco[i].categoria,
                banco[i].curso,
                banco[i].resposta
            ) < 0) {

            printf("\nERRO: Falha ao escrever no arquivo %s.\n", ARQUIVO);

            fclose(arquivo);

            return 0;
        }
    }


    if (fclose(arquivo) != 0) {

        printf("\nERRO: Falha ao fechar o arquivo %s.\n", ARQUIVO);

        return 0;
    }


    return 1;
}


// FUNÇÃO: VALIDAR REGISTRO DO CSV


int validarRegistroCSV(Pergunta pergunta) {

    if (!validarPergunta(pergunta)) {

        return 0;
    }

    return 1;
}



// FUNÇÃO: CARREGAR CSV


void carregarCSV() {

    FILE *arquivo;
    char linha[600];

    arquivo = fopen(ARQUIVO, "r");


    // Se o arquivo não existir, cria um arquivo vazio
    if (arquivo == NULL) {

        arquivo = fopen(ARQUIVO, "w");

        if (arquivo == NULL) {

            printf("ERRO: Nao foi possivel criar o arquivo %s.\n", ARQUIVO);
            printf("Verifique as permissoes da pasta.\n");

            return;
        }

        if (fclose(arquivo) != 0) {

            printf("ERRO: Nao foi possivel fechar o arquivo %s.\n", ARQUIVO);

            return;
        }

        return;
    }


    total = 0;


    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        char *campo;
        Pergunta nova;


        removerEnter(linha);


        if (textoVazio(linha)) {

            continue;
        }


        // ID


        campo = strtok(linha, ";");

        if (campo == NULL) {

            printf("AVISO: Registro invalido ignorado.\n");
            continue;
        }


        char *fim;
        long id;

        id = strtol(campo, &fim, 10);

        while (isspace((unsigned char)*fim)) {

            fim++;
        }

        if (*fim != '\0' ||
            id <= 0 ||
            id > 2147483647) {

            printf("AVISO: ID invalido encontrado no CSV. Registro ignorado.\n");
            continue;
        }

        nova.id = (int)id;


        // PERGUNTA
   

        campo = strtok(NULL, ";");

        if (campo == NULL ||
            textoVazio(campo) ||
            strlen(campo) >= TAM_PERGUNTA) {

            printf("AVISO: Pergunta invalida no CSV. Registro ignorado.\n");
            continue;
        }

        strcpy(nova.texto, campo);


   
        // CATEGORIA

        campo = strtok(NULL, ";");

        if (campo == NULL ||
            textoVazio(campo) ||
            strlen(campo) >= TAM_CATEGORIA) {

            printf("AVISO: Categoria invalida no CSV. Registro ignorado.\n");
            continue;
        }

        strcpy(nova.categoria, campo);


        // CURSO


        campo = strtok(NULL, ";");

        if (campo == NULL ||
            strlen(campo) >= TAM_CURSO) {

            printf("AVISO: Curso invalido no CSV. Registro ignorado.\n");
            continue;
        }

        strcpy(nova.curso, campo);
        converterMaiusculo(nova.curso);


        // RESPOSTA


        campo = strtok(NULL, ";");

        if (campo == NULL ||
            strlen(campo) >= TAM_RESPOSTA) {

            printf("AVISO: Resposta invalida no CSV. Registro ignorado.\n");
            continue;
        }

        strcpy(nova.resposta, campo);
        converterMaiusculo(nova.resposta);


        // VERIFICAR SE EXISTEM CAMPOS EXTRAS


        campo = strtok(NULL, ";");

        if (campo != NULL) {

            printf("AVISO: Registro com campos extras no CSV. Registro ignorado.\n");
            continue;
        }



        // VERIFICAR REGISTRO COMPLETO
   

        if (!validarRegistroCSV(nova)) {

            printf("AVISO: Registro invalido no CSV. Registro ignorado.\n");
            continue;
        }


   
        // VERIFICAR ID DUPLICADO
   

        if (procurarPorID(nova.id) != -1) {

            printf("AVISO: ID %d duplicado no CSV. Registro ignorado.\n", nova.id);
            continue;
        }


      
        // VERIFICAR LIMITE


        if (total >= MAX_PERGUNTAS) {

            printf("AVISO: Limite de perguntas atingido.\n");
            break;
        }


        // Registro aprovado
        banco[total] = nova;
        total++;
    }


    if (fclose(arquivo) != 0) {

        printf("ERRO: Nao foi possivel fechar o arquivo %s.\n", ARQUIVO);
    }
}



// 1 - CADASTRAR PERGUNTA


void cadastrarPergunta() {

    Pergunta nova;


    if (total >= MAX_PERGUNTAS) {

        printf("\nO banco de perguntas esta cheio!\n");

        return;
    }


    printf("\n========================================\n");
    printf("         CADASTRAR PERGUNTA\n");
    printf("========================================\n");


    // =================================================
    // ID
    // =================================================

    printf("Codigo ID: ");

    nova.id = lerInteiro();


    if (!validarID(nova.id)) {

        printf("ERRO: O codigo deve ser maior que zero.\n");

        return;
    }


    if (procurarPorID(nova.id) != -1) {

        printf("ERRO: Esse codigo ja esta cadastrado.\n");

        return;
    }


    // PERGUNTA


    printf("Pergunta: ");

    if (fgets(nova.texto, TAM_PERGUNTA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a pergunta.\n");

        return;
    }

    removerEnter(nova.texto);


    if (textoVazio(nova.texto)) {

        printf("ERRO: A pergunta nao pode ficar vazia.\n");

        return;
    }


    if (possuiPontoEVirgula(nova.texto)) {

        printf("ERRO: A pergunta nao pode conter ';'.\n");

        return;
    }



    // CATEGORIA


    printf("Categoria: ");

    if (fgets(nova.categoria, TAM_CATEGORIA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a categoria.\n");

        return;
    }

    removerEnter(nova.categoria);


    if (textoVazio(nova.categoria)) {

        printf("ERRO: A categoria nao pode ficar vazia.\n");

        return;
    }


    if (possuiPontoEVirgula(nova.categoria)) {

        printf("ERRO: A categoria nao pode conter ';'.\n");

        return;
    }



    // CURSO


    printf("Curso (CC/ES/ADS): ");

    if (fgets(nova.curso, TAM_CURSO, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler o curso.\n");

        return;
    }

    removerEnter(nova.curso);
    converterMaiusculo(nova.curso);


    if (!validarCurso(nova.curso)) {

        printf("ERRO: Curso invalido.\n");
        printf("Use somente CC, ES ou ADS.\n");

        return;
    }



    // RESPOSTA


    printf("Resposta (SIM/NAO): ");

    if (fgets(nova.resposta, TAM_RESPOSTA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a resposta.\n");

        return;
    }

    removerEnter(nova.resposta);
    converterMaiusculo(nova.resposta);


    if (!validarResposta(nova.resposta)) {

        printf("ERRO: Resposta invalida.\n");
        printf("Use somente SIM ou NAO.\n");

        return;
    }



    // VALIDAÇÃO FINAL


    if (!validarPergunta(nova)) {

        printf("\nERRO: Os dados informados sao invalidos.\n");

        return;
    }



    // ADICIONAR AO BANCO


    banco[total] = nova;
    total++;

    // SALVAR


    if (salvarCSV()) {

        printf("\nPergunta cadastrada com sucesso!\n");
    }
    else {

        // Se não conseguiu salvar,
        // desfaz a inclusão no banco.
        total--;

        printf("\nERRO: A pergunta nao foi salva no arquivo.\n");
    }
}


// 2 - LISTAR TODAS AS PERGUNTAS


void listarPerguntas() {

    int i;


    if (total == 0) {

        printf("\nNenhuma pergunta cadastrada.\n");

        return;
    }


    printf("\n========================================\n");
    printf("         TODAS AS PERGUNTAS\n");
    printf("========================================\n");


    for (i = 0; i < total; i++) {

        printf("\n----------------------------------------\n");

        printf("Codigo: %d\n", banco[i].id);
        printf("Pergunta: %s\n", banco[i].texto);
        printf("Categoria: %s\n", banco[i].categoria);
        printf("Curso: %s\n", banco[i].curso);
        printf("Resposta: %s\n", banco[i].resposta);
    }
}



// 3 - CONSULTAR POR CATEGORIA

void consultarPorCategoria() {

    char busca[TAM_CATEGORIA];

    int achou = 0;
    int i;


    if (total == 0) {

        printf("\nNenhuma pergunta cadastrada.\n");

        return;
    }


    printf("\nDigite a categoria desejada: ");

    if (fgets(busca, TAM_CATEGORIA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a categoria.\n");

        return;
    }

    removerEnter(busca);


    if (textoVazio(busca)) {

        printf("ERRO: A categoria nao pode ficar vazia.\n");

        return;
    }


    printf("\n========================================\n");
    printf("       PERGUNTAS DA CATEGORIA\n");
    printf("========================================\n");


    for (i = 0; i < total; i++) {

        if (strcmp(banco[i].categoria, busca) == 0) {

            achou = 1;

            printf("\n[%d] %s - %s - %s\n",
                   banco[i].id,
                   banco[i].texto,
                   banco[i].curso,
                   banco[i].resposta);
        }
    }


    if (!achou) {

        printf("\nNenhuma pergunta encontrada nessa categoria.\n");
    }
}


// 4 - CONSULTAR POR CURSO


void consultarPorCurso() {

    char busca[TAM_CURSO];

    int achou = 0;
    int i;


    if (total == 0) {

        printf("\nNenhuma pergunta cadastrada.\n");

        return;
    }


    printf("\nDigite o curso (CC/ES/ADS): ");

    if (fgets(busca, TAM_CURSO, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler o curso.\n");

        return;
    }

    removerEnter(busca);
    converterMaiusculo(busca);


    if (!validarCurso(busca)) {

        printf("ERRO: Curso invalido.\n");
        printf("Use somente CC, ES ou ADS.\n");

        return;
    }


    printf("\n========================================\n");
    printf("        PERGUNTAS DO CURSO %s\n", busca);
    printf("========================================\n");


    for (i = 0; i < total; i++) {

        if (strcmp(banco[i].curso, busca) == 0) {

            achou = 1;

            printf("\n[%d] %s - %s - %s\n",
                   banco[i].id,
                   banco[i].texto,
                   banco[i].categoria,
                   banco[i].resposta);
        }
    }


    if (!achou) {

        printf("\nNenhuma pergunta encontrada para esse curso.\n");
    }
}


// 5 - ATUALIZAR PERGUNTA

void atualizarPergunta() {

    int idBusca;
    int posicao;

    Pergunta nova;


    if (total == 0) {

        printf("\nNenhuma pergunta cadastrada.\n");

        return;
    }


    printf("\nDigite o codigo da pergunta: ");

    idBusca = lerInteiro();


    if (!validarID(idBusca)) {

        printf("\nERRO: Codigo invalido.\n");

        return;
    }


    posicao = procurarPorID(idBusca);


    if (posicao == -1) {

        printf("\nPergunta com codigo %d nao encontrada.\n", idBusca);

        return;
    }


    printf("\n========================================\n");
    printf("          ATUALIZAR PERGUNTA\n");
    printf("========================================\n");


    // O ID não será alterado
    nova.id = banco[posicao].id;



    // NOVO TEXTO

    printf("Novo texto: ");

    if (fgets(nova.texto, TAM_PERGUNTA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler o texto.\n");

        return;
    }

    removerEnter(nova.texto);


    if (textoVazio(nova.texto)) {

        printf("ERRO: A pergunta nao pode ficar vazia.\n");

        return;
    }


    if (possuiPontoEVirgula(nova.texto)) {

        printf("ERRO: A pergunta nao pode conter ';'.\n");

        return;
    }


    // NOVA CATEGORIA


    printf("Nova categoria: ");

    if (fgets(nova.categoria, TAM_CATEGORIA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a categoria.\n");

        return;
    }

    removerEnter(nova.categoria);


    if (textoVazio(nova.categoria)) {

        printf("ERRO: A categoria nao pode ficar vazia.\n");

        return;
    }


    if (possuiPontoEVirgula(nova.categoria)) {

        printf("ERRO: A categoria nao pode conter ';'.\n");

        return;
    }


    // NOVO CURSO


    printf("Novo curso (CC/ES/ADS): ");

    if (fgets(nova.curso, TAM_CURSO, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler o curso.\n");

        return;
    }

    removerEnter(nova.curso);
    converterMaiusculo(nova.curso);


    if (!validarCurso(nova.curso)) {

        printf("ERRO: Curso invalido.\n");
        printf("Use somente CC, ES ou ADS.\n");

        return;
    }


    // NOVA RESPOSTA


    printf("Nova resposta (SIM/NAO): ");

    if (fgets(nova.resposta, TAM_RESPOSTA, stdin) == NULL) {

        printf("ERRO: Nao foi possivel ler a resposta.\n");

        return;
    }

    removerEnter(nova.resposta);
    converterMaiusculo(nova.resposta);


    if (!validarResposta(nova.resposta)) {

        printf("ERRO: Resposta invalida.\n");
        printf("Use somente SIM ou NAO.\n");

        return;
    }



    // VALIDAÇÃO FINAL


    if (!validarPergunta(nova)) {

        printf("\nERRO: Os dados informados sao invalidos.\n");

        return;
    }


    // SALVAR ALTERAÇÃO


    Pergunta antiga = banco[posicao];

    banco[posicao] = nova;


    if (salvarCSV()) {

        printf("\nPergunta atualizada com sucesso!\n");
    }
    else {

        // Se falhar ao salvar, restaura os dados anteriores.
        banco[posicao] = antiga;

        printf("\nERRO: A alteracao nao foi salva.\n");
    }
}



// 6 - EXCLUIR PERGUNTA


void excluirPergunta() {

    int idBusca;
    int posicao;
    int i;


    if (total == 0) {

        printf("\nNenhuma pergunta cadastrada.\n");

        return;
    }


    printf("\nDigite o codigo ID da pergunta: ");

    idBusca = lerInteiro();


    if (!validarID(idBusca)) {

        printf("\nERRO: Codigo invalido.\n");

        return;
    }


    posicao = procurarPorID(idBusca);


    if (posicao == -1) {

        printf("\nPergunta com codigo %d nao encontrada.\n", idBusca);

        return;
    }


    // Move as perguntas seguintes uma posição para trás
    for (i = posicao; i < total - 1; i++) {

        banco[i] = banco[i + 1];
    }


    total--;


    if (salvarCSV()) {

        printf("\nPergunta excluida com sucesso!\n");
    }
    else {

        printf("\nERRO: Nao foi possivel atualizar o arquivo CSV.\n");
    }
}


// MAIN

int main() {

    int opcao;


    // Configuração do terminal para acentos
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);


    // Carrega as perguntas existentes
    carregarCSV();


    do {

        system("cls");


        printf("||=========================================||");
        printf("\n||     GERENCIADOR DE PERGUNTAS - TI       ||");
        printf("\n||=========================================||\n");

        printf("\nPerguntas cadastradas: %d\n", total);

        printf("\n1 - Cadastrar pergunta\n");
        printf("2 - Listar todas as perguntas\n");
        printf("3 - Consultar perguntas por categoria\n");
        printf("4 - Consultar perguntas por curso\n");
        printf("5 - Atualizar pergunta\n");
        printf("6 - Excluir pergunta\n");
        printf("0 - Sair\n");

        printf("\n-----------------------------------------\n");
        printf("Escolha uma opcao: ");


        opcao = lerInteiro();


        switch (opcao) {

            case 1:

                cadastrarPergunta();

                break;


            case 2:

                listarPerguntas();

                break;


            case 3:

                consultarPorCategoria();

                break;


            case 4:

                consultarPorCurso();

                break;


            case 5:

                atualizarPergunta();

                break;


            case 6:

                excluirPergunta();

                break;


            case 0:

                printf("\nPrograma encerrado.\n");

                break;


            default:

                printf("\nERRO: Opcao invalida!\n");
        }


        if (opcao != 0) {

            printf("\nPressione ENTER para continuar...");

            getchar();
        }


    } while (opcao != 0);


    return 0;
}

