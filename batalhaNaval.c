#include <stdio.h>
#include <stdbool.h>

// ========================
// Configurações gerais
// ========================
#define TAM 10
#define TAM_NAVIO 3
#define AGUA 0
#define NAVIO 3
#define HAB  5

// Orientações de navio
typedef enum { HORIZONTAL, VERTICAL, DIAG_PRINCIPAL, DIAG_SECUNDARIA } Orientacao;

// ------------------------
// Coordenadas dos 4 navios
// (índices 0..9)
// ------------------------
#define N1_LINHA 2   // horizontal ↔ a partir de (2,4)
#define N1_COL   4
#define N2_LINHA 6   // vertical ↕   a partir de (6,1)
#define N2_COL   1
#define N3_LINHA 1   // diagonal principal ↘ a partir de (1,1)
#define N3_COL   1
#define N4_LINHA 1   // diagonal secundária ↙ a partir de (1,8)
#define N4_COL   8

// ------------------------
// Habilidades (matrizes 5x5)
// ORIGEM NO TABULEIRO (onde "ancora" a máscara)
// ------------------------
#define HAB_N 5

// Pontos de origem no TABULEIRO para cada habilidade
#define ORIG_CONE_R   4
#define ORIG_CONE_C   5
#define ORIG_CRUZ_R   5
#define ORIG_CRUZ_C   5
#define ORIG_OCTA_R   3
#define ORIG_OCTA_C   3

// Para centralizar/sobrepor, definimos QUAL é a célula-origem DENTRO DA MÁSCARA
// - Cone: origem no TOPO da máscara, coluna do meio -> (0,2)
// - Cruz e Octaedro: origem no CENTRO -> (2,2)
#define MASK_ORIG_CONE_R 0
#define MASK_ORIG_CONE_C 2
#define MASK_ORIG_CENTRO_R 2
#define MASK_ORIG_CENTRO_C 2

// ========================
// Utilidades de NAVIOS
// ========================
bool cabe_no_tabuleiro(int linha, int col, Orientacao o) {
    switch (o) {
        case HORIZONTAL:
            return (linha >= 0 && linha < TAM) && (col >= 0 && col + TAM_NAVIO - 1 < TAM);
        case VERTICAL:
            return (col >= 0 && col < TAM) && (linha >= 0 && linha + TAM_NAVIO - 1 < TAM);
        case DIAG_PRINCIPAL:   // ↘
            return (linha >= 0 && linha + TAM_NAVIO - 1 < TAM) &&
                   (col   >= 0 && col   + TAM_NAVIO - 1 < TAM);
        case DIAG_SECUNDARIA:  // ↙
            return (linha >= 0 && linha + TAM_NAVIO - 1 < TAM) &&
                   (col - (TAM_NAVIO - 1) >= 0 && col < TAM);
        default: return false;
    }
}

static inline void posicao_i(int base_r, int base_c, Orientacao o, int i, int *r, int *c) {
    *r = base_r; *c = base_c;
    if (o == HORIZONTAL)            *c += i;
    else if (o == VERTICAL)         *r += i;
    else if (o == DIAG_PRINCIPAL) { *r += i; *c += i; }
    else if (o == DIAG_SECUNDARIA){ *r += i; *c -= i; }
}

bool ha_sobreposicao(int tab[TAM][TAM], int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r, c; posicao_i(linha, col, o, i, &r, &c);
        if (tab[r][c] != AGUA) return true;
    }
    return false;
}

void posiciona_navio(int tab[TAM][TAM], int vetor[TAM_NAVIO],
                     int linha, int col, Orientacao o) {
    for (int i = 0; i < TAM_NAVIO; i++) {
        int r, c; posicao_i(linha, col, o, i, &r, &c);
        tab[r][c] = vetor[i];
    }
}

// ========================
// Utilidades de HABILIDADES
// ========================

