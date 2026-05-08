#include <stdio.h>
#include "driver/gpio.h"       // incluimos las librerías necesarias para el uso de GPIOs en el ESP32
#include "freertos/FreeRTOS.h" // incluimos las librerías necesarias para el uso de FreeRTOS en el ESP32
#include "freertos/task.h"     // incluimos las librerías necesarias para el uso de tareas en FreeRTOS
#include "esp_log.h"           // incluimos las librerías necesarias para el uso de funciones de logging en el ESP32
#include "freertos/timers.h"   // incluimos las librerías necesarias para el uso de temporizadores en FreeRTOS

#define LED1 2 // definimos el número del pin GPIO al que está conectado el LED (LED incorporado en la placa)

uint8_t led_level = 0;     // variable para almacenar el estado del LED
TimerHandle_t xTimer;      // variable para almacenar el handle del temporizador

esp_err_t init_led(void);  // Aqui estamos definiendo una función para inicializar el LED, configurando el pin GPIO como salida
esp_err_t blink_led(void); // Aqui estamos definiendo una función para hacer parpadear el LED, alternando su estado entre encendido y apagado
esp_err_t set_timer(void); // Aqui estamos definiendo una función para configurar el temporizador
int interval = 100;       // intervalo de tiempo en milisegundos para el parpadeo del LED (1 segundo)
int timerID = 1;           // identificador del temporizador (puede ser cualquier valor único, se usa para identificar el temporizador en la función de callback)


#define TAG "LED_BLINK"    // definimos una etiqueta para los mensajes de log relacionados con el parpadeo del LED


// Esta es la función de callback que se ejecutará cada vez que el temporizador expire, haciendo parpadear el LED
void vTimerCallback(TimerHandle_t xTimer){
    ESP_LOGI(TAG, "EVENT WAS CALLED FROM TIMER");
    blink_led(); // llamamos a la función para hacer parpadear el LED
}


void app_main(void)
{
    init_led(); // llamamos a la funcion para inicializar el LED
    set_timer(); // llamamos a la funcion para configurar el temporizador
}



// Funcion para inicializar el LED
esp_err_t init_led(void)
{
    gpio_reset_pin(LED1);                       // reseteamos el pin GPIO para asegurarnos de que esté en un estado conocido
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT); // configuramos el pin GPIO como salida para poder controlar el LED
    return ESP_OK;                              // retornamos un valor indicando que la función se ejecutó correctamente
}

// Funcion para hacer parpadear el LED
esp_err_t blink_led(void)
{
    led_level = !led_level;          // alternamos el estado del LED (si estaba apagado, lo encendemos; si estaba encendido, lo apagamos)
    gpio_set_level(LED1, led_level); // establecemos el nivel lógico del pin GPIO para controlar el estado del LED
    return ESP_OK;                   // retornamos un valor indicando que la función se ejecutó correctamente
}

// Funcion para configurar el temporizador
esp_err_t set_timer(void)
{
    ESP_LOGI(TAG, "Timer init configuration");           // imprimimos un mensaje de log para indicar que estamos configurando el temporizador

    xTimer = xTimerCreate("Timer",                       // Nombre del temporizador (solo para fines de depuración, no es utilizado por el kernel)
                              (pdMS_TO_TICKS(interval)), // periodo del temporizador en ticks (convertimos el intervalo de tiempo en milisegundos a ticks utilizando la macro pdMS_TO_TICKS)
                              pdTRUE,                    // el temporizador se reiniciará automáticamente cada vez que expire (modo auto-reload)
                              (void *)timerID,           // identificador del temporizador (se puede usar cualquier valor único, se utiliza para identificar el temporizador en la función de callback)
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