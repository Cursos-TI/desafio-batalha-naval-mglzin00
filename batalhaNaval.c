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

// Habilitar/Desabilitar cores ANSI (1 = ligado, 0 = desligado)
#define USE_ANSI_COLORS 1

#if USE_ANSI_COLORS
  #define C_RESET  "\x1b[0m"
  #define C_WATER  "\x1b[37m"  // cinza claro
  #define C_SHIP   "\x1b[36m"  // ciano
  #define C_SKILL  "\x1b[33m"  // amarelo
  #define C_HDR    "\x1b[90m"  // cinza escuro (índices/bordas)
#else
  #define C_RESET  ""
  #define C_WATER  ""
  #define C_SHIP   ""
  #define C_SKILL  ""
  #define C_HDR    ""
#endif

// Orientações de navio
typedef enum { HORIZONTAL, VERTICAL, DIAG_PRINCIPAL, DIAG_SECUNDARIA } Orientacao;

// ------------------------
// Coordenadas dos 4 navios (índices 0..9)
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
// Habilidades (máscaras 5x5)
// ------------------------
#define HAB_N 5

// Pontos de origem no TABULEIRO
#define ORIG_CONE_R 4
#define ORIG_CONE_C 5
#define ORIG_CRUZ_R 5
#define ORIG_CRUZ_C 5
#define ORIG_OCTA_R 3
#define ORIG_OCTA_C 3

// Ponto de origem DENTRO DAS MÁSCARAS
#define MASK_ORIG_CONE_R   0  // topo do cone
#define MASK_ORIG_CONE_C   2  // coluna do meio
#define MASK_ORIG_CENTRO_R 2  // centro (2,2) p/ cruz e octaedro
#define MASK_ORIG_CENTRO_C 2

// ========================
// NAVIOS
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
// HABILIDADES
// ========================
void aplica_habilidade(int tab[TAM][TAM], int mask[HAB_N][HAB_N],
                       int origin_r, int origin_c,
                       int mask_or_r, int mask_or_c)
{
    for (int i = 0; i < HAB_N; i++) {
        for (int j = 0; j < HAB_N; j++) {
            if (mask[i][j] == 0) continue;

            int r = origin_r + (i - mask_or_r);
            int c = origin_c + (j - mask_or_c);
            if (r < 0 || r >= TAM || c < 0 || c >= TAM) continue;

            // Não cobre navio (3 tem prioridade visual)
            if (tab[r][c] == AGUA) tab[r][c] = HAB;
        }
    }
}

// Impressão bonita com bordas, índices e legenda
void exibe_tabuleiro(int tab[TAM][TAM]) {
    // Cabeçalho de colunas
    printf("%s    ", C_HDR);
    for (int j = 0; j < TAM; j++) printf("%d ", j);
    printf("%s\n", C_RESET);

    // Linha superior
    printf("%s   ┌", C_HDR);
    for (int j = 0; j < TAM*2 - 1; j++) printf("─");
    printf("┐%s\n", C_RESET);

    for (int i = 0; i < TAM; i++) {
        // Número da linha
        printf("%s%2d %s│%s ", C_HDR, i, C_HDR, C_RESET);
        for (int j = 0; j < TAM; j++) {
            int v = tab[i][j];
            if (v == NAVIO)      printf("%s%d%s", C_SHIP, v, C_RESET);
            else if (v == HAB)   printf("%s%d%s", C_SKILL, v, C_RESET);
            else                 printf("%s%d%s", C_WATER, v, C_RESET);
            if (j != TAM-1) printf(" ");
        }
        printf("%s │%s\n", C_HDR, C_RESET);
    }

    // Linha inferior
    printf("%s   └", C_HDR);
    for (int j = 0; j < TAM*2 - 1; j++) printf("─");
    printf("┘%s\n", C_RESET);

    // Legenda
    printf("%sLegenda:%s ", C_HDR, C_RESET);
    printf("%s0%s=água  ",   C_WATER, C_RESET);
    printf("%s3%s=navio  ",  C_SHIP,  C_RESET);
    printf("%s5%s=habilidade\n", C_SKILL, C_RESET);
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

    // 3) Matrizes de habilidade (0/1)
    int cone[HAB_N][HAB_N] = {      // origem no TOPO (0,2)
        {0,0,1,0,0},
        {0,1,1,1,0},
        {1,1,1,1,1},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };
    int cruz[HAB_N][HAB_N] = {      // origem no CENTRO (2,2)
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0}
    };
    int octa[HAB_N][HAB_N] = {      // origem no CENTRO (2,2)
        {0,0,1,0,0},
        {0,1,1,1,0},
        {1,1,1,1,1},
        {0,1,1,1,0},
        {0,0,1,0,0}
    };

    // 4) Aplica habilidades (com limites, sem cobrir navio)
    aplica_habilidade(tab, cone, ORIG_CONE_R, ORIG_CONE_C,
                      MASK_ORIG_CONE_R, MASK_ORIG_CONE_C);
    aplica_habilidade(tab, cruz, ORIG_CRUZ_R, ORIG_CRUZ_C,
                      MASK_ORIG_CENTRO_R, MASK_ORIG_CENTRO_C);
    aplica_habilidade(tab, octa, ORIG_OCTA_R, ORIG_OCTA_C,
                      MASK_ORIG_CENTRO_R, MASK_ORIG_CENTRO_C);

    // 5) Exibe
    exibe_tabuleiro(tab);
    return 0;
}