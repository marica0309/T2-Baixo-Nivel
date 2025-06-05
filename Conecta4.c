#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LINHAS 6
#define COLUNAS 7
#define MAX_ANIMACOES 10

typedef struct {
    int coluna;
    int linha_final;
    float y_atual;
    int jogador;
    bool ativa;
} AnimacaoPeca;

AnimacaoPeca animacoes[MAX_ANIMACOES] = {0};
int tabuleiro_virtual[LINHAS][COLUNAS] = {0};

typedef enum {
    MENU,
    JOGO_PVP,
    JOGO_IA,
    FINAL
} EstadoJogo;

EstadoJogo estado_atual = MENU;
int jogador_vencedor = 0;

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
    for (int i = 0; i < LINHAS; i++)
        for (int j = 0; j < COLUNAS; j++)
            if (tabuleiro_virtual[i][j] == 0) return false;
    return true;
}

int encontrar_linha_disponivel(int coluna) {
    for (int i = LINHAS - 1; i >= 0; i--) {
        if (tabuleiro_virtual[i][coluna] == 0) return i;
    }
    return -1;
}

int escolher_coluna_ia() {
    int colunas_validas[COLUNAS];
    int num_validas = 0;
    for (int j = 0; j < COLUNAS; j++) {
        if (encontrar_linha_disponivel(j) != -1) {
            colunas_validas[num_validas++] = j;
        }
    }
    if (num_validas == 0) return -1;
    return colunas_validas[rand() % num_validas];
}

void iniciar_animacao(int coluna, int linha, int jogador, SDL_Rect quad1) {
    for (int i = 0; i < MAX_ANIMACOES; i++) {
        if (!animacoes[i].ativa) {
            animacoes[i].coluna = coluna;
            animacoes[i].linha_final = linha;
            animacoes[i].y_atual = quad1.y - 100;
            animacoes[i].jogador = jogador;
            animacoes[i].ativa = true;
            break;
        }
    }
}

void tratar_clique(int mouse_x, int mouse_y, SDL_Rect quad1, int* jogador_atual) {
    if (mouse_x >= quad1.x && mouse_x < quad1.x + quad1.w &&
        mouse_y >= quad1.y && mouse_y < quad1.y + quad1.h) {
        int cellWidth = quad1.w / COLUNAS;
        int coluna = (mouse_x - quad1.x) / cellWidth;
        int linha_disp = encontrar_linha_disponivel(coluna);

        if (linha_disp != -1) {
            iniciar_animacao(coluna, linha_disp, *jogador_atual, quad1);
        }
    }
}

