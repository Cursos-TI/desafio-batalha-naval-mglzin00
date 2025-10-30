#include <stdio.h>
#include <stdbool.h>

// ========================
// Configurações do jogo
// ========================
#define TAM 10            // tamanho do tabuleiro: 10x10
#define TAM_NAVIO 3       // cada navio ocupa 3 posições
#define AGUA 0
#define NAVIO 3

// Coordenadas iniciais (linha, coluna) e orientações dos navios
// ATENÇÃO: índices começam em 0 (0..9). Ajuste aqui, se quiser.
// Navio horizontal começa em (2, 4)
#define N1_LINHA 2
#define N1_COL   4
// Navio vertical começa em (6, 1)
#define N2_LINHA 6
#define N2_COL   1

// Orientações
typedef enum { HORIZONTAL, VERTICAL } Orientacao;

// ========================
// Funções utilitárias
// ========================

// Verifica se um segmento de navio cabe no tabuleiro
bool cabe_no_tabuleiro(int linha, int col, Orientacao o) {
    if (o == HORIZONTAL) {
        return (linha >= 0 && linha < TAM) &&
               (col >= 0 && (col + TAM_NAVIO - 1) < TAM);
    } else { // VERTICAL
        return (linha >= 0 && (linha + TAM_NAVIO - 1) < TAM) &&
               (col >= 0 && col < TAM);
    }
}

// Verifica se haverá sobreposição com o que já está no tabuleiro
bool ha_sobreposicao(int tab[TAM][TAM], int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r = linha + (o == VERTICAL ? i : 0);
        int c = col   + (o == HORIZONTAL ? i : 0);
        if (tab[r][c] != AGUA) {
            return true; // já tem algo aqui
        }
    }
    return false;
}

// Marca no tabuleiro as células do navio copiando o valor do vetor do navio
void posiciona_navio(int tab[TAM][TAM], int vetor_navio[TAM_NAVIO],
                     int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r = linha + (o == VERTICAL ? i : 0);
        int c = col   + (o == HORIZONTAL ? i : 0);
        tab[r][c] = vetor_navio[i]; // copia o valor 3 de cada posição do vetor
    }
}

// Exibe o tabuleiro no console (0 = água, 3 = navio)
void exibe_tabuleiro(int tab[TAM][TAM]) {
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    // 1) Representação do tabuleiro: matriz 10x10 iniciada com 0 (água)
    int tabuleiro[TAM][TAM];
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            tabuleiro[i][j] = AGUA;
        }
    }

    // 2) Representação dos navios: dois vetores unidimensionais de tamanho 3
    // Cada posição do vetor contém o valor 3, que será copiado para a matriz.
    int navio_horizontal[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO};
    int navio_vertical[TAM_NAVIO]   = {NAVIO, NAVIO, NAVIO};

    // Validações de limites
    if (!cabe_no_tabuleiro(N1_LINHA, N1_COL, HORIZONTAL)) {
        printf("Erro: navio horizontal fora dos limites.\n");
        return 1;
    }
    if (!cabe_no_tabuleiro(N2_LINHA, N2_COL, VERTICAL)) {
        printf("Erro: navio vertical fora dos limites.\n");
        return 1;
    }

    // Validação de sobreposição
    if (ha_sobreposicao(tabuleiro, N1_LINHA, N1_COL, HORIZONTAL)) {
        printf("Erro: sobreposicao ao posicionar o navio horizontal.\n");
        return 1;
    }
    // Posiciona o primeiro navio (horizontal)
    posiciona_navio(tabuleiro, navio_horizontal, N1_LINHA, N1_COL, HORIZONTAL);

    if (ha_sobreposicao(tabuleiro, N2_LINHA, N2_COL, VERTICAL)) {
        printf("Erro: sobreposicao ao posicionar o navio vertical.\n");
        return 1;
    }
    // Posiciona o segundo navio (vertical)
    posiciona_navio(tabuleiro, navio_vertical, N2_LINHA, N2_COL, VERTICAL);

    // 3) Exibe o tabuleiro completo
    exibe_tabuleiro(tabuleiro);

    return 0;
}