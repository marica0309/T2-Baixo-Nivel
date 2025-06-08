# Connect Four (Lig4) com SDL2

Este projeto é uma implementação gráfica do clássico jogo **Connect Four** (também conhecido como "Lig4") usando as bibliotecas SDL2 e SDL2_image em C. O objetivo é proporcionar uma experiência visual agradável, incluindo animação das peças, menu interativo e modos de jogo tanto contra outro jogador quanto contra uma IA simples.

## 🎮 Como Jogar

- **Objetivo:** Seja o primeiro a formar uma linha de 4 peças (horizontais, verticais ou diagonais).
- **Jogador 1:** Peças vermelhas
- **Jogador 2 / IA:** Peças amarelas

### Modos Disponíveis
- **Jogo PvP:** Dois jogadores humanos alternam as jogadas.
- **Jogo contra IA:** Um jogador humano joga contra o computador.
- **Menu:** Permite escolher o modo de jogo ou sair.

## 📦 Dependências

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

### Instalação das dependências no Ubuntu/Debian:
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev
```

## 🛠️ Compilação

Compile utilizando `gcc`:

```bash
gcc -o connect_four main.c -lSDL2 -lSDL2_image
```

> **Nota:** Certifique-se que as imagens estejam na estrutura de diretórios correta, conforme indicado no código (ex: `imagens/`, `imagens1/`).

## ▶️ Execução

```bash
./connect_four
```

## 🖼️ Estrutura de Imagens Esperada

- `imagens/menu.png` &mdash; Tela de menu inicial
- `imagens/vencedor1.png` &mdash; Tela de vitória do jogador 1
- `imagens/vencedor2.png` &mdash; Tela de vitória do jogador 2
- `imagens1/jogo_tabuleiro.png` &mdash; Imagem do tabuleiro
- `imagens1/ficha_vermelha.png` &mdash; Peça do jogador 1
- `imagens1/ficha_amarela.png` &mdash; Peça do jogador 2/IA

## 📝 Explicação do Funcionamento

- **Menu inicial:** Clique para escolher jogar contra IA, jogar contra outro jogador ou sair.
- **Durante o jogo:** Clique sobre a coluna desejada para jogar sua peça.
- **Animação:** As peças "caem" animadamente até a posição correta.
- **Verificação automática:** O jogo detecta vitórias e empates automaticamente.
- **Tela final:** Após vitória, clique para voltar ao menu ou sair.

## 👨‍💻 Estrutura do Código

- **main.c:** Todo o código-fonte do jogo, incluindo:
  - Lógica do jogo (tabuleiro, regras, vitória/empate)
  - Gerenciamento de estados (menu, jogo, vitória)
  - Renderização com SDL2
  - Tratamento de eventos (cliques, alternância de jogadores, IA)

## 💡 Possíveis Melhorias

- IA mais inteligente (Minimax)
- Áudio e efeitos sonoros
- Animações mais suaves
- Placar de vitórias

## 📄 Licença

Este projeto é fornecido como exemplo educacional, sem garantia de funcionamento ou suporte.

---

> **Autor:** [Seu Nome]  
> **Contato:** [Seu e-mail ou GitHub]
