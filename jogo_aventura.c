#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Integrantes:
    Enzo Maranho Tucilho - 10436106
    Lorenzo Silverio Morales - 10415482
    Mateus Fernandes de Araujo - 10731058
    Pedro Daniel Reis Souza - 10443566

    Link para vídeo do Youtube: https://youtu.be/xZTkAuj18ug
*/

// Estrutura de dados do item disponível de cada fase
typedef struct{
    char nome[100];
    float peso;
    float valor;
    char regra[100];
} item;

// Estrutura de dados da fase
struct fase{
    char nome[100];
    float capacidade;
    char regra[100];
    int qtdeItens;
    item item[100];
};

/*
    Faz parte do algoritmo de ordenação escolhido (Quick Sort [nlogn])
    Função de particionamento seguindo a regra valor/peso
    Itens com maior eficiência resultante da razão ficam à esquerda
*/
int particiona(item v[], int inicio, int fim){
    item pivo = v[fim];
    int i = inicio;

    for(int j = inicio; j < fim; j++){
        float razaoAtual = v[j].valor / v[j].peso;
        float razaoPivo = pivo.valor / pivo.peso;

        if(razaoAtual >= razaoPivo){
            item aux = v[j];
            v[j] = v[i];
            v[i] = aux;
            i++;
        }
    }

    item aux = v[i];
    v[i] = v[fim];
    v[fim] = aux;

    return i;
}

/*
    Ordena os itens em ordem decrescente com base
    na razão valor/peso
*/
void quickSortDecrescente(item v[], int inicio, int fim){
    if(inicio < fim){
        int p = particiona(v, inicio, fim);
        quickSortDecrescente(v, inicio, p - 1);
        quickSortDecrescente(v, p + 1, fim);
    }
}

/*
    Algoritmo de mochila fracionária
    Retorna um vetor com as seguintes relações:
    x = 1 -> item inteiro
    0<x<1 -> item fracinado
    x = 0 -> item não escolhido
*/
float* mochilaFracionaria(float* p, int n, float W){
    float* x = calloc(n, sizeof(float));

    int i = 0;
    while(i < n && W > 0){
        if(p[i] <= W){
            x[i] = 1;
            W -= p[i];
        }else{
            x[i] = W / p[i];
            W = 0;
        }
        i++;
    }

    return x;
}

/*
    Aplica multiplicador para regras específicas 
    (como para itens do tipo mágico e sobrevivência)
*/
void multiplicador(struct fase *f, char *regra, float fator){
    for(int i = 0; i < f->qtdeItens; i++){
        if(strcmp(f->item[i].regra, regra) == 0){
            f->item[i].valor *= fator;
        }
    }
}

// Ordena os itens e aplica o algoritmo de mochila fracionária
float *processoGenerico(struct fase *f){
    float w[f->qtdeItens];

    quickSortDecrescente(f->item, 0, f->qtdeItens - 1);

    for(int i = 0; i < f->qtdeItens; i++){
        w[i] = f->item[i].peso;
    }

    return mochilaFracionaria(w, f->qtdeItens, f->capacidade);
}

/*
    Ordena os itens e aplica o algoritmo de mochila fracionária modificada
    pela regra itens tecnológicos não podem ser fracionados
*/
float *processarTecnologicos(struct fase *f){
    float w[f->qtdeItens];

    quickSortDecrescente(f->item, 0, f->qtdeItens - 1);

    for(int i = 0; i < f->qtdeItens; i++){
        w[i] = f->item[i].peso;
    }

    float *x = calloc(f->qtdeItens, sizeof(float));
    float capacidadeRestante = f->capacidade;

    for(int i = 0; i < f->qtdeItens && capacidadeRestante > 0; i++){
        if(w[i] <= capacidadeRestante){
            x[i] = 1;
            capacidadeRestante -= w[i];
        }else if(strcmp(f->item[i].regra, "tecnologico") != 0){
            x[i] = capacidadeRestante / w[i];
            capacidadeRestante = 0;
        }
    }

    return x;
}

/*
    Ordena os itens e aplica o algoritmo de mochila fracionária modificada
    pela regra apenas os 3 melhores itens podem ser escolhidos
*/
float *processarTresMelhores(struct fase *f){
    float w[f->qtdeItens];

    quickSortDecrescente(f->item, 0, f->qtdeItens - 1);

    for(int i = 0; i < f->qtdeItens; i++){
        w[i] = f->item[i].peso;
    }

    float *x = calloc(f->qtdeItens, sizeof(float));
    float capacidadeRestante = f->capacidade;

    for(int i = 0; i < 3 && capacidadeRestante > 0; i++){
        if(w[i] <= capacidadeRestante){
            x[i] = 1;
            capacidadeRestante -= w[i];
        }else{
            x[i] = capacidadeRestante / w[i];
            capacidadeRestante = 0;
        }
    }

    return x;
}

