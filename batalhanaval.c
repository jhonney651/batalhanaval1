#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define TAMANHO_TABULEIRO 10
#define NUM_NAVIOS 5

typedef struct {
    int x;
    int y;
} Coordenada;

typedef struct {
    Coordenada posicao;
    int atingido;
} Navio;

typedef struct {
    Navio navios[NUM_NAVIOS];
    char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
} Jogador;

typedef enum {
    CORVETA,
    SUBMARINO,
    FRAGATA,
    DESTROYER,
    CRUZADOR,
    PORTA_AVIOES,
    NUM_TIPOS_NAVIO
} TipoNavio;

typedef struct {
    char simbolo;
    int tamanho;
    char nome[20];
} InfoNavio;

InfoNavio tiposNavio[NUM_TIPOS_NAVIO] = {
    {'C', 2, "Corveta"},
    {'S', 3, "Submarino"},
    {'F', 3, "Fragata"},
    {'D', 4, "Destroyer"},
    {'R', 5, "Cruzador"},
    {'A', 6, "Porta-Aviões"}
};

void inicializarTabuleiro(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void exibirTabuleiro(WINDOW *win, char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
Coordenada obterCoordenadaNavio(int indiceNavio, TipoNavio tipoNavio, int orientacao);
int eValidaPosicaoNavio(Jogador *jogador, Coordenada inicio, TipoNavio tipoNavio, int orientacao);
void posicionarNaviosManualmente(Jogador *jogador);
Coordenada obterCoordenadaTiro(WINDOW *win);
int eFimDeJogo(Jogador *jogador);
void realizarTurno(Jogador *atacante, Jogador *defensor);

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    Jogador jogador1, jogador2;
    inicializarTabuleiro(jogador1.tabuleiro);
    inicializarTabuleiro(jogador2.tabuleiro);

    printw("Jogador 1, posicione seus navios:\n");
    posicionarNaviosManualmente(&jogador1);

    clear();

    printw("Jogador 2, posicione seus navios:\n");
    posicionarNaviosManualmente(&jogador2);

    Coordenada historicoTiros[TAMANHO_TABULEIRO * TAMANHO_TABULEIRO * 2];

    int jogadorAtual = 1;
    while (!eFimDeJogo(&jogador1) && !eFimDeJogo(&jogador2)) {
        clear();

        printw("Tabuleiro do Jogador 1:\n");
        exibirTabuleiro(stdscr, jogador1.tabuleiro);

        printw("\nTabuleiro do Jogador 2:\n");
        exibirTabuleiro(stdscr, jogador2.tabuleiro);

        printw("\nJogador %d, é sua vez:\n", jogadorAtual);
        realizarTurno(jogadorAtual == 1 ? &jogador1 : &jogador2, jogadorAtual == 1 ? &jogador2 : &jogador1);

        jogadorAtual = 3 - jogadorAtual;
    }

    clear();
    if (eFimDeJogo(&jogador1)) {
        printw("Jogador 1 venceu!\n");
    } else {
        printw("Jogador 2 venceu!\n");
    }

    refresh();
    getch();

    endwin();
    return 0;
}

void inicializarTabuleiro(char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; ++i) {
        for (int j = 0; j < TAMANHO_TABULEIRO; ++j) {
            tabuleiro[i][j] = ' ';
        }
    }
}

void exibirTabuleiro(WINDOW *win, char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; ++i) {
        for (int j = 0; j < TAMANHO_TABULEIRO; ++j) {
            if (tabuleiro[i][j] == ' ') {
                wprintw(win, "\u25A1 ");
            } else if (tabuleiro[i][j] == 'O') {
                wprintw(win, "\u25A0 ");
            } else {
                wprintw(win, "%c ", tabuleiro[i][j]);
            }
        }
        wprintw(win, "\n");
    }
}

