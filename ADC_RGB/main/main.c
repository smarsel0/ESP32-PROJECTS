#include <stdio.h>             // libreria para funciones de entrada/salida (ejemplo: printf)
#include "driver/gpio.h"       // libreria para controlar los pines GPIO
#include "freertos/FreeRTOS.h" // libreria para el sistema operativo en tiempo real FreeRTOS
#include "freertos/task.h"     // libreria para las tareas de FreeRTOS
#include "freertos/timers.h"   // libreria para los temporizadores de FreeRTOS
#include "esp_log.h"           // libreria para el sistema de log de ESP-IDF
#include "driver/adc.h"        // libreria para el controlador de ADC (Analog to Digital Converter)

// Definimos el pin (GPIO N) en el que se encuentra conectado el LED
#define LED1 2 
#define LED2 2 
#define LED3 2 
#define LED4 2 

uint8_t led_level = 0;            // variable para almacenar el estado del LED
uint8_t count = 0;                // variable para contar el numero de veces que el LED ha cambiado de estado
static const char *TAG = "Main";  // etiqueta para las funciones de logging, útil para identificar el origen de los mensajes en la consola
TimerHandle_t timer;              // variable para almacenar el temporizador de FreeRTOS que se utilizará para hacer parpadear el LED cada cierto tiempo
TimerHandle_t xTimer;             // variable para almacenar el handle del temporizador

int interval = 50;  // intervalo de tiempo en milisegundos para hacer parpadear el LED (500 ms = 0.5 segundos)
int timerId = 1;    // identificador del temporizador, se puede utilizar para diferenciar entre múltiples temporizadores si es necesario
int val_adc = 0;    // variable para almacenar el valor leído del ADC que se utilizará para ajustar el intervalo de parpadeo del LED en función del valor del potenciometro conectado al ADC1_CHANNEL_4

// Prototipos de las funciones que se utilizarán en el programa
esp_err_t init_led(void);    // Definimios una función para inicializar el LED, configurando el pin GPIO como salida
esp_err_t blink_led(void);   // Definimios una función para hacer parpadear el LED, alternando su estado entre encendido y apagado
esp_err_t set_timer(void);   // Aqui estamos definiendo una función para configurar el temporizador
esp_err_t set_adc(void);     // Definimos una función para configurar el ADC (Analog to Digital Converter) para leer valores analógicos.


// Esta es la función de callback que se ejecutará cada vez que el temporizador expire, haciendo parpadear el LED
void vTimerCallback(TimerHandle_t xTimer){
    blink_led(); // llamamos a la función para hacer parpadear el LED

    // Usamos la funcion adc1_get_raw(adc1_channel_t channel) para leer el valor del ADC del canal 4
    val_adc = adc1_get_raw(ADC1_CHANNEL_4);

    // CONTROLAMOS LA ACTIVACION DE LOS LEDs en funcion del valor que ha leido el ADC del Potenciometro
    
    int adc_case = val_adc / 1000; // Dividimos el valor del ADC por 1000 para obtener un valor entre 0 y 4 (ya que el valor maximo 
                                   // del ADC es 4095, y al dividirlo por 1000 obtenemos un valor entre 0 y 4) -> max = 4094

    ESP_LOGI(TAG, "Valor del ADC: %d", val_adc); // Imprimimos el valor del ADC en la consola para verificar que se está leyendo correctamente

    switch (adc_case){
        case 0:
            // Si el valor del ADC es menor a 1000, apagamos todos los LEDs
            gpio_set_level(LED2, 0);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);
            break;
        case 1:
            // Si el valor del ADC es entre 1000 y 1999, encendemos el LED2 y apagamos los demás
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);
            break;
        case 2:
            // Si el valor del ADC es entre 2000 y 2999, encendemos el LED2 y el LED3, y apagamos el LED4
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 1);
            gpio_set_level(LED4, 0);
            break;
        case 3:
            // Si el valor del ADC es entre 3000 y 3999, encendemos el LED2, el LED3 y el LED4
            gpio_set_level(LED2, 1);
            gpio_set_level(LED3, 1);
            gpio_set_level(LED4, 1);
            break;
        default:
            break;
    }
    
}

