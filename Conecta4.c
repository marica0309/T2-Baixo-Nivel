#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

// Define o tamanho do tabuleiro (6 linhas por 7 colunas)
#define LINHAS 6
#define COLUNAS 7

int y; 
int y_destino; 

bool animando_ficha = false;

//bool para ignorar o clique anterior
bool ignorar_primeiro_clique = false;

//Coluna selecionada para colocar a ficha
int coluna_selecionada; 
int meio_coluna_selecionada; 

// Matriz que representa o estado do tabuleiro
int tabuleiro_virtual[LINHAS][COLUNAS] = {0};

// Enumeração para os possíveis estados do jogo
typedef enum {
    MENU,
    JOGO_PVP,
    JOGO_IA
} EstadoJogo;

// Variável global que guarda o estado atual do jogo
EstadoJogo estado_atual = MENU;

// Função que verifica se o jogador atual venceu o jogo
bool checar_vitoria(int jogador) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (tabuleiro_virtual[i][j] != jogador) continue;

            // Verifica vitória horizontal
            if (j <= COLUNAS - 4 &&
                tabuleiro_virtual[i][j+1] == jogador &&
                tabuleiro_virtual[i][j+2] == jogador &&
                tabuleiro_virtual[i][j+3] == jogador)
                return true;

            // Verifica vitória vertical
            if (i <= LINHAS - 4 &&
                tabuleiro_virtual[i+1][j] == jogador &&
                tabuleiro_virtual[i+2][j] == jogador &&
                tabuleiro_virtual[i+3][j] == jogador)
                return true;

            // Verifica vitória diagonal principal (↘)
            if (i <= LINHAS - 4 && j <= COLUNAS - 4 &&
                tabuleiro_virtual[i+1][j+1] == jogador &&
                tabuleiro_virtual[i+2][j+2] == jogador &&
                tabuleiro_virtual[i+3][j+3] == jogador)
                return true;

            // Verifica vitória diagonal secundária (↙)
            if (i >= 3 && j <= COLUNAS - 4 &&
                tabuleiro_virtual[i-1][j+1] == jogador &&
                tabuleiro_virtual[i-2][j+2] == jogador &&
                tabuleiro_virtual[i-3][j+3] == jogador)
                return true;
        }
    }
    return false;
}

//Função para encontrar primeira linha disponível para a ficha "cair"
int encontrar_linha_disponivel(int coluna) {
    for (int linha = LINHAS - 1; linha >= 0; linha--) {
        if (tabuleiro_virtual[linha][coluna] == 0) {
            tabuleiro_virtual[linha][coluna] = 1; //sinaliza que aquele espaço no tabuleiro já está ocupado
            if(linha == 0){
                y_destino = 173; 
            }
            else if(linha == 1){
                y_destino = 253; 
            }
            else if(linha == 2){
                y_destino = 333; 
            }
            else if(linha == 3){
                y_destino = 413; 
            }
            else if(linha == 4){
                y_destino = 495; 
            }else if(linha == 5){ 
                y_destino = 584; 
            }
            else{
                y_destino = -1; 
            }
            return linha; //linha = linha que a peça deve cair
        }
    }
    return -1; // Coluna cheia
}