// Aplica uma máscara HAB_N×HAB_N (valores 0/1) no tabuleiro.
// origin_r/origin_c: ponto de origem no TABULEIRO.
// mask_or_r/mask_or_c: qual célula da MÁSCARA corresponde a esse ponto.
// Marca HAB (5) onde mask==1 *sem* cobrir navio (mantém 3).
void aplica_habilidade(int tab[TAM][TAM], int mask[HAB_N][HAB_N],
                       int origin_r, int origin_c,
                       int mask_or_r, int mask_or_c)
{
    for (int i = 0; i < HAB_N; i++) {
        for (int j = 0; j < HAB_N; j++) {
            if (mask[i][j] == 0) continue;

            int r = origin_r + (i - mask_or_r);
            int c = origin_c + (j - mask_or_c);

            // garante ficar dentro do tabuleiro (condicionais)
            if (r < 0 || r >= TAM || c < 0 || c >= TAM) continue;

            // não sobrepõe navio para manter visual (3 tem prioridade)
            if (tab[r][c] == AGUA) tab[r][c] = HAB;
        }
    }
}

void exibe_tabuleiro(int tab[TAM][TAM]) {
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    // 1) Tabuleiro 10x10 com água
    int tab[TAM][TAM];
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            tab[i][j] = AGUA;

    // 2) Quatro navios (3 casas cada)
    int nav_h[TAM_NAVIO]  = {NAVIO, NAVIO, NAVIO};
    int nav_v[TAM_NAVIO]  = {NAVIO, NAVIO, NAVIO};
    int nav_d1[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO};
    int nav_d2[TAM_NAVIO] = {NAVIO, NAVIO, NAVIO};

    // Valida e posiciona
    if (!cabe_no_tabuleiro(N1_LINHA, N1_COL, HORIZONTAL) ||
        ha_sobreposicao(tab, N1_LINHA, N1_COL, HORIZONTAL)) {
        printf("Erro ao posicionar navio horizontal.\n"); return 1;
    }
    posiciona_navio(tab, nav_h,  N1_LINHA, N1_COL, HORIZONTAL);

    if (!cabe_no_tabuleiro(N2_LINHA, N2_COL, VERTICAL) ||
        ha_sobreposicao(tab, N2_LINHA, N2_COL, VERTICAL)) {
        printf("Erro ao posicionar navio vertical.\n"); return 1;
    }
    posiciona_navio(tab, nav_v,  N2_LINHA, N2_COL, VERTICAL);

    if (!cabe_no_tabuleiro(N3_LINHA, N3_COL, DIAG_PRINCIPAL) ||
        ha_sobreposicao(tab, N3_LINHA, N3_COL, DIAG_PRINCIPAL)) {
        printf("Erro ao posicionar navio diagonal principal.\n"); return 1;
    }
    posiciona_navio(tab, nav_d1, N3_LINHA, N3_COL, DIAG_PRINCIPAL);

    if (!cabe_no_tabuleiro(N4_LINHA, N4_COL, DIAG_SECUNDARIA) ||
        ha_sobreposicao(tab, N4_LINHA, N4_COL, DIAG_SECUNDARIA)) {
        printf("Erro ao posicionar navio diagonal secundaria.\n"); return 1;
    }
    posiciona_navio(tab, nav_d2, N4_LINHA, N4_COL, DIAG_SECUNDARIA);

    // 3) Matrizes de HABILIDADE (0 = não afeta, 1 = afeta)
    // --- CONE (origem no topo, coluna central): formato 3 "andares"
    int cone[HAB_N][HAB_N] = {
        {0,0,1,0,0},
        {0,1,1,1,0},
        {1,1,1,1,1},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };
    // --- CRUZ (origem no centro)
    int cruz[HAB_N][HAB_N] = {
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0}
    };
    // --- OCTAEDRO (losango; origem no centro)
    int octa[HAB_N][HAB_N] = {
        {0,0,1,0,0},
        {0,1,1,1,0},
        {1,1,1,1,1},
        {0,1,1,1,0},
        {0,0,1,0,0}
    };

    // 4) Sobrepõe habilidades no tabuleiro (com limites)
    aplica_habilidade(tab, cone, ORIG_CONE_R, ORIG_CONE_C,
                      MASK_ORIG_CONE_R, MASK_ORIG_CONE_C);

    aplica_habilidade(tab, cruz, ORIG_CRUZ_R, ORIG_CRUZ_C,
                      MASK_ORIG_CENTRO_R, MASK_ORIG_CENTRO_C);

    aplica_habilidade(tab, octa, ORIG_OCTA_R, ORIG_OCTA_C,
                      MASK_ORIG_CENTRO_R, MASK_ORIG_CENTRO_C);

    // 5) Exibe resultado final
    exibe_tabuleiro(tab);
    return 0;
}