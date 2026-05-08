# ESP32: Blink LED con Software Timers (FreeRTOS) ⏱️

Este proyecto es una evolución del "Hola Mundo" básico. En lugar de utilizar un bucle infinito en la tarea principal (`app_main`) con retardos bloqueantes, aquí implementamos el uso de **Software Timers** de FreeRTOS. 

Este enfoque es mucho más eficiente para sistemas embebidos, ya que permite que la CPU quede libre para otras tareas mientras el temporizador gestiona los eventos en segundo plano.

## Objetivos del Proyecto
*   **Aprender Software Timers:** Uso de la librería `freertos/timers.h`.
*   **Callbacks:** Entender cómo funciona una función de callback que se ejecuta cuando un evento ocurre.
*   **Programación Orientada a Eventos:** Evitar el uso de bucles `while(1)` innecesarios en la tarea principal para tareas simples.

## Tecnologías y Librerías
*   **SDK:** ESP-IDF.
*   **Librerías de FreeRTOS:** 
    *   `freertos/FreeRTOS.h` y `freertos/task.h` (Núcleo).
    *   `freertos/timers.h` (Gestión de temporizadores por software).
*   **Driver:** `driver/gpio.h` para el control del hardware.
*   **Logging:** `esp_log.h` para monitorizar la inicialización y el disparo del timer.

## Cómo funciona el código

### 1. Inicialización del Temporizador
En lugar de un bucle, utilizamos la función `xTimerCreate()`. Los parámetros clave son:
- **Periodo:** Definido por la variable `interval` y convertido a ticks mediante `pdMS_TO_TICKS`.
- **Auto-reload (`pdTRUE`):** Esto hace que el temporizador se reinicie automáticamente cada vez que expire, creando un parpadeo constante.
- **Callback:** La función `vTimerCallback` es la que contiene la lógica para alternar el LED.

### 2. El Callback (`vTimerCallback`)
Esta función no es llamada directamente por nuestro código, sino por el **Timer Service Task** de FreeRTOS cada vez que el tiempo llega a cero. Esto permite que el flujo de ejecución sea asíncrono.

### 3. Función `app_main`
A diferencia de un parpadeo tradicional, aquí `app_main` solo configura el hardware y arranca el timer. Una vez lanzado, la tarea principal podría terminar o dedicarse a otras funciones mientras el LED sigue parpadeando de forma independiente.

## Configuración
- **GPIO:** Pin 2 (LED integrado).
- **Frecuencia de parpadeo:** 100ms (definido en la variable `interval`).
- **Nivel de Log:** `ESP_LOGI` para confirmar cada ejecución del timer en el monitor serie.