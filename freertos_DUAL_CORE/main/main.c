/* ¿QUÉ ES UN RTOS?
 * Un RTOS es (Real Time Operating System) es un componente software que cambia rapidamente entre tareas, dando la impresion
 * de que varios programas se ejecutan al mismo tiempo con uno o mas nucleos de procesamiento
 *
 * DIFERENCIAS ENTRE UN SO Y UN RTOS
 * La principal diferencia es el tiempo de respuesta a eventos externos. Un SO generalmente brinda una respuesta suave en tiempo real no
 * determinista (esto significa que el tiempo de respuesta puede variar y no es garantizado), mientras que un RTOS proporciona una
 * respuesta rápida y determinista (esto significa que el tiempo de respuesta es predecible y garantizado)
 *
 * ¿QUÉ ES FREERTOS?
 * FreeRTOS es un sistema operativo de tiempo real (RTOS) de código abierto diseñado para microcontroladores y sistemas embebidos.
 * Proporciona una plataforma eficiente y ligera para ejecutar múltiples tareas de manera concurrente, permitiendo a los desarrolladores
 * crear aplicaciones en tiempo real con facilidad. FreeRTOS ofrece características como planificación de tareas, sincronización,
 * comunicación entre tareas y gestión de recursos, lo que lo convierte en una opción popular para proyectos de sistemas embebidos.
 *
 * ¿QUÉ ES UN TASK?
 * Una tarea (task) es una unidad de ejecución dentro de un sistema operativo de tiempo real (RTOS). Es un bloque de código que
 * realiza una función específica y se ejecuta de manera concurrente (simultáneamente) con otras tareas. Las tareas pueden ser programadas
 * para ejecutarse en momentos específicos o en respuesta a eventos, y pueden comunicarse entre sí utilizando mecanismos de sincronización
 * y comunicación proporcionados por el RTOS. En FreeRTOS, las tareas se crean utilizando la función xTaskCreate() y se gestionan
 * mediante el planificador del sistema operativo
 *
 * Los estados de una tarea son:
 * - Ready: La tarea está lista para ejecutarse
 * - Running: La tarea está actualmente en ejecución
 * - Blocked: La tarea está bloqueada esperando un evento
 * - Suspended: La tarea está suspendida
 *
 * ¿QUÉ ES UNA COLA?
 * Una cola (queue) es una estructura de datos que permite almacenar y recuperar elementos en un orden específico, generalmente
 * FIFO (First In, First Out). En el contexto de un RTOS, las colas se utilizan para la comunicación entre tareas, permitiendo
 * que una tarea envíe datos a otra tarea de manera segura y eficiente.
 *
 * ¿QUÉ SON LOS MUTEXES?
 * Los MUTEXES (mutual exclusion) son mecanismos de sincronización que permiten que solo una tarea acceda a un recurso compartido
 * a la vez, evitando condiciones de carrera. IMagina que tenemos la tarea A y la tarea B y ambas tareas quieren acceder al mismo recurso.
 * Este tipo de semaforo le da la llave a la tarea que la necesita y solo esta puede acceder al recurso. Mientras la tarea A no devuelva la
 * llave, la tarea B no puede acceder al recurso.
 *
 * ¿QUÉ ES UN SEMAFORO BINARIO?
 * Se trata de una cola de un solo elemento, que puede contener un valor de 0 o 1. Se utiliza para sincronizar tareas o para
 * proteger recursos compartidos.
 * Un semáforo binario puede ser utilizado para indicar la disponibilidad de un recurso o para sincronizar la ejecución de tareas.
 * Por ejemplo, una tarea puede esperar a que un semáforo binario se vuelva disponible (valor 1) antes de acceder a un recurso
 * compartido, y luego liberar el semáforo (establecerlo a 0) una vez que haya terminado de usar el recurso */

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// definimos los GPIOs que vamos a usar para encender los LEDs
#define LEDR 33
#define LEDG 25
#define LEDB 26

// definimos el tamaño de la pila que ocupara cada una de las tareas (en bytes)
#define STACK_SIZE 2048

// definimos las prioridades de cada una de las tareas
#define vTask_R_PRIORITY 1
#define vTask_G_PRIORITY 1
#define vTask_B_PRIORITY 1

// defnimos el delay de cada uno de los LEDs
#define LEDR_Delay 1000
#define LEDG_Delay 50
#define LEDB_Delay 4000

// definimos el TAG para los logs (el TAG es una etiqueta que se utiliza para identificar el origen de los mensajes
// de log en el sistema, lo que facilita la depuración y el seguimiento de eventos específicos en el código)
const char *TAG = "main";

// prototipos de las funcionesk
esp_err_t init_leds(void);
esp_err_t create_task(void);
void vTask_R(void *pvParameters);
void vTask_G(void *pvParameters);
void vTask_B(void *pvParameters);

void app_main(void)
{
    // inicializamos los LEDs
    init_leds();

    // creamos las tareas para cada uno de los LEDs
    create_task();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("Hello from main!\n");

        printf("Number of cores: %d\n", portNUM_PROCESSORS);
    }
}

