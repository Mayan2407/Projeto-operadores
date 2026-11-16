#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "operadores.txt"

// 1. O REGISTRO (Struct dos dados do operador)
// Chave única inteira (matrícula) + 3 campos (2 textos e 1 inteiro)
typedef struct {
    int matricula;      // Chave única (inteiro)
    char nome[50];      // Campo de texto 1
    char setor[30];     // Campo de texto 2
    int chamados;       // Campo inteiro
} Operador;

// 2. O NÓ DA ÁRVORE
// Cada nó guarda os dados do operador e dois ponteiros para os filhos
typedef struct No {
    Operador op;
    struct No *esquerda;
    struct No *direita;
} No;

// Protótipos das funções para o compilador não reclamar
No* criarNo(Operador novoOp);
void inserir(No **raiz, Operador novoOp);
No* buscar(No *raiz, int mat);
void listarEmOrdem(No *raiz);
void exibirArvore(No *raiz, int nivel);
void carregarArquivo(No **raiz);
void salvarArquivo(No *raiz, FILE *arq);
void liberarMemoria(No *raiz);

int main(void) {
    No *raiz = NULL;
    int opcao;

    // Ao iniciar, lemos o arquivo de texto e jogamos os dados na árvore
    carregarArquivo(&raiz);

    do {
        printf("\n=== SISTEMA DE SUPORTE TECNICO (ABB) ===\n");
        printf("1. Cadastrar novo operador\n");
        printf("2. Buscar operador por matricula\n");
        printf("3. Listar todos (Ordem crescente)\n");
        printf("4. Exibir estrutura da arvore\n");
        printf("5. Imprimir OLA");
        printf("0. Sair (Salvar e liberar memoria)\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida!\n");
            while(getchar() != '\n'); // Limpa o buffer do teclado
            continue;
        }

        if (opcao == 1) {
            Operador novo;
            printf("\n--- CADASTRAR ---\n");
            printf("Matricula: ");
            scanf("%d", &novo.matricula);

            // Verificamos se a matrícula já existe na árvore antes de cadastrar
            if (buscar(raiz, novo.matricula) != NULL) {
                printf("[ERRO] Esta matricula ja esta cadastrada!\n");
            } else {
                printf("Nome: ");
                scanf(" %49[^\n]", novo.nome); // Lê texto com espaços
                printf("Setor: ");
                scanf(" %29[^\n]", novo.setor);
                printf("Chamados resolvidos: ");
                scanf("%d", &novo.chamados);

                inserir(&raiz, novo);
                printf("[SUCESSO] Operador cadastrado!\n");
            }

        } else if (opcao == 2) {
            int mat;
            printf("\n--- BUSCAR ---\n");
            printf("Digite a matricula: ");
            scanf("%d", &mat);

            No *resultado = buscar(raiz, mat);
            if (resultado != NULL) {
                printf("\n[ENCONTRADO] \n");
                printf("Nome: %s\nSetor: %s\nChamados: %d\n", 
                       resultado->op.nome, resultado->op.setor, resultado->op.chamados);
            } else {
                printf("[AVISO] Operador nao encontrado.\n");
            }

        } else if (opcao == 3) {
            printf("\n--- LISTAGEM CRESCENTE (EM-ORDEM) ---\n");
            if (raiz == NULL) {
                printf("Nenhum operador cadastrado.\n");
            } else {
                printf("%-10s | %-25s | %-15s | %-8s\n", "MATRICULA", "NOME", "SETOR", "CHAMADOS");
                printf("------------------------------------------------------------------\n");
                listarEmOrdem(raiz);
            }

        } else if (opcao == 4) {
            printf("\n--- ESTRUTURA DA ARVORE (PRE-ORDEM) ---\n");
            if (raiz == NULL) {
                printf("Arvore vazia.\n");
            } else {
                exibirArvore(raiz, 0);
            }
        } 
		else if (opcao == 5) {
            imprimirOla();
		}else if (opcao == 0) {
            printf("\nSalvando dados no arquivo e limpando memoria...\n");
            
            FILE *arq = fopen(ARQUIVO, "w");
            if (arq != NULL) {
                salvarArquivo(raiz, arq);
                fclose(arq);
            }
            
            liberarMemoria(raiz);
            raiz = NULL;
            printf("Programa encerrado com seguranca!\n");
        }

    } while (opcao != 0);

    return 0;
}


void imprimirOla(){
	printf("OLA");
}

