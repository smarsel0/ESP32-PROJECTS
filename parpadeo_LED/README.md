# Introducción al ESP32 con ESP-IDF: Parpadeo de LED y Logging Dinámico

Este proyecto representa el primer contacto con el ecosistema de desarrollo de **Espressif (ESP-IDF)** y el microcontrolador **ESP32**. El objetivo principal fue configurar el entorno de trabajo, entender la estructura básica de un proyecto en C y gestionar periféricos simples (GPIO) junto con el sistema de logs del sistema operativo en tiempo real (FreeRTOS).

## Objetivos del Proyecto

1. **Arranque con ESP-IDF:** Configuración inicial y uso del SDK oficial.
2. **Control de GPIO:** Configurar un pin físico como salida para controlar un LED.
3. **Gestión de Tiempos:** Uso de tareas de FreeRTOS para crear retardos precisos.
4. **Sistema de Logging:** Implementar mensajes por consola con diferentes niveles de prioridad y colores.

## Librerías Utilizadas

Se han implementado las siguientes librerías fundamentales:

* `driver/gpio.h`: Permite la configuración y el control de los pines de entrada/salida (General Purpose Input/Output).
* `freertos/FreeRTOS.h` y `freertos/task.h`: Proporcionan el núcleo del sistema operativo para gestionar el tiempo de ejecución y las pausas mediante `vTaskDelay`.
* `esp_log.h`: Herramienta para enviar mensajes formateados al monitor serie, permitiendo depurar el código de forma profesional.

## Descripción del Funcionamiento

El programa realiza un ciclo infinito donde se alterna el estado de un LED conectado al **GPIO 2** (comúnmente el LED integrado en la placa) cada 1000 milisegundos.

Además del parpadeo, se ha implementado una lógica de contador que incrementa en cada iteración. Según el valor de este contador, el sistema utiliza diferentes niveles de registro (logging) para mostrar el estado en la consola:

* **ESP_LOGI (Información - Verde):** Se muestra mientras el contador es inferior a 10.
* **ESP_LOGW (Advertencia - Amarillo):** Se muestra cuando el contador está entre 10 y 19.
* **ESP_LOGE (Error - Rojo):** Se muestra cuando el contador es igual o superior a 20.
* **Reinicio:** El contador se resetea automáticamente al superar el valor de 30 para mantener el ciclo controlado.

## Funciones Principales

* `init_led()`: Realiza el reset del pin y establece su dirección como salida (`GPIO_MODE_OUTPUT`).
* `blink_led()`: Realiza la lógica de inversión de estado (`!led_level`) y aplica el cambio al hardware.
* `app_main()`: Punto de entrada del programa donde se inicializa el hardware y se ejecuta el bucle de control (`while(1)`).

---

**Resultado:** Este mini proyecto sienta las bases para desarrollos más complejos, dominando la estructura de archivos, el control de pines básicos y la monitorización de variables a través del puerto serie.