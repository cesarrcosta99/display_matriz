/* DisplayC.c - Projeto Completo Funcional */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

// ==================== DEFINIÇÕES ====================
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

#define WS2812_PIN 7
#define NUM_PIXELS 25
#define IS_RGBW false

#define LED_R 13
#define LED_G 11
#define LED_B 12

#define BUTTON_A 5
#define BUTTON_B 6

// ==================== VARIÁVEIS GLOBAIS ====================
volatile uint32_t last_button_time_A = 0;
volatile uint32_t last_button_time_B = 0;
volatile bool led_g_state = false;
volatile bool led_b_state = false;
volatile bool update_display = false;
volatile char serial_char = ' ';
char current_num = -1;

bool led_buffer[NUM_PIXELS] = {0};
ssd1306_t ssd;
PIO ws2812_pio = pio0;
uint ws2812_sm = 0;

void ssd1306_clear(ssd1306_t *p);

// Declaração do programa PIO (gerado automaticamente)
extern const struct pio_program ws2812_program;

// ==================== PROTÓTIPOS ====================
void setup_OLED();
void setup_RGB();
void setup_buttons();
void setup_WS2812();
void update_matrix(uint8_t num);
void draw_display();
void handle_serial();
bool debounce(volatile uint32_t *last_time);
void button_handler(uint gpio, uint32_t events);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void update_led_matrix();

void ssd1306_clear(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
}


// ==================== IMPLEMENTAÇÃO ====================
void setup_OLED() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&ssd, 128, 64, false, OLED_ADDR, I2C_PORT);
    ssd1306_clear(&ssd);
    ssd1306_draw_string(&ssd, "Sistema Pronto!", 20, 30);
    ssd1306_send_data(&ssd);
}

// Função auxiliar para enviar um valor para a matriz WS2812 com o deslocamento necessário.
static inline void put_pixel(uint32_t pixel_grb) {
    // Desloca 8 bits para a esquerda antes de enviar.
    pio_sm_put_blocking(ws2812_pio, ws2812_sm, pixel_grb << 8u);
}

void setup_RGB() {
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_init(LED_B);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_set_dir(LED_B, GPIO_OUT);
    
    gpio_put(LED_R, 0);
    gpio_put(LED_G, 0);
    gpio_put(LED_B, 0);
}

void setup_WS2812() {
    uint offset = pio_add_program(ws2812_pio, &ws2812_program);
    ws2812_program_init(ws2812_pio, ws2812_sm, offset, WS2812_PIN, 800000, IS_RGBW);
}

bool debounce(volatile uint32_t *last_time) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    return (now - *last_time) > 50;
}

void button_handler(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (gpio == BUTTON_A && (now - last_button_time_A) > 150) {
        // Atualiza o tempo do último acionamento para o Botão A
        last_button_time_A = now;
        
        // Ação do Botão A: alterna o LED conectado em LED_B (cor Azul)
        led_b_state ^= 1;
        gpio_put(LED_B, led_b_state);
        serial_char = 'B';  // Sempre maiúsculo
        update_display = true;
        printf("[BOTAO A] Azul: %s\n", led_b_state ? "ON" : "OFF");
    }
    else if (gpio == BUTTON_B && (now - last_button_time_B) > 150) {
        // Atualiza o tempo do último acionamento para o Botão B
        last_button_time_B = now;
        
        // Ação do Botão B: alterna o LED conectado em LED_G (cor Verde)
        led_g_state ^= 1;
        gpio_put(LED_G, led_g_state);
        serial_char = 'G';  // Sempre maiúsculo
        update_display = true;
        printf("[BOTAO B] Verde: %s\n", led_g_state ? "ON" : "OFF");
    }
}




void setup_buttons() {
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
}

void update_matrix(uint8_t num) {
    const bool numeros[10][25] = {
        // Número 0
        {1,1,1,1,1,
         1,0,0,0,1,
         1,0,0,0,1,
         1,0,0,0,1,
         1,1,1,1,1},
        // Número 1 (Coluna central)
        {0,0,1,0,0,
         0,0,1,0,0,
         0,0,1,0,1,
         0,1,1,0,0,
         0,0,1,0,0},
        // Número 2
        {1,1,1,1,1,
         1,0,0,0,0,
         1,1,1,1,1,
         0,0,0,0,1,
         1,1,1,1,1},
        // Número 3
        {1,1,1,1,1,
         0,0,0,0,1,
         1,1,1,1,1,
         0,0,0,0,1,
         1,1,1,1,1},
        // Número 4
        {1,0,0,0,0,
         0,0,0,0,1,
         1,1,1,1,1,
         1,0,0,0,1,
         1,0,0,0,1},
        // Número 5
        {1,1,1,1,1,
         0,0,0,0,1,
         1,1,1,1,1,
         1,0,0,0,0,
         1,1,1,1,1},
        // Número 6
        {1,1,1,1,1,
         1,0,0,0,1,
         1,1,1,1,1,
         1,0,0,0,0,
         1,1,1,1,1},
        // Número 7
        {1,0,0,0,0,
         0,0,0,0,1,
         1,0,0,0,0,
         0,0,0,0,1,
         1,1,1,1,1},
        // Número 8
        {1,1,1,1,1,
         1,0,0,0,1,
         1,1,1,1,1,
         1,0,0,0,1,
         1,1,1,1,1},
        // Número 9
        {1,1,1,1,1,
         0,0,0,0,1,
         1,1,1,1,1,
         1,0,0,0,1,
         1,1,1,1,1}
    };

    if(num <= 9) {
        memcpy(led_buffer, numeros[num], sizeof(led_buffer));
    }
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void update_led_matrix() {
    if(current_num == -1) return;
    
    // Define a cor azul – ajuste se os seus LEDs esperarem GRB (p.ex.: talvez precise trocar os valores)
    uint32_t color = urgb_u32(0, 0, 255);
    
    // Envia para cada LED da matriz o valor (com deslocamento via put_pixel)
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel( led_buffer[i] ? color : 0 );
    }
    // Aguarda um curto período para garantir que os LEDs fixem os dados
    sleep_us(50);
    
    // Reseta current_num para evitar atualizações repetidas
    current_num = -1;
}



void draw_display() {
    ssd1306_fill(&ssd, false);
    
    // Caractere central
    ssd1306_draw_char(&ssd, serial_char, 60, 20);
    
    // Status LEDs
    ssd1306_draw_string(&ssd, "Verde:", 10, 40);
    ssd1306_draw_string(&ssd, led_g_state ? "ON " : "OFF", 60, 40);
    ssd1306_draw_string(&ssd, "Azul:", 10, 50);
    ssd1306_draw_string(&ssd, led_b_state ? "ON " : "OFF", 60, 50);
    
    ssd1306_send_data(&ssd);
}

void handle_serial() {
    int c = getchar_timeout_us(0);
    if(c != PICO_ERROR_TIMEOUT && c >= 32 && c <= 126) {
        serial_char = (char)c;
        update_display = true;
        
        if(c >= '0' && c <= '9') {
            current_num = c - '0';
            update_matrix(current_num);
            update_led_matrix();
        }
    }
}

int main() {
    stdio_init_all();
    setup_OLED();
    setup_RGB();
    setup_buttons();
    setup_WS2812();

    printf("\n=== Sistema Inicializado ===\n");
    printf("Digite caracteres ou numeros (0-9)\n");

    while(true) {
        handle_serial();
        
        if(update_display) {
            draw_display();
            update_display = false;
        }
        
        sleep_ms(10);
    }
}