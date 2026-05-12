/*
 * PROYECTO: Comunicación entre tareas mediante Queues (Colas)
 * OBJETIVO: vTask_A envía un contador y parpadea el LED, vTask_B recibe y procesa.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Configuración de Hardware
#define LED_PLACA 2

// Configuración de Tiempos (ms)
#define LED_DELAY 1000
#define RECEIVE_TIMEOUT 500

// Configuración de FreeRTOS
#define STACK_SIZE 2048
#define TASK_PRIORITY 1

// TAG para logs
static const char *TAG = "MAIN_APP";

// Handle de la cola
QueueHandle_t GlobalQueue = NULL;

// Prototipos de las funciones
esp_err_t init_hardware(void);
void create_tasks(void);
void vTask_Sender(void *pvParameters);
void vTask_Receiver(void *pvParameters);


/********************************************************************************************
 ************************************ FUNCION PRINCIPAL *************************************
 ********************************************************************************************/

void app_main(void)
{
    // 1. Creamos la cola antes que las tareas
    // Capacidad para 10 elementos de 32 bits
    GlobalQueue = xQueueCreate(10, sizeof(uint32_t));

    if (GlobalQueue != NULL)
    {
        ESP_LOGI(TAG, "Cola creada con éxito");

        init_hardware();
        create_tasks();
    }
    else
    {
        ESP_LOGE(TAG, "Error crítico: No se pudo crear la cola");
    }
}


/********************************************************************************************
 **************************************** FUNCIONES *****************************************
 ********************************************************************************************/

// Inicialización de GPIO
esp_err_t init_hardware(void)
{
    gpio_reset_pin(LED_PLACA);
    gpio_set_direction(LED_PLACA, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

// Creación de Tareas
void create_tasks(void)
{
    xTaskCreatePinnedToCore(vTask_Sender, 
                            "Sender", 
                            STACK_SIZE, 
                            NULL, 
                            TASK_PRIORITY, 
                            NULL, 
                            0
                           );

    xTaskCreatePinnedToCore(vTask_Receiver, 
                            "Receiver", 
                            STACK_SIZE, 
                            NULL, 
                            TASK_PRIORITY, 
                            NULL, 
                            0
                        );
}

/********************************************************************************************
 ***************************************** TAREAS *******************************************
 ********************************************************************************************/

// TAREA A: Emisor
void vTask_Sender(void *pvParameters)
{
    uint32_t count = 0;

    while (1)
    {
        // Encendemos el LED
        gpio_set_level(LED_PLACA, 1);

        ESP_LOGW(TAG, "SENDER: Enviando valor %lu a la cola", count);

        // Enviamos el valor por COPIA. Esperamos hasta 1 segundo si la cola está llena.
        if (xQueueSend(GlobalQueue, &count, pdMS_TO_TICKS(1000)) != pdPASS) // emviamos el dato (count) a la cola (GlobalQueue) y 
                                                                            // esperamos hasta 1000ms si la cola está llena
        {
            ESP_LOGE(TAG, "SENDER: Error, cola llena");
        }

        vTaskDelay(pdMS_TO_TICKS(LED_DELAY));

        // Apagamos el LED
        gpio_set_level(LED_PLACA, 0);
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY));

        count++;
        if (count > 7) count = 0; // Reiniciamos el contador
    }
}

// TAREA B: Receptor
void vTask_Receiver(void *pvParameters)
{
    uint32_t receivedValue = 0;

    while (1)
    {
        // xQueueReceive bloquea la tarea hasta que haya algo en la cola
        // El tercer parámetro es el tiempo de espera (Ticks to wait)
        if (xQueueReceive(GlobalQueue, &receivedValue, pdMS_TO_TICKS(RECEIVE_TIMEOUT)) == pdPASS) // recibimos el dato de la cola (GlobalQueue) 
                                                                                                  // y lo almacenamos en receivedValue, esperando hasta RECEIVE_TIMEOUT ms
        {
            ESP_LOGI(TAG, "RECEIVER: Dato obtenido -> %lu", receivedValue);

            // Simulamos que el receptor tarda un poco en procesar el dato
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        else
        {
            // Este log saldrá si pasan 500ms sin que llegue nada
            ESP_LOGD(TAG, "RECEIVER: Esperando datos...");
        }
    }
}

/*********************************************************************************************/