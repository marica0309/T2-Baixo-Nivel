#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define LINHAS 6
#define COLUNAS 7

int tabuleiro_virtual[LINHAS][COLUNAS] = {0};

typedef enum {
    MENU,
    JOGO_PVP,
    JOGO_IA,
    FINAL
} EstadoJogo;

EstadoJogo estado_atual = MENU;
int jogador_vencedor = 0; // Agora está no escopo global

bool checar_vitoria(int jogador) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro_virtual[i][j] != jogador) continue;
            if (j <= COLUNAS - 4 && tabuleiro_virtual[i][j+1] == jogador &&
                tabuleiro_virtual[i][j+2] == jogador &&
                tabuleiro_virtual[i][j+3] == jogador) return true;
            if (i <= LINHAS - 4 && tabuleiro_virtual[i+1][j] == jogador &&
                tabuleiro_virtual[i+2][j] == jogador &&
                tabuleiro_virtual[i+3][j] == jogador) return true;
            if (i <= LINHAS - 4 && j <= COLUNAS - 4 && tabuleiro_virtual[i+1][j+1] == jogador &&
                tabuleiro_virtual[i+2][j+2] == jogador &&
                tabuleiro_virtual[i+3][j+3] == jogador) return true;
            if (i >= 3 && j <= COLUNAS - 4 && tabuleiro_virtual[i-1][j+1] == jogador &&
                tabuleiro_virtual[i-2][j+2] == jogador &&
                tabuleiro_virtual[i-3][j+3] == jogador) return true;
        }
    }
    return false;
}

bool checar_empate() {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro_virtual[i][j] == 0)
                return false;
        }
    }
    return true;
}

int encontrar_linha_disponivel(int coluna) {
    for (int i = LINHAS - 1; i >= 0; i--) {
        if (tabuleiro_virtual[i][coluna] == 0){
            return i;
        }
    }
    return -1;
}

void tratar_clique_pvp(int mouse_x, int mouse_y, SDL_Rect quad1, int* jogador_atual, EstadoJogo* estado_atual) {
    if (mouse_x >= quad1.x && mouse_x < quad1.x + quad1.w &&
        mouse_y >= quad1.y && mouse_y < quad1.y + quad1.h) {

        int cellWidth = quad1.w / COLUNAS;
        int coluna = (mouse_x - quad1.x) / cellWidth;
        int linha_disp = encontrar_linha_disponivel(coluna);

        if (linha_disp != -1) {
            tabuleiro_virtual[linha_disp][coluna] = *jogador_atual;

            if (checar_vitoria(*jogador_atual)) {
                *estado_atual = FINAL;
                jogador_vencedor = *jogador_atual;
            } else if (checar_empate()) {
                printf("Empate! Reiniciando o jogo...\n");
                *estado_atual = MENU;
                *jogador_atual = 1;
                for (int i = 0; i < LINHAS; i++)
                    for (int j = 0; j < COLUNAS; j++)
                        tabuleiro_virtual[i][j] = 0;
            } else {
                *jogador_atual = 3 - *jogador_atual;
            }
        } else {
            printf("Coluna %d cheia! Clique ignorado.\n", coluna);
        }
    }
}

void tratar_clique_ia(int mouse_x, int mouse_y, SDL_Rect quad1, int* jogador_atual, EstadoJogo* estado_atual) {
    if (mouse_x >= quad1.x && mouse_x < quad1.x + quad1.w &&
        mouse_y >= quad1.y && mouse_y < quad1.y + quad1.h) {

        int cellWidth = quad1.w / COLUNAS;
        int coluna = (mouse_x - quad1.x) / cellWidth;
        int linha_disp = encontrar_linha_disponivel(coluna);

        if (linha_disp != -1) {
            tabuleiro_virtual[linha_disp][coluna] = *jogador_atual;

            if (checar_vitoria(*jogador_atual)) {
                *estado_atual = FINAL;
                jogador_vencedor = *jogador_atual;
            } else if (checar_empate()) {
                printf("Empate! Reiniciando o jogo...\n");
                *estado_atual = MENU;
                *jogador_atual = 1;
                for (int i = 0; i < LINHAS; i++)
                    for (int j = 0; j < COLUNAS; j++)
                        tabuleiro_virtual[i][j] = 0;
            } else {
                *jogador_atual = 3 - *jogador_atual;
            }
        }
    }
}

