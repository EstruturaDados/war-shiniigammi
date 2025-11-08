// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ======== ESTRUTURAS ========
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct Missao {
    int id;
    char descricao[100];
    int (*verificar)(struct Missao*, Territorio*, int, const char*);
    int concluida;
    int progresso;
} Missao;

// ======== DECLARAÇÕES ========
void inicializarMapa(Territorio *mapa, int n);
void exibirMapa(const Territorio *mapa, int n);
void atacar(Territorio *atacante, Territorio *defensor);
void gerarMissao(Missao *missao);
void exibirMissao(const Missao *missao);
int verificarMissao(Missao *missao, Territorio *mapa, int n, const char *cor);
void liberarMemoria(Territorio *mapa, Missao *missao);
void limparBuffer();
int menuPrincipal();
int verificarImpossibilidadeDeMissao(Territorio *mapa, int n, Missao *m);

// ======== FUNÇÕES DE MISSÃO ========
int missaoConquistar3(Missao *m, Territorio *mapa, int n, const char *cor);
int missaoEliminarVermelho(Missao *m, Territorio *mapa, int n, const char *cor);
int missaoEliminarAzul(Missao *m, Territorio *mapa, int n, const char *cor);
int missaoControlarNaboo(Missao *m, Territorio *mapa, int n, const char *cor);
int missaoEliminarAmarelo(Missao *m, Territorio *mapa, int n, const char *cor);

// --- Função Principal (main) ---
int main() {
    srand(time(NULL));

    const int n = 5;
    Territorio *mapa = (Territorio *)calloc(n, sizeof(Territorio));
    if (!mapa) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    inicializarMapa(mapa, n);

    Missao *missao = (Missao *)malloc(sizeof(Missao));
    gerarMissao(missao);

    printf("\n🎯 Sua missão: %s\n", missao->descricao);

    char corJogador[10] = "Verde"; // Exemplo de jogador controlando o exército verde

    int opcao;
    int jogoAtivo = 1;

    while (jogoAtivo) {
        opcao = menuPrincipal();

        switch (opcao) {
            case 1: {
                exibirMapa(mapa, n);

                int iAtacante, iDefensor;
                printf("\nEscolha o território atacante (1-%d): ", n);
                scanf("%d", &iAtacante);
                limparBuffer();

                if (iAtacante < 1 || iAtacante > n) {
                    printf("Escolha inválida!\n");
                    break;
                }

                printf("Escolha o território defensor (1-%d ou 0 para sair): ", n);
                scanf("%d", &iDefensor);
                limparBuffer();

                if (iDefensor == 0) break;
                if (iDefensor < 1 || iDefensor > n || iAtacante == iDefensor) {
                    printf("Escolha inválida!\n");
                    break;
                }

                Territorio *atacante = &mapa[iAtacante - 1];
                Territorio *defensor = &mapa[iDefensor - 1];

                if (strcmp(atacante->cor, defensor->cor) == 0) {
                    printf("🚫 Não é possível atacar um aliado!\n");
                    break;
                }

                atacar(atacante, defensor);

                if (verificarMissao(missao, mapa, n, corJogador)) {
                    printf("\n🏆 Missão concluída! Você venceu o jogo!\n");
                    liberarMemoria(mapa, missao);
                    return 0;
                }

                if (verificarImpossibilidadeDeMissao(mapa, n, missao)) {
                    printf("\n💀 Nenhuma missão pode mais ser concluída! Todas as tropas estão em 1.\n");
                    printf("Fim de jogo! Ninguém venceu.\n");
                    jogoAtivo = 0;
                    break;
                }

                printf("\nPressione ENTER para continuar...");
                getchar();
                break;
            }

            case 2:
                exibirMissao(missao);
                printf("\nPressione ENTER para continuar...");
                getchar();
                break;

            case 0:
                printf("\nEncerrando o jogo...\n");
                jogoAtivo = 0;
                break;

            default:
                printf("Opção inválida!\n");
        }
    }

    liberarMemoria(mapa, missao);
    return 0;
}

// ======== IMPLEMENTAÇÕES ========

// Inicializa os territórios do mapa
void inicializarMapa(Territorio *mapa, int n) {
    const char *cores[] = {"Vermelho", "Azul", "Verde", "Amarelo", "Roxo"};
    const char *nomes[] = {"Alderaan", "Tatooine", "Hoth", "Naboo", "Endor"};

    for (int i = 0; i < n; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        strcpy(mapa[i].cor, cores[i]);
        mapa[i].tropas = rand() % 5 + 3;
    }
}