/*
    Gera relatório da fase no arquivo de saída
*/
void arquivoSaida(float *x, struct fase *fases, int indiceFase, char *nomeSaida){
    FILE *saida = fopen(nomeSaida, "a");

    if(saida == NULL){
        printf("Erro no arquivo de saída\n");
        exit(1);
    }

    fprintf(saida, "--- FASE: %s ---\n", fases[indiceFase].nome);
    fprintf(saida, "Capacidade da mochila: %.2f kg\n", fases[indiceFase].capacidade);

    if(strcmp(fases[indiceFase].regra, "MAGICOS_VALOR_DOBRADO\n") == 0)
        fprintf(saida, "Regra aplicada: Itens mágicos com valor dobrado\n");
    else if(strcmp(fases[indiceFase].regra, "TECNOLOGICOS_INTEIROS\n") == 0)
        fprintf(saida, "Regra aplicada: Itens tecnologicos não podem ser fracionados\n");
    else if(strcmp(fases[indiceFase].regra, "SOBREVIVENCIA_DESVALORIZADA\n") == 0)
        fprintf(saida, "Regra aplicada: Itens de sobrevivencia perdem 20%% do valor\n");
    else if(strcmp(fases[indiceFase].regra, "TRES_MELHORES_VALOR_PESO\n") == 0)
        fprintf(saida, "Regra aplicada: Apenas os tres itens com maior valor/peso podem ser escolhidos\n");

    fprintf(saida, "\n");

    float soma = 0;

    for(int j = 0; j < fases[indiceFase].qtdeItens; j++){
        if(x[j] == 1.0){
            fprintf(saida, "Pegou (inteiro) %s, (%.2fkg, R$ %.2f)\n",
                fases[indiceFase].item[j].nome,
                fases[indiceFase].item[j].peso,
                fases[indiceFase].item[j].valor);
        }
        else if(x[j] > 0){
            fprintf(saida, "Pegou (fracionado) %s, (%.2fkg, R$ %.2f)\n",
                fases[indiceFase].item[j].nome,
                fases[indiceFase].item[j].peso * x[j],
                fases[indiceFase].item[j].valor * x[j]);
        }

        soma += x[j] * fases[indiceFase].item[j].valor;
    }

    fprintf(saida, "Lucro da fase: R$ %.2f\n\n", soma);

    fclose(saida);
}

// Limpa (ou cria, inicialmente) o arquivo de saída antes de gerar novo relatório
void limparArquivoSaida(char *nomeSaida){
    FILE *saida = fopen(nomeSaida, "w");
    if(saida != NULL)
        fclose(saida);
}

// Lê o arquivo de entrada e carrega as fases nas structs
void carregarFases(FILE *entrada, struct fase fases[], int *totalFases){
    char buffer[256];
    int faseAtual = -1;
    int itemAtual = 0;

    while(fgets(buffer, sizeof(buffer), entrada) != NULL){

        if(strncmp(buffer, "FASE:", 5) == 0){
            strcpy(fases[++faseAtual].nome, buffer + 6);
            fases[faseAtual].nome[strcspn(fases[faseAtual].nome, "\n")] = '\0';
            itemAtual = 0;
        }

        else if(strncmp(buffer, "CAPACIDADE:", 11) == 0){
            fases[faseAtual].capacidade = atof(buffer + 12);
        }

        else if(strncmp(buffer, "REGRA:", 6) == 0){
            strcpy(fases[faseAtual].regra, buffer + 7);
        }

        else if(strncmp(buffer, "ITEM:", 5) == 0){
            strcpy(fases[faseAtual].item[itemAtual].nome, strtok(buffer + 6, ","));
            fases[faseAtual].item[itemAtual].peso = atof(strtok(NULL, ","));
            fases[faseAtual].item[itemAtual].valor = atof(strtok(NULL, ","));
            strcpy(fases[faseAtual].item[itemAtual].regra, strtok(NULL, ","));

            fases[faseAtual].item[itemAtual].regra[
                strcspn(fases[faseAtual].item[itemAtual].regra, "\n")
            ] = '\0';

            // remove espaços extras
            char *t = fases[faseAtual].item[itemAtual].regra;
            while(*t == ' ') memmove(t, t + 1, strlen(t));

            itemAtual++;
            fases[faseAtual].qtdeItens = itemAtual;
        }
    }

    *totalFases = faseAtual + 1;
}

// Seleciona e executa a regra adequada para cada fase
void processarFase(struct fase *f, struct fase fases[], int indice, char *nomeSaida){
    float *x;

    if(strcmp(f->regra, "MAGICOS_VALOR_DOBRADO\n") == 0){
        multiplicador(f, "magico", 2.0);
        x = processoGenerico(f);
    }

    else if(strcmp(f->regra, "TECNOLOGICOS_INTEIROS\n") == 0){
        x = processarTecnologicos(f);
    }

    else if(strcmp(f->regra, "SOBREVIVENCIA_DESVALORIZADA\n") == 0){
        multiplicador(f, "sobrevivencia", 0.8);
        x = processoGenerico(f);
    }

    else if(strcmp(f->regra, "TRES_MELHORES_VALOR_PESO\n") == 0){
        x = processarTresMelhores(f);
    }

    else{
        return;
    }

    arquivoSaida(x, fases, indice, nomeSaida);
    free(x);
}

/*
    FLUXO PRINCIPAL:
    1. Valida argumentos passados na linha de comando (argc)
    2. Abre arquivo de Entrada
    3. Carrega fases nas structs
    4. Processa cada fase com suas respectivas regras
    5. Gera saída (relatório)
*/
int main(int argc, char **argv){
    if(argc != 3){
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");

    if(entrada == NULL){
        printf("Erro com a abertura de arquivo entrada\n");
        return 1;
    }

    struct fase fases[4];
    int totalFases;

    limparArquivoSaida(argv[2]);

    carregarFases(entrada, fases, &totalFases);

    for(int i = 0; i < totalFases; i++){
        processarFase(&fases[i], fases, i, argv[2]);
    }

    fclose(entrada);
    return 0;
}