# ESP32 PROJECTS

¡Bienvenido a mi repositorio de aprendizaje de sistemas embebidos! Aquí voy documentando mi progreso con el microcontrolador **ESP32**, utilizando el framework oficial **ESP-IDF** (Espressif IoT Development Framework) y el sistema operativo en tiempo real **FreeRTOS**.

El objetivo de este repositorio es servir como bitácora de mi evolución, desde conceptos básicos de GPIO hasta implementaciones más complejas.

---

## Herramientas y Tecnologías
* **Microcontrolador:** ESP32 (Serie DevKitV1 o similar).
* **SDK:** ESP-IDF (VS Code Extension / Terminal).
* **Lenguaje:** C.
* **RTOS:** FreeRTOS.

---

## Índice de Proyectos

A continuación, se listan los proyectos desarrollados, ordenados por complejidad o cronología:

### 1. [Blink & Logging](./01-blink-led) 💡
Mi primer proyecto con el ESP32. El "Hola Mundo" del hardware.
* **Conceptos aprendidos:** Configuración de GPIO, creación de tareas con FreeRTOS (`vTaskDelay`) y niveles de log con `esp_log.h`.
* **Lo más destacado:** Implementación de logs dinámicos (Info, Warn, Error) según un contador de ciclos.

---

## Cómo ejecutar estos proyectos

1. **Instalar ESP-IDF:** Sigue la [guía oficial de Espressif](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).
