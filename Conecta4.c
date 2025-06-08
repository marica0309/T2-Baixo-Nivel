/*
    Jogo Connect Four (Lig4) utilizando SDL2.

    Este código implementa o famoso jogo de tabuleiro "Connect Four" com interface gráfica,
    permitindo jogar contra outro jogador (PvP) ou contra uma IA simples (PvE).
    Utiliza a biblioteca SDL2 para gráficos e eventos.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LINHAS 6     // Número de linhas do tabuleiro
#define COLUNAS 7    // Número de colunas do tabuleiro
#define MAX_ANIMACOES 10 // Número máximo de animações simultâneas de peças caindo

// Coordenadas dos centros das casas do tabuleiro, conforme o layout da imagem de fundo
const int centros_x[COLUNAS] = {257, 311, 365, 419, 473, 527, 581};
const int centros_y[LINHAS]  = {188, 242, 296, 350, 404, 458};
const int raio_ficha = 25; // Raio da ficha, usado para desenhar as peças

// Estrutura que representa uma animação de peça caindo
typedef struct {
    int coluna;         // Coluna da peça
    int linha_final;    // Linha onde a peça irá parar
    float y_atual;      // Posição vertical atual da peça (para animação)
    int jogador;        // Jogador dono da peça (1 ou 2)
    bool ativa;         // Se a animação está ativa
} AnimacaoPeca;

// Vetor global de animações de peças caindo
AnimacaoPeca animacoes[MAX_ANIMACOES] = {0};

// Tabuleiro virtual: 0 = vazio, 1 = jogador 1, 2 = jogador 2
int tabuleiro_virtual[LINHAS][COLUNAS] = {0};

// Enumeração para os possíveis estados do jogo (tela atual)
typedef enum {
    MENU,       // Tela de menu inicial
    JOGO_PVP,   // Jogo jogador vs jogador
    JOGO_IA,    // Jogo jogador vs IA
    FINAL       // Tela final (vencedor)
} EstadoJogo;

EstadoJogo estado_atual = MENU; // Estado atual do jogo
int jogador_vencedor = 0;       // Armazena o vencedor da partida (1 ou 2)

/*
    Função para checar se um jogador venceu o jogo.
    Verifica todas as posições do tabuleiro para encontrar 4 peças consecutivas
    do mesmo jogador, nas direções horizontal, vertical e diagonais.
*/
bool checar_vitoria(int jogador) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // Só avalia se a posição pertence ao jogador
            if (tabuleiro_virtual[i][j] != jogador) continue;
            // Checa horizontal para a direita
            if (j <= COLUNAS - 4 && tabuleiro_virtual[i][j+1] == jogador &&
                tabuleiro_virtual[i][j+2] == jogador &&
                tabuleiro_virtual[i][j+3] == jogador) return true;
            // Checa vertical para baixo
            if (i <= LINHAS - 4 && tabuleiro_virtual[i+1][j] == jogador &&
                tabuleiro_virtual[i+2][j] == jogador &&
                tabuleiro_virtual[i+3][j] == jogador) return true;
            // Checa diagonal principal (baixo-direita)
            if (i <= LINHAS - 4 && j <= COLUNAS - 4 && tabuleiro_virtual[i+1][j+1] == jogador &&
                tabuleiro_virtual[i+2][j+2] == jogador &&
                tabuleiro_virtual[i+3][j+3] == jogador) return true;
            // Checa diagonal secundária (cima-direita)
            if (i >= 3 && j <= COLUNAS - 4 && tabuleiro_virtual[i-1][j+1] == jogador &&
                tabuleiro_virtual[i-2][j+2] == jogador &&
                tabuleiro_virtual[i-3][j+3] == jogador) return true;
        }
    }
    return false;
}

/*
    Função que verifica se houve empate.
    Retorna true se não houver mais espaços vazios no tabuleiro.
*/
bool checar_empate() {
    for (int i = 0; i < LINHAS; i++)
        for (int j = 0; j < COLUNAS; j++)
            if (tabuleiro_virtual[i][j] == 0) return false;
    return true;
}

/*
    Dada uma coluna, retorna a linha mais baixa disponível (ou -1 se cheia).
    Usada para saber onde a peça deve "cair".
*/
int encontrar_linha_disponivel(int coluna) {
    for (int i = LINHAS - 1; i >= 0; i--) {
        if (tabuleiro_virtual[i][coluna] == 0) return i;
    }
    return -1;
}

/*
    Função simples de IA: escolhe uma coluna aleatória válida para jogar.
    Retorna o índice da coluna escolhida, ou -1 se não houver opções.
*/
int escolher_coluna_ia() {
    int colunas_validas[COLUNAS];
    int num_validas = 0;
    // Monta lista de colunas não-cheias
    for (int j = 0; j < COLUNAS; j++) {
        if (encontrar_linha_disponivel(j) != -1) {
            colunas_validas[num_validas++] = j;
        }
    }
    if (num_validas == 0) return -1;
    // Escolhe aleatoriamente entre as válidas
    return colunas_validas[rand() % num_validas];
}

