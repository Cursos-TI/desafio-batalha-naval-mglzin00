#include <stdio.h>
#include <stdbool.h>

// ========================
// Configurações gerais
// ========================
#define TAM 10
#define TAM_NAVIO 3
#define AGUA 0
#define NAVIO 3

// Orientações possíveis
typedef enum { HORIZONTAL, VERTICAL, DIAG_PRINCIPAL, DIAG_SECUNDARIA } Orientacao;

// ========================
// Coordenadas iniciais (linha, coluna) dos 4 navios
// OBS: índices de 0..9. Ajuste aqui se quiser.
// 1) Navio horizontal (↔) a partir de (2, 4)
#define N1_LINHA 2
#define N1_COL   4

// 2) Navio vertical (↕) a partir de (6, 1)
#define N2_LINHA 6
#define N2_COL   1

// 3) Navio diagonal principal (↘) a partir de (1, 1)
//    anda +1 na linha e +1 na coluna
#define N3_LINHA 1
#define N3_COL   1

// 4) Navio diagonal secundária (↙) a partir de (1, 8)
//    anda +1 na linha e -1 na coluna
#define N4_LINHA 1
#define N4_COL   8

// ========================
// Funções utilitárias
// ========================

// Checa se um navio de tamanho TAM_NAVIO "cabe" a partir de (linha,col) na orientação dada
bool cabe_no_tabuleiro(int linha, int col, Orientacao o) {
    switch (o) {
        case HORIZONTAL:
            return (linha >= 0 && linha < TAM) &&
                   (col >= 0 && (col + TAM_NAVIO - 1) < TAM);
        case VERTICAL:
            return (col >= 0 && col < TAM) &&
                   (linha >= 0 && (linha + TAM_NAVIO - 1) < TAM);
        case DIAG_PRINCIPAL: // ↘ : (r+i, c+i)
            return (linha >= 0 && (linha + TAM_NAVIO - 1) < TAM) &&
                   (col   >= 0 && (col   + TAM_NAVIO - 1) < TAM);
        case DIAG_SECUNDARIA: // ↙ : (r+i, c-i)
            return (linha >= 0 && (linha + TAM_NAVIO - 1) < TAM) &&
                   (col   - (TAM_NAVIO - 1) >= 0 && col < TAM);
        default:
            return false;
    }
}

// Retorna o par (r,c) da i-ésima peça do navio
static inline void posicao_i(int base_r, int base_c, Orientacao o, int i, int *r, int *c) {
    *r = base_r;
    *c = base_c;
    if (o == HORIZONTAL)           *c += i;
    else if (o == VERTICAL)        *r += i;
    else if (o == DIAG_PRINCIPAL) { *r += i; *c += i; }
    else if (o == DIAG_SECUNDARIA){ *r += i; *c -= i; }
}

// Verifica sobreposição com o que já está no tabuleiro
bool ha_sobreposicao(int tab[TAM][TAM], int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r, c; posicao_i(linha, col, o, i, &r, &c);
        if (tab[r][c] != AGUA) return true;
    }
    return false;
}

// Copia os valores do vetor (todos 3) para o tabuleiro nas posições do navio
void posiciona_navio(int tab[TAM][TAM], int vetor[TAM_NAVIO],
                     int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r, c; posicao_i(linha, col, o, i, &r, &c);
        tab[r][c] = vetor[i];
    }
}

// Exibe o tabuleiro 10x10
void exibe_tabuleiro(int tab[TAM][TAM]) {
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    // 1) Matriz 10x10 iniciada em 0 (água)
    int tab[TAM][TAM];
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            tab[i][j] = AGUA;

    // 2) Vetores unidimensionais (cada posição vale 3)
    int nav_h[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO};
    int nav_v[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO};
    int nav_d1[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO}; // diagonal principal ↘
    int nav_d2[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO}; // diagonal secundária ↙

    // 3) Validações de limites
    if (!cabe_no_tabuleiro(N1_LINHA, N1_COL, HORIZONTAL)) {
        printf("Erro: navio horizontal fora dos limites.\n"); return 1;
    }
    if (!cabe_no_tabuleiro(N2_LINHA, N2_COL, VERTICAL)) {
        printf("Erro: navio vertical fora dos limites.\n"); return 1;
    }
    if (!cabe_no_tabuleiro(N3_LINHA, N3_COL, DIAG_PRINCIPAL)) {
        printf("Erro: navio diagonal principal fora dos limites.\n"); return 1;
    }
    if (!cabe_no_tabuleiro(N4_LINHA, N4_COL, DIAG_SECUNDARIA)) {
        printf("Erro: navio diagonal secundaria fora dos limites.\n"); return 1;
    }

    // 4) Validação de sobreposição e posicionamento
    if (ha_sobreposicao(tab, N1_LINHA, N1_COL, HORIZONTAL)) {
        printf("Erro: sobreposicao ao posicionar navio horizontal.\n"); return 1;
    }
    posiciona_navio(tab, nav_h, N1_LINHA, N1_COL, HORIZONTAL);

    if (ha_sobreposicao(tab, N2_LINHA, N2_COL, VERTICAL)) {
        printf("Erro: sobreposicao ao posicionar navio vertical.\n"); return 1;
    }
    posiciona_navio(tab, nav_v, N2_LINHA, N2_COL, VERTICAL);

    if (ha_sobreposicao(tab, N3_LINHA, N3_COL, DIAG_PRINCIPAL)) {
        printf("Erro: sobreposicao ao posicionar navio diagonal principal.\n"); return 1;
    }
    posiciona_navio(tab, nav_d1, N3_LINHA, N3_COL, DIAG_PRINCIPAL);

    if (ha_sobreposicao(tab, N4_LINHA, N4_COL, DIAG_SECUNDARIA)) {
        printf("Erro: sobreposicao ao posicionar navio diagonal secundaria.\n"); return 1;
    }
    posiciona_navio(tab, nav_d2, N4_LINHA, N4_COL, DIAG_SECUNDARIA);

    // 5) Exibe o tabuleiro completo
    exibe_tabuleiro(tab);
    return 0;
}