int main(int argc, char** argv) {
    float escala = 0.6f;
    bool ignorar_primeiro_clique = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    int largura_janela = (int)(1500 * escala);
    int altura_janela = (int)(1024 * escala);

    SDL_Window* window = SDL_CreateWindow("Connect Four", 100, 100, largura_janela, altura_janela, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture* menu_img = IMG_LoadTexture(renderer, "imagens/menu.png");
    SDL_Texture* tabuleiro = IMG_LoadTexture(renderer, "imagens/jogo_tabuleiro.png");
    SDL_Texture* ficha_vermelha = IMG_LoadTexture(renderer, "imagens/ficha_vermelha.png");
    SDL_Texture* ficha_amarela = IMG_LoadTexture(renderer, "imagens/ficha_amarela.png");
    SDL_Texture* vencedor1 = IMG_LoadTexture(renderer, "imagens/vencedor1.png");
    SDL_Texture* vencedor2 = IMG_LoadTexture(renderer, "imagens/vencedor2.png");

    SDL_Rect quad1 = { (int)(200 * escala), (int)(150 * escala),
                       (int)(1108 * escala), (int)(887 * escala) };

    int jogador_atual = 1;
    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            if (estado_atual == MENU && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                if (x >= 299 && x <= 585 && y >= 282 && y <= 335) {
                    estado_atual = JOGO_IA;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    for (int i = 0; i < LINHAS; i++)
                        for (int j = 0; j < COLUNAS; j++)
                            tabuleiro_virtual[i][j] = 0;
                } else if (x >= 301 && x <= 585 && y >= 403 && y <= 449) {
                    estado_atual = JOGO_PVP;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    for (int i = 0; i < LINHAS; i++)
                        for (int j = 0; j < COLUNAS; j++)
                            tabuleiro_virtual[i][j] = 0;
                } else if (x >= 294 && x <= 585 && y >= 516 && y <= 566) {
                    running = 0;
                }
            }

            if ((estado_atual == JOGO_PVP || estado_atual == JOGO_IA) &&
                event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {

                if (ignorar_primeiro_clique) {
                    ignorar_primeiro_clique = false;
                    continue;
                }

                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (estado_atual == JOGO_PVP) {
                    tratar_clique_pvp(mouse_x, mouse_y, quad1, &jogador_atual, &estado_atual);
                } else if (estado_atual == JOGO_IA) {
                    tratar_clique_ia(mouse_x, mouse_y, quad1, &jogador_atual, &estado_atual);
                }
            }

            if (estado_atual == FINAL && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                if (x >= 278 && x <= 599 && y >= 448 && y <= 495) {
                    estado_atual = MENU;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    for (int i = 0; i < LINHAS; i++)
                        for (int j = 0; j < COLUNAS; j++)
                            tabuleiro_virtual[i][j] = 0;
                } else if (x >= 281 && x <= 601 && y >= 452 && y <= 498) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (estado_atual == MENU) {
            SDL_RenderCopy(renderer, menu_img, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        if (estado_atual == JOGO_PVP) {
            SDL_SetRenderDrawColor(renderer, 200, 255, 200, 255);
            SDL_RenderClear(renderer);
        } else if (estado_atual == JOGO_IA) {
            SDL_SetRenderDrawColor(renderer, 255, 230, 200, 255);
            SDL_RenderClear(renderer);
        }

        SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1);

        SDL_Rect local_vermelha = { (int)(100 * escala), (int)(200 * escala),
                                    (int)(132 * escala), (int)(132 * escala) };
        SDL_RenderCopy(renderer, ficha_vermelha, NULL, &local_vermelha);

        SDL_Rect local_amarela = { (int)(100 * escala), (int)(400 * escala),
                                   (int)(132 * escala), (int)(132 * escala) };
        SDL_RenderCopy(renderer, ficha_amarela, NULL, &local_amarela);

        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                if (tabuleiro_virtual[i][j] == 0){
                    continue;
                } 
                SDL_Texture* ficha = (tabuleiro_virtual[i][j] == 1) ? ficha_vermelha : ficha_amarela;

                int cellWidth = quad1.w / COLUNAS;
                int cellHeight = quad1.h / LINHAS;
                int fichaLargura = (int)(cellWidth * 0.8);
                int fichaAltura = (int)(cellHeight * 0.8);

                SDL_Rect destino = {
                    quad1.x + j * cellWidth + (cellWidth - fichaLargura) / 2,
                    quad1.y + i * cellHeight + (cellHeight - fichaAltura) / 2,
                    fichaLargura,
                    fichaAltura
                };

                SDL_RenderCopy(renderer, ficha, NULL, &destino);
            }
        }

        if (estado_atual == FINAL) {
            if (jogador_vencedor == 1) {
                SDL_RenderCopy(renderer, vencedor1, NULL, NULL);
            } else if (jogador_vencedor == 2) {
                SDL_RenderCopy(renderer, vencedor2, NULL, NULL);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_DestroyTexture(menu_img);
    SDL_DestroyTexture(ficha_amarela);
    SDL_DestroyTexture(ficha_vermelha);
    SDL_DestroyTexture(tabuleiro);
    SDL_DestroyTexture(vencedor1);
    SDL_DestroyTexture(vencedor2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
