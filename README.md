## Link do vídeo: 

# Projeto BitDogLab – Interfaces de Comunicação Serial com RP2040


## Funcionalidades Implementadas

- **Entrada Serial e Exibição no OLED:**
  - **Recepção de Caracteres:** Utiliza a interface UART para receber caracteres digitados no Serial Monitor do VS Code.
  - **Display SSD1306:** Cada caractere recebido é exibido no display OLED 128x64 via I2C (conectado aos pinos GPIO 14 e 15).
  - **Fontes Customizadas:** A biblioteca `font.h` foi modificada para incluir caracteres minúsculos, permitindo a exibição correta de textos mistos.

- **Controle da Matriz de LEDs WS2812 (5x5):**
  - **Símbolos Numéricos:** Ao digitar um número entre 0 e 9 no Serial Monitor, um padrão correspondente é exibido na matriz 5x5 de LEDs (conectada ao GPIO 7).  
  - **Lookup Table:** Foi implementada uma tabela (lookup table) que mapeia cada dígito para um padrão de 25 bits (5 linhas x 5 colunas).

- **Interação com Botões e Controle de LED RGB:**
  - **Botão A (GPIO 5):** Ao ser pressionado, alterna o estado do LED RGB (canal que, conforme sua fiação, está associado à cor Azul) e exibe uma mensagem informativa no display e envia um texto via Serial Monitor.
  - **Botão B (GPIO 6):** Ao ser pressionado, alterna o estado do LED RGB (canal que, conforme sua fiação, está associado à cor Verde) e também exibe a mensagem correspondente no display e via Serial.
  - **Uso de Interrupções e Debouncing:** Todos os botões são gerenciados por meio de rotinas de interrupção (IRQ) e possuem tratamento de debounce por software para evitar múltiplos acionamentos indesejados.

- **Outras Funcionalidades:**
  - **Comunicação I2C:** Para a inicialização e controle do display SSD1306.
  - **Controle via PIO:** A matriz WS2812 é acionada por meio de um programa PIO customizado (arquivo `ws2812.pio`) que garante a temporização necessária para a comunicação com os LEDs endereçáveis.
  - **Envio de Informações pela UART:** Mensagens de status (por exemplo, estado dos LEDs e operações dos botões) são enviadas para o Serial Monitor.

---

## Requisitos do Projeto

- **Hardware Utilizado:**
  - **Placa BitDogLab** (com RP2040)
  - **Matriz de LEDs WS2812 (5x5):** Conectada ao GPIO 7.
  - **LED RGB Comum:** Conectado aos GPIOs 11, 12 e 13.
  - **Botões:** Botão A no GPIO 5 e Botão B no GPIO 6.
  - **Display OLED SSD1306 (128x64):** Conectado via I2C aos GPIOs 14 (SDA) e 15 (SCL).

- **Software e Bibliotecas:**
  - Pico SDK (com suporte a PIO, I2C, UART, IRQ, etc.)
  - Biblioteca de display SSD1306
  - Biblioteca de fontes customizada (`font.h`)
  - Programa PIO (`ws2812.pio`) para o controle dos LEDs WS2812

- **Conceitos Abordados:**
  - Comunicação Serial (UART)
  - Interfaces I2C
  - Controle de LEDs comuns e endereçáveis (WS2812)
  - Interrupções e Debouncing em botões
  - Estruturação e organização do código para sistemas embarcados

---

## Como Compilar e Executar

1. **Pré-requisitos:**
   - Instale o [Pico SDK](https://github.com/raspberrypi/pico-sdk) conforme as instruções oficiais.
   - Configure o ambiente de desenvolvimento (CMake, toolchain, etc.).

2. **Clonando o Repositório:**
   ```bash
   git clone <URL_DO_SEU_REPOSITORIO>
   cd <NOME_DA_PASTA>
   mkdir build && cd build
cmake ..
ninja

Organização do Código
DisplayC.c: Arquivo principal com a implementação do sistema, que integra as interfaces UART, I2C, controle dos LEDs (RGB e WS2812) e o tratamento dos botões com interrupções e debouncing.
inc/ssd1306.c / inc/ssd1306.h: Biblioteca para controle do display OLED SSD1306.
inc/font.h: Biblioteca de fontes, modificada para incluir caracteres minúsculos.
ws2812.pio: Código PIO para comunicação com os LEDs endereçáveis WS2812.
CMakeLists.txt: Arquivo de configuração do CMake para a compilação do projeto.

# FEITO POR CÉSAR REBOUÇAS COSTA