/*
    Inicia a animação de uma peça caindo em uma coluna e linha específica para um jogador.
    Busca um slot livre no vetor de animações.
*/
void iniciar_animacao(int coluna, int linha, int jogador) {
    for (int i = 0; i < MAX_ANIMACOES; i++) {
        if (!animacoes[i].ativa) {
            animacoes[i].coluna = coluna;
            animacoes[i].linha_final = linha;
            animacoes[i].y_atual = centros_y[0] - raio_ficha * 2; // Inicia acima do topo
            animacoes[i].jogador = jogador;
            animacoes[i].ativa = true;
            break;
        }
    }
}

/*
    Trata um clique do mouse no tabuleiro do jogo, determinando se uma peça pode ser jogada.
    Se sim, inicia a animação da peça caindo na coluna apropriada.
*/
void tratar_clique(int mouse_x, int mouse_y, int* jogador_atual) {
    // Define os limites do tabuleiro conforme a imagem
    int x_ini = centros_x[0] - (centros_x[1] - centros_x[0]) / 2;
    int x_fim = centros_x[COLUNAS-1] + (centros_x[1] - centros_x[0]) / 2;
    int y_ini = centros_y[0] - (centros_y[1] - centros_y[0]) / 2;
    int y_fim = centros_y[LINHAS-1] + (centros_y[1] - centros_y[0]) / 2;

    // Checa se o clique está dentro do tabuleiro
    if (mouse_x >= x_ini && mouse_x < x_fim && mouse_y >= y_ini && mouse_y < y_fim) {
        // Descobre qual coluna foi clicada (mais próxima)
        int coluna = 0;
        int menor_d = abs(mouse_x - centros_x[0]);
        for (int j = 1; j < COLUNAS; j++) {
            int d = abs(mouse_x - centros_x[j]);
            if (d < menor_d) {
                menor_d = d;
                coluna = j;
            }
        }
        // Encontra a linha disponível na coluna selecionada
        int linha_disp = encontrar_linha_disponivel(coluna);
        if (linha_disp != -1) {
            iniciar_animacao(coluna, linha_disp, *jogador_atual);
        }
    }
}

