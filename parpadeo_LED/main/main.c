#include <stdio.h>
#include "driver/gpio.h" // incluimos las librerías necesarias para el uso de GPIOs en el ESP32
#include "freertos/FreeRTOS.h" // incluimos las librerías necesarias para el uso de FreeRTOS en el ESP32
#include "freertos/task.h" // incluimos las librerías necesarias para el uso de tareas en FreeRTOS
#include "esp_log.h" // incluimos las librerías necesarias para el uso de funciones de logging en el ESP32

#define LED1 2 // definimos el número del pin GPIO al que está conectado el LED (LED incorporado en la placa)

uint8_t led_level = 0; // variable para almacenar el estado del LED
uint8_t count = 0; // variable para contar el numero de veces que el LED ha cambiado de estado
static const char *TAG = "LED_APP"; // etiqueta para las funciones de logging, útil para identificar el origen de los mensajes en la consola

esp_err_t init_led(void); // Aqui estamos definiendo una función para inicializar el LED, configurando el pin GPIO como salida
esp_err_t blink_led(void); // Aqui estamos definiendo una función para hacer parpadear el LED, alternando su estado entre encendido y apagado

void app_main(void)
{
    init_led(); // llamamos a la funcion para inicializar el LED

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS); // esperamos 1 segundo (1000 milisegundos) antes de cambiar el estado del LED
        blink_led(); // llamamos a la funcion para hacer parpadear el LED
        //printf("LED state: %s\n", led_level ? "ON" : "OFF"); // imprimimos el estado actual del LED en la consola (ON o OFF)

        count++; // Incrementamos el contador cada vez que el LED cambia de estado

        // reiniciamos el contador después de 30 iteraciones para evitar que crezca indefinidamente
        (count > 30) ? count = 0 : count;

        // Usamos la libreria ESP_LOG para imprimir el estado del LED en la consola con un formato más estructurado)
        if(count < 10){ESP_LOGI(TAG, "Counter Value: %u.", count); } // ESP_LOGI para mensajes informativos (color: verde)
        if(count >= 10 && count < 20){ESP_LOGW(TAG, "Counter Value: %u.", count);} // ESP_LOGW para mensajes de advertencia (color: amarillo)
        if(count >= 20){ESP_LOGE(TAG, "Counter Value: %u.", count);} // ESP_LOGE para mensajes de error (color: rojo)
    }
}

// Funcion para inicializar el LED
esp_err_t init_led(void){
    gpio_reset_pin(LED1); // reseteamos el pin GPIO para asegurarnos de que esté en un estado conocido
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT); // configuramos el pin GPIO como salida para poder controlar el LED
    return ESP_OK; // retornamos un valor indicando que la función se ejecutó correctamente
}

// Funcion para hacer parpadear el LED
esp_err_t blink_led(void){
    led_level = !led_level; // alternamos el estado del LED (si estaba apagado, lo encendemos; si estaba encendido, lo apagamos)
    gpio_set_level(LED1, led_level); // establecemos el nivel lógico del pin GPIO para controlar el estado del LED
    return ESP_OK; // retornamos un valor indicando que la función se ejecutó correctamente
}