Coordenada obterCoordenadaNavio(int indiceNavio, TipoNavio tipoNavio, int orientacao) {
    Coordenada coordenadaNavio;
    printw("Posicione o %s (%c) de tamanho %d %s (x y): ",
           tiposNavio[tipoNavio].nome, tiposNavio[tipoNavio].simbolo,
           tiposNavio[tipoNavio].tamanho,
           orientacao == 0 ? "horizontal" : "vertical");
    scanw("%d %d", &coordenadaNavio.x, &coordenadaNavio.y);
    return coordenadaNavio;
}

int eValidaPosicaoNavio(Jogador *jogador, Coordenada inicio, TipoNavio tipoNavio, int orientacao) {
    int fimX = inicio.x + (orientacao == 0 ? tiposNavio[tipoNavio].tamanho - 1 : 0);
    int fimY = inicio.y + (orientacao == 1 ? tiposNavio[tipoNavio].tamanho - 1 : 0);

    if (fimX >= TAMANHO_TABULEIRO || fimY >= TAMANHO_TABULEIRO) {
        return 0;
    }

    for (int i = inicio.x; i <= fimX; ++i) {
        for (int j = inicio.y; j <= fimY; ++j) {
            if (jogador->tabuleiro[i][j] != ' ') {
                return 0;
            }
        }
    }

    return 1;
}

void posicionarNaviosManualmente(Jogador *jogador) {
    for (int i = 0; i < NUM_NAVIOS; ++i) {
        TipoNavio tipoAtual = (TipoNavio)i;
        int orientacao;
        do {
            orientacao = -1;
            printw("Escolha a orientação para o %s (%c): 0 - horizontal, 1 - vertical: ",
                   tiposNavio[tipoAtual].nome, tiposNavio[tipoAtual].simbolo);
            scanw("%d", &orientacao);
        } while (orientacao < 0 || orientacao > 1);

        Coordenada inicio;
        int ePosicaoValida;
        do {
            inicio = obterCoordenadaNavio(i, tipoAtual, orientacao);
            ePosicaoValida = eValidaPosicaoNavio(jogador, inicio, tipoAtual, orientacao);
            if (!ePosicaoValida) {
                printw("Posição inválida. Tente novamente.\n");
            }
        } while (!ePosicaoValida);

        int fimX = inicio.x + (orientacao == 0 ? tiposNavio[tipoAtual].tamanho - 1 : 0);
        int fimY = inicio.y + (orientacao == 1 ? tiposNavio[tipoAtual].tamanho - 1 : 0);

        for (int j = 0; j < tiposNavio[tipoAtual].tamanho; ++j) {
            jogador->navios[i].posicao.x = inicio.x + (orientacao == 0 ? j : 0);
            jogador->navios[i].posicao.y = inicio.y + (orientacao == 1 ? j : 0);
            jogador->tabuleiro[jogador->navios[i].posicao.x][jogador->navios[i].posicao.y] =
                tiposNavio[tipoAtual].simbolo;
        }
    }
}

Coordenada obterCoordenadaTiro(WINDOW *win) {
    Coordenada tiro;
    printw("Informe as coordenadas do tiro (x y): ");
    scanw("%d %d", &tiro.x, &tiro.y);
    return tiro;
}

int eFimDeJogo(Jogador *jogador) {
    for (int i = 0; i < NUM_NAVIOS; ++i) {
        if (!jogador->navios[i].atingido) {
            return 0;
        }
    }
    return 1;
}

void realizarTurno(Jogador *atacante, Jogador *defensor) {
    Coordenada tiro = obterCoordenadaTiro(stdscr);

    for (int i = 0; i < NUM_NAVIOS; ++i) {
        if (!defensor->navios[i].atingido &&
            defensor->navios[i].posicao.x == tiro.x &&
            defensor->navios[i].posicao.y == tiro.y) {
            printw("Acerto!\n");
            defensor->navios[i].atingido = 1;
            atacante->tabuleiro[tiro.x][tiro.y] = 'X';
            return;
        }
    }

    printw("Água!\n");
    atacante->tabuleiro[tiro.x][tiro.y] = 'O';
}