int main(int argc, char** argv) {
    bool ignorar_primeiro_clique = false;
    srand((unsigned int)time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    int largura_janela = (int)(900);
    int altura_janela = (int)(614.4);
    SDL_Window* window = SDL_CreateWindow("Connect Four", 100, 100, largura_janela, altura_janela, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture* menu_img = IMG_LoadTexture(renderer, "imagens/menu.png");
    SDL_Texture* tabuleiro = IMG_LoadTexture(renderer, "imagens1/jogo_tabuleiro.png");
    SDL_Texture* ficha_vermelha = IMG_LoadTexture(renderer, "imagens1/ficha_vermelha.png");
    SDL_Texture* ficha_amarela = IMG_LoadTexture(renderer, "imagens1/ficha_amarela.png");
    SDL_Texture* vencedor1 = IMG_LoadTexture(renderer, "imagens/vencedor1.png");
    SDL_Texture* vencedor2 = IMG_LoadTexture(renderer, "imagens/vencedor2.png");

    SDL_Rect quad1 = {200, 150 , 447 , 358};

    int jogador_atual = 1;
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (estado_atual == MENU && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= 299 && x <= 585 && y >= 282 && y <= 335) {
                    estado_atual = JOGO_IA;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                } else if (x >= 301 && x <= 585 && y >= 403 && y <= 449) {
                    estado_atual = JOGO_PVP;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                } else if (x >= 294 && x <= 585 && y >= 516 && y <= 566) {
                    running = false;
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
                tratar_clique(mouse_x, mouse_y, quad1, &jogador_atual);
            }

            if (estado_atual == FINAL && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= 276 && x <= 600 && y >= 368 && y <= 420) {
                    estado_atual = MENU;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                } else if (x >= 276 && x <= 600 && y >= 448 && y <= 506) {
                    running = false;
                }
            }
        }
        if (estado_atual == JOGO_IA && jogador_atual == 2) {
            bool animando = false;
            for (int i = 0; i < MAX_ANIMACOES; i++) {
                if (animacoes[i].ativa) {
                    animando = true;
                    break;
                }
            }

            static Uint32 tempo_espera = 0;

            if (!animando) {
                if (tempo_espera == 0) tempo_espera = SDL_GetTicks();  

                if (SDL_GetTicks() - tempo_espera > 500) {  
                    int coluna_ia = escolher_coluna_ia();
                    int linha_disp = encontrar_linha_disponivel(coluna_ia);
                    if (linha_disp != -1) {
                        iniciar_animacao(coluna_ia, linha_disp, 2, quad1);
                    }
                    tempo_espera = 0;  
                }
            } else {
                tempo_espera = 0;  
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

        if (estado_atual == JOGO_PVP) SDL_SetRenderDrawColor(renderer, 200, 255, 200, 255);
        if (estado_atual == JOGO_IA)  SDL_SetRenderDrawColor(renderer, 255, 230, 200, 255);

        SDL_RenderClear(renderer);
        int cellWidth = quad1.w / COLUNAS;
        int cellHeight = quad1.h / LINHAS;

        for (int i = 0; i < MAX_ANIMACOES; i++) {
            if (animacoes[i].ativa) {
                float destinoY = quad1.y + animacoes[i].linha_final * cellHeight - 18; 
                if (animacoes[i].y_atual < destinoY) {
                    animacoes[i].y_atual += 10;
                } else {
                    animacoes[i].y_atual = destinoY;
                    animacoes[i].ativa = false;
                    tabuleiro_virtual[animacoes[i].linha_final][animacoes[i].coluna] = animacoes[i].jogador;

                    if (checar_vitoria(animacoes[i].jogador)) {
                        estado_atual = FINAL;
                        jogador_vencedor = animacoes[i].jogador;
                    } else if (checar_empate()) {
                        estado_atual = MENU;
                        jogador_atual = 1;
                        memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                    } else {
                        jogador_atual = 3 - animacoes[i].jogador;
                    }
                }

                SDL_Texture* ficha = (animacoes[i].jogador == 1) ? ficha_vermelha : ficha_amarela;
                SDL_Rect destino = {
                    quad1.x + animacoes[i].coluna * cellWidth, 
                    (int)animacoes[i].y_atual,
                    cellWidth,
                    cellHeight
                };
                SDL_RenderCopy(renderer, ficha, NULL, &destino);
            }
        }

        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                if (tabuleiro_virtual[i][j] == 0) continue;
                SDL_Texture* ficha = (tabuleiro_virtual[i][j] == 1) ? ficha_vermelha : ficha_amarela;
                SDL_Rect destino = {
                    quad1.x + j * cellWidth,                  
                    quad1.y + i * cellHeight - 18,           
                    cellWidth,
                    cellHeight
                };
                SDL_RenderCopy(renderer, ficha, NULL, &destino);
            }
        }

        SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1);

        if (estado_atual == FINAL) {
            SDL_RenderCopy(renderer, (jogador_vencedor == 1 ? vencedor1 : vencedor2), NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyTexture(menu_img);
    SDL_DestroyTexture(tabuleiro);
    SDL_DestroyTexture(ficha_vermelha);
    SDL_DestroyTexture(ficha_amarela);
    SDL_DestroyTexture(vencedor1);
    SDL_DestroyTexture(vencedor2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}