// Exibe o estado atual do mapa
void exibirMapa(const Territorio *mapa, int n) {
    printf("\n=============================\n");
    printf("      MAPA ATUAL DO JOGO\n");
    printf("=============================\n");
    for (int i = 0; i < n; i++) {
        printf("%2d. %-12s | Exército: %-8s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Sistema de ataque
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante->tropas <= 1) {
        printf("\n⚠️  %s não possui tropas suficientes para atacar!\n", atacante->nome);
        return;
    }

    printf("\n⚔️  %s (%s) está atacando %s (%s)!\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);

    int dadoAtaque = rand() % 6 + 1;
    int dadoDefesa = rand() % 6 + 1;

    printf("🎲 Dado de ataque: %d | Dado de defesa: %d\n", dadoAtaque, dadoDefesa);

    if (dadoAtaque > dadoDefesa) {
        defensor->tropas--;
        if (defensor->tropas <= 1) {
            printf("🏆 %s conquistou o território %s!\n", atacante->nome, defensor->nome);
            strcpy(defensor->cor, atacante->cor);
            defensor->tropas = atacante->tropas / 2;
            if (defensor->tropas < 1) defensor->tropas = 1;
        } else {
            printf("💥 Tropas de %s reduzidas para %d!\n", defensor->nome, defensor->tropas);
        }
    } else {
        atacante->tropas--;
        printf("🛡️  %s resistiu ao ataque! Tropas do atacante agora: %d\n",
               atacante->nome, atacante->tropas);
    }
}

// Gera uma missão aleatória
void gerarMissao(Missao *m) {
    int sorteio = rand() % 5;
    m->concluida = 0;
    m->progresso = 0;

    switch (sorteio) {
        case 0:
            m->id = 0;
            strcpy(m->descricao, "Conquistar 3 territórios");
            m->verificar = missaoConquistar3;
            break;
        case 1:
            m->id = 1;
            strcpy(m->descricao, "Eliminar todos os territórios Vermelhos");
            m->verificar = missaoEliminarVermelho;
            break;
        case 2:
            m->id = 2;
            strcpy(m->descricao, "Eliminar todos os territórios Azuis");
            m->verificar = missaoEliminarAzul;
            break;
        case 3:
            m->id = 3;
            strcpy(m->descricao, "Conquistar o território Naboo");
            m->verificar = missaoControlarNaboo;
            break;
        case 4:
            m->id = 4;
            strcpy(m->descricao, "Eliminar todos os territórios Amarelos");
            m->verificar = missaoEliminarAmarelo;
            break;
    }
}

// Exibe o status da missão
void exibirMissao(const Missao *m) {
    printf("\n=== STATUS DA MISSÃO ===\n");
    printf("%s\n", m->descricao);
    printf("Status: %s\n", m->concluida ? "✅ CONCLUÍDA" : "⏳ Em andamento");
}

// Verifica se a missão foi concluída
int verificarMissao(Missao *m, Territorio *mapa, int n, const char *cor) {
    if (m->verificar(m, mapa, n, cor)) {
        m->concluida = 1;
        return 1;
    }
    return 0;
}

// Libera memória
void liberarMemoria(Territorio *mapa, Missao *missao) {
    free(mapa);
    free(missao);
    printf("\nMemória liberada. Fim de jogo.\n");
}

// ======== FUNÇÕES DE MISSÃO ========

int missaoConquistar3(Missao *m, Territorio *mapa, int n, const char *cor) {
    int count = 0;
    for (int i = 0; i < n; i++)
        if (strcmp(mapa[i].cor, cor) == 0) count++;
    return (count >= 3);
}

int missaoEliminarVermelho(Missao *m, Territorio *mapa, int n, const char *cor) {
    for (int i = 0; i < n; i++)
        if (strcmp(mapa[i].cor, "Vermelho") == 0) return 0;
    return 1;
}

int missaoEliminarAzul(Missao *m, Territorio *mapa, int n, const char *cor) {
    for (int i = 0; i < n; i++)
        if (strcmp(mapa[i].cor, "Azul") == 0) return 0;
    return 1;
}

int missaoControlarNaboo(Missao *m, Territorio *mapa, int n, const char *cor) {
    for (int i = 0; i < n; i++)
        if (strcmp(mapa[i].nome, "Naboo") == 0 && strcmp(mapa[i].cor, cor) == 0)
            return 1;
    return 0;
}

int missaoEliminarAmarelo(Missao *m, Territorio *mapa, int n, const char *cor) {
    for (int i = 0; i < n; i++)
        if (strcmp(mapa[i].cor, "Amarelo") == 0)
            return 0;
    return 1;
}

// ======== FUNÇÕES AUXILIARES ========

int verificarImpossibilidadeDeMissao(Territorio *mapa, int n, Missao *m) {
    int podeAtacar = 0;
    for (int i = 0; i < n; i++)
        if (mapa[i].tropas > 1)
            podeAtacar = 1;

    if (!podeAtacar && !m->concluida)
        return 1;

    return 0;
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int menuPrincipal() {
    int opcao;
    printf("\n\n====== MENU PRINCIPAL ======\n");
    printf("1 - Atacar\n");
    printf("2 - Ver Missão\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    limparBuffer();
    return opcao;
}