esp_err_t init_leds(void)
{

    // Configuramos los GPIOs para encender los LEDs
    gpio_reset_pin(LEDR);
    gpio_set_direction(LEDR, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LEDG);
    gpio_set_direction(LEDG, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LEDB);
    gpio_set_direction(LEDB, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

// Funcion
esp_err_t create_task(void)
{

    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL; // variable que se utilizara para almacenar el handle de la tarea creada, lo que nos permitira referenciar y controlar la tarea en el futuro

    xTaskCreatePinnedToCore(vTask_R,            // funcion que vamos a pasarle para que se ejecute en la tarea (vTask_R)
                            "vTask_R",          // nombre de la tarea (este nombre es solo para fines de depuracion y no tiene un impacto funcional en la tarea)
                            STACK_SIZE,         // tamaño de la pila (memoria) que se asignara a la tarea (es una constante que define el tamaño de la pila en bytes)
                            &ucParameterToPass, // parametro que se le pasara a la tarea (en este caso, estamos pasando la direccion de una variable ucParameterToPass, que es un uint8_t)
                            vTask_R_PRIORITY,   // prioridad de la tarea (vTask_R_PRIORITY es una constante que define la prioridad de la tarea como la misma que la tarea idle, lo que significa que esta tarea se ejecutara cuando no haya otras tareas de mayor prioridad listas para ejecutarse)
                            &xHandle,           // puntero a una variable de tipo TaskHandle_t donde se almacenara el handle de la tarea creada
                            0                   // especifica que la tarea se ejecutara en el núcleo 0 del procesador (en un sistema con múltiples núcleos, esto permite asignar tareas a núcleos específicos para optimizar el rendimiento y la eficiencia)
                            );          

    xTaskCreatePinnedToCore(vTask_G,            // funcion que vamos a pasarle para que se ejecute en la tarea (vTask_R)
                            "vTask_G",          // nombre de la tarea (este nombre es solo para fines de depuracion y no tiene un impacto funcional en la tarea)
                            STACK_SIZE,         // tamaño de la pila (memoria) que se asignara a la tarea (es una constante que define el tamaño de la pila en bytes)
                            &ucParameterToPass, // parametro que se le pasara a la tarea (en este caso, estamos pasando la direccion de una variable ucParameterToPass, que es un uint8_t)
                            vTask_G_PRIORITY,   // prioridad de la tarea (vTask_R_PRIORITY es una constante que define la prioridad de la tarea como la misma que la tarea idle, lo que significa que esta tarea se ejecutara cuando no haya otras tareas de mayor prioridad listas para ejecutarse)
                            &xHandle,
                            1                   // especifica que la tarea se ejecutara en el núcleo 1 del procesador (en un sistema con múltiples núcleos, esto permite asignar tareas a núcleos específicos para optimizar el rendimiento y la eficiencia)
                            );

    xTaskCreatePinnedToCore(vTask_B,            // funcion que vamos a pasarle para que se ejecute en la tarea (vTask_R)
                            "vTask_B",          // nombre de la tarea (este nombre es solo para fines de depuracion y no tiene un impacto funcional en la tarea)
                            STACK_SIZE,         // tamaño de la pila (memoria) que se asignara a la tarea (es una constante que define el tamaño de la pila en bytes)
                            &ucParameterToPass, // parametro que se le pasara a la tarea (en este caso, estamos pasando la direccion de una variable ucParameterToPass, que es un uint8_t)
                            vTask_B_PRIORITY,   // prioridad de la tarea (vTask_R_PRIORITY es una constante que define la prioridad de la tarea como la misma que la tarea idle, lo que significa que esta tarea se ejecutara cuando no haya otras tareas de mayor prioridad listas para ejecutarse)
                            &xHandle,
                            tskNO_AFFINITY      // especifica que la tarea no tiene afinidad a un núcleo específico, lo que significa que el planificador del sistema operativo puede asignar la tarea a cualquier núcleo disponible para su ejecución
                            );

    return ESP_OK;
}

/* DEFINIMOS LAS TAREAS PARA CADA UNO DE LOS LEDs*/
void vTask_R(void *pvParameters)
{

    while (1)
    {
        ESP_LOGI(TAG, "LED R Core 0");

        // encendemos LEDR
        gpio_set_level(LEDR, 1);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDR_Delay)); // esperamos 0.5 s

        // apagamos LEDR
        gpio_set_level(LEDR, 0);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDR_Delay)); // esperamos 0.5 s
    }
}

void vTask_G(void *pvParameters)
{

    while (1)
    {
        ESP_LOGW(TAG, "LED G Core 1");

        // encendemos LEDG
        gpio_set_level(LEDG, 1);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDG_Delay)); // esperamos 0.5 s

        // apagamos LEDG
        gpio_set_level(LEDG, 0);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDG_Delay)); // esperamos 0.5 s
    }
}

void vTask_B(void *pvParameters)
{

    while (1)
    {
        ESP_LOGE(TAG, "LED B Core ANY");

        // encendemos LEDB
        gpio_set_level(LEDB, 1);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDB_Delay)); // esperamos 0.5 s

        // apagamos LEDB
        gpio_set_level(LEDB, 0);

        // Esta funcion lo que hace es retrasar la ejecucion de la tarea actual
        vTaskDelay(pdMS_TO_TICKS(LEDB_Delay)); // esperamos 0.5 s
    }
}