//Seleciona a coluna que o jogador quer colocar a ficha; funcionando
void selecionar_coluna(int coord_x, int coord_y) 
{
    if (coord_x >= 168 && coord_x <= 243 && coord_y <= 586)
    {
        coluna_selecionada = 0;
        meio_coluna_selecionada = 161; 
    }
    else if (coord_x >= 246 && coord_x <= 321 && coord_y <= 586)
    {
        coluna_selecionada = 1;
        meio_coluna_selecionada =245;
    }
    else if (coord_x >= 324 && coord_x <= 399 && coord_y <= 586)
    {
        coluna_selecionada = 2;
        meio_coluna_selecionada = 325;
    }
    else if (coord_x >= 405 && coord_x <= 480 && coord_y <= 586)
    {
        coluna_selecionada = 3; 
        meio_coluna_selecionada = 408;
    }
    else if (coord_x >= 483 && coord_x <= 558 && coord_y <= 586)
    {
        coluna_selecionada = 4;
        meio_coluna_selecionada = 486; 
    }
    else if (coord_x >= 561 && coord_x <= 636 && coord_y <= 586)
    {
        coluna_selecionada = 5;
        meio_coluna_selecionada = 567;
    }
    else if (coord_x >= 639 && coord_x <= 714 && coord_y <= 586){
        coluna_selecionada = 6; 
        meio_coluna_selecionada = 643; 
    }
    else
    {
        coluna_selecionada = -1; //nenhuma coluna selecionada 
    }
}