// Cria um nó novo usando malloc (alocação dinâmica na memória RAM)
No* criarNo(Operador novoOp) {
    // Pedimos espaço ao sistema operacional do tamanho exato de um No
    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro de memoria!\n");
        exit(1);
    }
    
    novo->op = novoOp;
    novo->esquerda = NULL; // O nó novo nasce sem filhos
    novo->direita = NULL;
    return novo;
}

// Função de inserção recursiva na Árvore Binária de Busca (ABB)
// Usamos ponteiro duplo (No **raiz) porque precisamos alterar a raiz original se ela for NULL
void inserir(No **raiz, Operador novoOp) {
    if (*raiz == NULL) {
        *raiz = criarNo(novoOp); // Local vazio - Alocar nó aqui
        return;
    }
    // Se a matrícula for menor que a do nó atual, vai para a esquerda
    if (novoOp.matricula < (*raiz)->op.matricula) {
        inserir(&((*raiz)->esquerda), novoOp);
    } 
    // Se for maior, vai para a direita
    else if (novoOp.matricula > (*raiz)->op.matricula) {
        inserir(&((*raiz)->direita), novoOp);
    }
}

No* buscar(No *raiz, int mat) {
    // Se chegou em NULL (não achou) ou se achou a matrícula, retorna o ponteiro
    if (raiz == NULL || raiz->op.matricula == mat) {
        return raiz;
    }
    if (mat < raiz->op.matricula) {
        return buscar(raiz->esquerda, mat);
    }
    return buscar(raiz->direita, mat);
}

// Percurso EM-ORDEM: Esquerda -> Raiz -> Direita
void listarEmOrdem(No *raiz) {
    if (raiz != NULL) {
        listarEmOrdem(raiz->esquerda); // 1. Vai tudo para a esquerda (menores)
        
        // 2. Imprime o operador atual formatado em tabela
        printf("%-10d | %-25s | %-15s | %-8d\n", 
               raiz->op.matricula, raiz->op.nome, raiz->op.setor, raiz->op.chamados);
        
        listarEmOrdem(raiz->direita);  // 3. Vai para a direita (maiores)
    }
}


// Percurso PRÉ-ORDEM: Raiz -> Esquerda -> Direita
void exibirArvore(No *raiz, int nivel) {
    if (raiz != NULL) {
        int i;
        for (i = 0; i < nivel; i++) {
            printf("    "); // Dá um espaço proporcional ao nível para parecer um gráfico
        }
        
        // Mostra a chave atual e quem são os filhos da esquerda e da direita
        printf("[Nivel %d] Matricula: %d (Esq: ", nivel, raiz->op.matricula);
        if (raiz->esquerda != NULL) {
            printf("%d", raiz->esquerda->op.matricula);
        } else {
            printf("NULL");
        }
        
        printf(" | Dir: ");
        if (raiz->direita != NULL) {
            printf("%d", raiz->direita->op.matricula);
        } else {
            printf("NULL");
        }
        printf(")\n");
        
        exibirArvore(raiz->esquerda, nivel + 1); // Desce pra esquerda somando 1 nível
        exibirArvore(raiz->direita, nivel + 1);  // Desce pra direita somando 1 nível
    }
}

// Função para gravar os dados no arquivo de texto ao sair
// preordenar para gravar porque, quando o programa ler de novo, reconstrói a árvore exatamente com a mesma forma e altura
void salvarArquivo(No *raiz, FILE *arq) {
    if (raiz != NULL) {
        fprintf(arq, "%d;%s;%s;%d\n", 
                raiz->op.matricula, raiz->op.nome, raiz->op.setor, raiz->op.chamados);
        salvarArquivo(raiz->esquerda, arq);
        salvarArquivo(raiz->direita, arq);
    }
}

// Função para ler o arquivo de texto quando o programa abre
void carregarArquivo(No **raiz) {
    FILE *arq = fopen(ARQUIVO, "r");
    if (arq != NULL) { // Se o arquivo existe...
        Operador op;
        // Lê linha por linha separada por ponto e vírgula
        while (fscanf(arq, "%d;%49[^;];%29[^;];%d\n", 
                      &op.matricula, op.nome, op.setor, &op.chamados) == 4) {
            inserir(raiz, op); // Insere cada operador lido na árvore em memória
        }
        fclose(arq);
    }
    // Se o arquivo não existe (primeira execução), o programa não faz nada e segue vazio.
}

// Função para liberar a memória RAM usando Pós-Ordem
//Só podemos dar free nos filhos antes de dar free no pai, senão perdemos o endereço de memória deles
void liberarMemoria(No *raiz) {
    if (raiz != NULL) {
        liberarMemoria(raiz->esquerda);
        liberarMemoria(raiz->direita);
        free(raiz); // Libera o nó atual
    }
}