void app_main(void)
{
    init_led(); // Llamamos a la función para inicializar el LED
    set_timer();
}

/* ¿QUE ES EL GPIO?
 * El GPIO (General Purpose Input/Output) es un pin de la placa que puede ser configurado por el usuario para realizar funciones de entrada o salida.
 * Este pin puede ser utilizado para controlar dispositivos externos, como LEDs, botones, sensores, etc. En este caso, estamos utilizando el GPIO 
 * para controlar un LED conectado al pin 2 de la placa.
 */

// Funcion para inicializar el LED (configuramos el pin GPIO como salida)
esp_err_t init_led(void){

    // LED 1 (se encuentra en la propia board)
    gpio_reset_pin(LED1);                       // reseteamos el pin GPIO para asegurarnos de que esté en un estado conocido(un estado conocido seria, por ejemplo, que el pin esté apagado)
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT); // configuramos el pin GPIO como salida para poder controlar el LED


    /* CONFIGURACION DE LOS LEDS EXTERNOS */
    // LED 2
    gpio_reset_pin(LED2);                       
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    // LED3
    gpio_reset_pin(LED3);                       
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

    // LED4
    gpio_reset_pin(LED4);                       
    gpio_set_direction(LED4, GPIO_MODE_OUTPUT);

    return ESP_OK;                              // devolvemos un valor indicando que la función se ejecutó correctamente
}

// Funcion para hacer parpadear el LED (cambiamos el valor del LED y lo aplicamos al pin GPIO)
esp_err_t blink_led(void){
    led_level = !led_level;          // alternamos el estado del LED (si estaba apagado, lo encendemos; si estaba encendido, lo apagamos)
    gpio_set_level(LED1, led_level); // establecemos el nivel lógico del pin GPIO para controlar el estado del LED
    return ESP_OK;                   // devolvemos un valor indicando que la funcion se ejecuto correctamente
}

// Funcion para configurar el temporizador
esp_err_t set_timer(void){
    ESP_LOGI(TAG, "Timer init configuration");           // imprimimos un mensaje de log para indicar que estamos configurando el temporizador

    xTimer = xTimerCreate("Timer",                       // Nombre del temporizador (solo para fines de depuración, no es utilizado por el kernel)
                              (pdMS_TO_TICKS(interval)), // periodo del temporizador en ticks (convertimos el intervalo de tiempo en milisegundos a ticks utilizando la macro pdMS_TO_TICKS)
                              pdTRUE,                    // el temporizador se reiniciará automáticamente cada vez que expire (modo auto-reload)
                              (void *)timerId,           // identificador del temporizador (se puede usar cualquier valor único, se utiliza para identificar el temporizador en la función de callback)
                              vTimerCallback             // función de callback que se ejecutará cada vez que el temporizador expire (en este caso, la función vTimerCallback que hace parpadear el LED)
    );

    if (xTimer == NULL)
    {
        // The timer was not created.
        ESP_LOGE(TAG, "The timer was not created.");
    }
    else
    {
        if (xTimerStart(xTimer, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(TAG, "The timer could not be set into the Active state.");
        }
    }

    return ESP_OK;
}

// Funcion para configurar el ADC (Analog to Digital Converter) para leer valores analógicos del Potenciometro
esp_err_t set_adc(void){

    // Añadimos la configuracion del ADC (canal y atenuacion que vamos a usar)
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12);

    // Configuramos de cuantos bits va a ser nuestra medida (el maximo de bit que permite nuestro devkitc -> 12 bits)
    adc1_config_width(ADC_WIDTH_BIT_12);

    return ESP_OK;
}