int main(int argc, char** argv) {
    float escala = 0.6f; // Escala para redimensionar elementos na tela

    // Inicializa a SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Inicializa suporte a imagens PNG com SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    // Define dimensões da janela com base na escala
    int largura_janela = (int)(1500 * escala);
    int altura_janela = (int)(1024 * escala);

    // Cria a janela do jogo
    SDL_Window* window = SDL_CreateWindow(
        "Connect Four",
        100, 100,
        largura_janela, altura_janela,
        SDL_WINDOW_SHOWN
    );

    // Cria o renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Carrega as texturas/imagens
    SDL_Texture* menu_img = IMG_LoadTexture(renderer, "imagens/menu.png");
    SDL_Texture* tabuleiro = IMG_LoadTexture(renderer, "imagens/jogo_tabuleiro.png");
    SDL_Texture* ficha_vermelha = IMG_LoadTexture(renderer, "imagens/ficha_vermelha.png");
    SDL_Texture* ficha_amarela = IMG_LoadTexture(renderer, "imagens/ficha_amarela.png");
    SDL_Texture* vencedor = IMG_LoadTexture(renderer, "imagens/vencedor.png");

    // Define a posição e tamanho do tabuleiro na tela
    SDL_Rect quad1 = {
        (int)(200 * escala),
        (int)(150 * escala),
        (int)(1108 * escala),
        (int)(887 * escala)
    };

    // Variáveis relacionadas à posição e movimentação das fichas
    int posicaoX = (int)(275 * escala);
    int posicaoY = (int)(50 * escala);
    int seleciona_ficha = -1; // -1 = nenhuma, 0 = vermelha, 1 = amarela
    int running = 1; // Controla o loop principal do jogo

    SDL_Event event;

   SDL_Rect local_vermelha = {
    (int)(100 * escala),
    (int)(200 * escala),
    (int)(132 * escala),
    (int)(132 * escala)
    };

    SDL_Rect local_amarela = {
    (int)(100 * escala),
    (int)(400 * escala),
    (int)(132 * escala),
    (int)(132 * escala)
    };

    // Loop principal do jogo
    while (running) {
        // Processamento de eventos
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            // Lógica do menu
            if (estado_atual == MENU) {
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;

                    // Clique no botão "Jogar contra IA"
                    if (x >= 299 && x <= 585 && y >= 282 && y <= 335) {
                        estado_atual = JOGO_IA;
                        printf("Modo: Jogar contra o Computador\n");
                        ignorar_primeiro_clique = true; 
                    }
                    // Clique no botão "Jogar contra outro Jogador"
                    else if (x >= 301 && x <= 585 && y >= 403 && y <= 449) {
                        estado_atual = JOGO_PVP;
                        printf("Modo: Jogar contra outro Jogador\n");
                    }
                    // Clique no botão "Sair"
                    else if (x >= 294 && x <= 585 && y >= 516 && y <= 566) {
                        running = 0;
                        printf("Saindo do jogo\n");
                    }
                }
            }

            if (estado_atual == JOGO_IA)
            {
                seleciona_ficha = 0;
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                {
                    if (ignorar_primeiro_clique)
                    {
                        ignorar_primeiro_clique = false;
                        continue; 
                    }

                    int x = event.button.x;
                    int y = event.button.y;
                    // printf("X = %d\n", x);
                    // printf("Y = %d\n", y);
                    selecionar_coluna(x, y);
                    int linha_disponivel = encontrar_linha_disponivel(coluna_selecionada);
                    
                    if (linha_disponivel != -1)
                    { // entra so depois do 2 clique - arrumar
                        int y = 2;
                        SDL_Rect inicio_destino = {
                            meio_coluna_selecionada,
                            y,
                            (int)(132 * escala),
                            (int)(132 * escala)};

                        while (y < y_destino)
                        {
                            printf("Entra no while"); 
                            y += 10;
                            inicio_destino.y = y;

                            SDL_SetRenderDrawColor(renderer, 255, 230, 200, 255); // Fundo branco
                            SDL_RenderClear(renderer);

                            SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1); // Redesenha o tabuleiro

                            SDL_RenderCopy(renderer, ficha_vermelha, NULL, &local_vermelha); //desenha a ficha vermelha na esquerda
                            SDL_RenderCopy(renderer, ficha_amarela, NULL, &local_amarela); //desenha a ficha amarela na esquerda 

                            // Desenha a ficha na nova posição
                            if (seleciona_ficha == 0)
                            {
                                SDL_RenderCopy(renderer, ficha_vermelha, NULL, &inicio_destino);
                            }
                            else if (seleciona_ficha == 1)
                            {
                                SDL_RenderCopy(renderer, ficha_amarela, NULL, &inicio_destino);
                            }

                            SDL_RenderPresent(renderer);
                            SDL_Delay(10); // Controla a velocidade da animação
                        }
                        printf("Sai do while"); 

                    }
                }
            }
        }

        // Limpa a tela com cor branca
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Exibe o menu principal
        if (estado_atual == MENU)
        {
            SDL_RenderCopy(renderer, menu_img, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }

        // Tela de jogo PvP
        if (estado_atual == JOGO_PVP)
        {
            SDL_SetRenderDrawColor(renderer, 200, 255, 200, 255); // Cor verde clara
            SDL_RenderClear(renderer);
        }

        // Tela de jogo contra IA
        if (estado_atual == JOGO_IA)
        {
            SDL_SetRenderDrawColor(renderer, 255, 230, 200, 255); // Cor laranja clara
            SDL_RenderClear(renderer);
        }

        // Desenha o tabuleiro
        SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1);

        // Desenha ficha vermelha fixa (lado esquerdo)
        SDL_RenderCopy(renderer, ficha_vermelha, NULL, &local_vermelha);

        // Desenha ficha amarela fixa (lado esquerdo)
       
        SDL_RenderCopy(renderer, ficha_amarela, NULL, &local_amarela);

        /*// Atualiza a posição vertical da ficha em movimento (efeito de queda)
        if (posicaoY < (int)(850 * escala) && seleciona_ficha != -1) {
            posicaoY += 1;
        }

        // Desenha a ficha em movimento (se houver)
        if (seleciona_ficha == 0 || seleciona_ficha == 1) {
            SDL_Texture* ficha = seleciona_ficha == 0 ? ficha_vermelha : ficha_amarela;
            SDL_Rect posicao_inicial = {
                posicaoX, posicaoY,
                (int)(132 * escala),
                (int)(132 * escala)
            };
            SDL_RenderCopy(renderer, ficha, NULL, &posicao_inicial);
        }*/

        // Atualiza a tela
        SDL_RenderPresent(renderer);
        SDL_Delay(20); // Pequeno atraso para controlar o FPS
    }

    // Liberação de recursos
    SDL_DestroyTexture(menu_img);
    SDL_DestroyTexture(ficha_amarela);
    SDL_DestroyTexture(ficha_vermelha);
    SDL_DestroyTexture(tabuleiro);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