/*
    Função principal do programa.
    Responsável por inicializar SDL, carregar imagens, executar o loop principal e finalizar recursos.
*/
int main(int argc, char** argv) {
    bool ignorar_primeiro_clique = false; // Flag para ignorar clique acidental após mudança de tela
    srand((unsigned int)time(NULL));      // Inicializa a semente do gerador de números aleatórios

    // Inicialização da SDL e SDL_image
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    // Criação da janela e do renderizador
    int largura_janela = 900;
    int altura_janela = 614;
    SDL_Window* window = SDL_CreateWindow("Connect Four", 100, 100, largura_janela, altura_janela, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Carregamento de imagens/texturas
    SDL_Texture* menu_img = IMG_LoadTexture(renderer, "imagens/menu.png");
    SDL_Texture* tabuleiro = IMG_LoadTexture(renderer, "imagens1/jogo_tabuleiro.png");
    SDL_Texture* ficha_vermelha = IMG_LoadTexture(renderer, "imagens1/ficha_vermelha.png");
    SDL_Texture* ficha_amarela = IMG_LoadTexture(renderer, "imagens1/ficha_amarela.png");
    SDL_Texture* vencedor1 = IMG_LoadTexture(renderer, "imagens/vencedor1.png");
    SDL_Texture* vencedor2 = IMG_LoadTexture(renderer, "imagens/vencedor2.png");

    int jogador_atual = 1;   // Indica de quem é a vez (1 = vermelho, 2 = amarelo)
    SDL_Event event;
    bool running = true;     // Controla se o jogo está rodando

    // Loop principal do jogo
    while (running) {
        // Loop de eventos SDL
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Lógica da tela de menu
            if (estado_atual == MENU && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                // Botão para jogar contra IA
                if (x >= 299 && x <= 585 && y >= 282 && y <= 335) {
                    estado_atual = JOGO_IA;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                // Botão para jogar PvP
                } else if (x >= 301 && x <= 585 && y >= 403 && y <= 449) {
                    estado_atual = JOGO_PVP;
                    ignorar_primeiro_clique = true;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                // Botão para sair
                } else if (x >= 294 && x <= 585 && y >= 516 && y <= 566) {
                    running = false;
                }
            }

            // Lógica do jogo (PvP e IA): trata clique para jogar peça
            if ((estado_atual == JOGO_PVP || estado_atual == JOGO_IA) &&
                event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {

                if (ignorar_primeiro_clique) {
                    ignorar_primeiro_clique = false;
                    continue; // Ignora primeiro clique após mudança de tela
                }

                int mouse_x = event.button.x;
                int mouse_y = event.button.y;
                tratar_clique(mouse_x, mouse_y, &jogador_atual);
            }

            // Lógica da tela final (após vitória): trata botões de "Menu" e "Sair"
            if (estado_atual == FINAL && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                // Botão para voltar ao menu
                if (x >= 276 && x <= 600 && y >= 368 && y <= 420) {
                    estado_atual = MENU;
                    jogador_atual = 1;
                    jogador_vencedor = 0;
                    memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                // Botão para sair
                } else if (x >= 276 && x <= 600 && y >= 448 && y <= 506) {
                    running = false;
                }
            }
        }

        // Movimento da IA (apenas no modo IA)
        if (estado_atual == JOGO_IA && jogador_atual == 2) {
            // Só joga se não há animação em andamento
            bool animando = false;
            for (int i = 0; i < MAX_ANIMACOES; i++) {
                if (animacoes[i].ativa) {
                    animando = true;
                    break;
                }
            }

            // Variável estática para controlar tempo de espera entre jogadas da IA
            static Uint32 tempo_espera = 0;

            if (!animando) {
                // Começa a contar tempo se não estava antes
                if (tempo_espera == 0) tempo_espera = SDL_GetTicks();

                // Espera 500 ms antes da IA jogar (efeito visual)
                if (SDL_GetTicks() - tempo_espera > 500) {
                    int coluna_ia = escolher_coluna_ia();
                    int linha_disp = encontrar_linha_disponivel(coluna_ia);
                    if (linha_disp != -1) {
                        iniciar_animacao(coluna_ia, linha_disp, 2);
                    }
                    tempo_espera = 0;
                }
            } else {
                tempo_espera = 0;
            }
        }

        // Renderiza a tela
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Tela de menu
        if (estado_atual == MENU) {
            SDL_RenderCopy(renderer, menu_img, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        // Define cor de fundo conforme modo de jogo
        if (estado_atual == JOGO_PVP) SDL_SetRenderDrawColor(renderer, 200, 255, 200, 255);
        if (estado_atual == JOGO_IA)  SDL_SetRenderDrawColor(renderer, 255, 230, 200, 255);

        SDL_RenderClear(renderer);

        // Desenha peças em animação (caindo)
        for (int i = 0; i < MAX_ANIMACOES; i++) {
            if (animacoes[i].ativa) {
                float destinoY = centros_y[animacoes[i].linha_final] - raio_ficha;
                // Move a peça verticalmente até o destino
                if (animacoes[i].y_atual < destinoY) {
                    animacoes[i].y_atual += 10;
                    if (animacoes[i].y_atual > destinoY) animacoes[i].y_atual = destinoY;
                } else {
                    // Chegou ao destino: marca a peça no tabuleiro virtual e troca jogador
                    animacoes[i].y_atual = destinoY;
                    animacoes[i].ativa = false;
                    tabuleiro_virtual[animacoes[i].linha_final][animacoes[i].coluna] = animacoes[i].jogador;

                    // Checa vitória ou empate
                    if (checar_vitoria(animacoes[i].jogador)) {
                        estado_atual = FINAL;
                        jogador_vencedor = animacoes[i].jogador;
                    } else if (checar_empate()) {
                        estado_atual = MENU;
                        jogador_atual = 1;
                        memset(tabuleiro_virtual, 0, sizeof(tabuleiro_virtual));
                    } else {
                        // Troca o jogador (1 <-> 2)
                        jogador_atual = 3 - animacoes[i].jogador;
                    }
                }

                // Seleciona textura da ficha conforme jogador
                SDL_Texture* ficha = (animacoes[i].jogador == 1) ? ficha_vermelha : ficha_amarela;
                SDL_Rect destino = {
                    centros_x[animacoes[i].coluna] - raio_ficha,
                    (int)animacoes[i].y_atual,
                    raio_ficha * 2,
                    raio_ficha * 2
                };
                SDL_RenderCopy(renderer, ficha, NULL, &destino);
            }
        }

        // Desenha todas as peças já posicionadas no tabuleiro
        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                if (tabuleiro_virtual[i][j] == 0) continue;
                SDL_Texture* ficha = (tabuleiro_virtual[i][j] == 1) ? ficha_vermelha : ficha_amarela;
                SDL_Rect destino = {
                    centros_x[j] - raio_ficha,
                    centros_y[i] - raio_ficha,
                    raio_ficha * 2,
                    raio_ficha * 2
                };
                SDL_RenderCopy(renderer, ficha, NULL, &destino);
            }
        }

        // Desenha a imagem do tabuleiro sobre as peças
        SDL_Rect quad1 = {200, 150 , 447 , 358};
        SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1);

        // Se for a tela final, mostra a imagem do vencedor
        if (estado_atual == FINAL) {
            SDL_RenderCopy(renderer, (jogador_vencedor == 1 ? vencedor1 : vencedor2), NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Controla FPS (ajusta performance do loop)
    }

    // Libera recursos e encerra SDL
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