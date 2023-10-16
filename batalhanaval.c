#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define tabuleiro 10

int tabuleiro[tabuleiro][tabuleiro];
int pontuacaoJogador1 = 0;
int pontuacaoJogador2 = 0;

void iniciaTabuleiro() {
    for (int i = 0; i < tabuleiro; ++i) {
        for (int j = 0; j < tabuleiro; ++j) {
            tabuleiro[i][j] = 0;
        }
    }
}

void imprimirTabuleiro() {
    for (int i = 0; i < tabuleiro; ++i) {
        for (int j = 0; j < tabuleiro; ++j) {
            if (tabuleiro[i][j] == 0) {
                printf(" ");
            } else {
                printf("O");
            }
        }
        printf("\n");
    }
}

void colocarNavio(int linha, int coluna) {
    if (linha >= 0 && linha < tabuleiro && coluna >= 0 && coluna < tabuleiro) {
        tabuleiro[linha][coluna] = 1;
    } else {
        printf("Coordenadas inválidas.\n");
    }
}

int verificarNavioAfundado() {
    for (int i = 0; i < tabuleiro; ++i) {
        for (int j = 0; j < tabuleiro; ++j) {
            if (tabuleiro[i][j] == 1) {
                return 0; // Ainda há pelo menos um navio no tabuleiro
            }
        }
    }
    return 1; // Todos os navios foram afundados
}

void posicionarNaviosAleatoriamente() {
    srand(time(NULL));

    for (int i = 0; i < tabuleiro; ++i) {
        int linha = rand() % tabuleiro;
        int coluna = rand() % tabuleiro;

        colocarNavio(linha, coluna);
    }
}

int main() {
    int vezDoJogador1 = 1;

    iniciaTabuleiro();
    posicionarNaviosAleatoriamente();

    while (!verificarNavioAfundado()) {
        printf("Tabuleiro Atual:\n");
        imprimirTabuleiro();

        int linha, coluna;
        printf("Jogador %d, informe as coordenadas para atirar (linha e coluna): ", vezDoJogador1 ? 1 : 2);
        scanf("%d %d", &linha, &coluna);

        if (linha >= 0 && linha < tabuleiro && coluna >= 0 && coluna < tabuleiro) {
            if (tabuleiro[linha][coluna] == 1) {
                printf("Acerto!\n");
                tabuleiro[linha][coluna] = 0; // Marca o navio como afundado
                if (vezDoJogador1) {
                    pontuacaoJogador1++;
                } else {
                    pontuacaoJogador2++;
                }
            } else {
                printf("Erro! Nenhum navio encontrado nessa posição.\n");
            }

            if (verificarNavioAfundado()) {
                printf("Parabéns, Jogador %d! Você afundou todos os navios do oponente.\n", vezDoJogador1 ? 1 : 2);
                printf("Pontuação:\nJogador 1: %d\nJogador 2: %d\n", pontuacaoJogador1, pontuacaoJogador2);
            }

            vezDoJogador1 = !vezDoJogador1; // Alterna os turnos
        } else {
            printf("Coordenadas inválidas. Tente novamente.\n");
        }
    }

    return 0;
}