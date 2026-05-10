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

### 2. [Software Timers](./02-software-timers) ⏱️

Evolución del parpadeo básico utilizando el servicio de temporizadores de FreeRTOS.

* **Conceptos aprendidos:** Uso de `freertos/timers.h`, funciones de callback y programación orientada a eventos.
* **Lo más destacado:** Ejecución asíncrona del parpadeo, liberando la tarea principal de bucles bloqueantes.

### 3. [PWM RGB Control](./03-pwm-rgb) 〰️

Control de intensidad de un LED RGB utilizando el periférico LEDC (LED Control).

* **Conceptos aprendidos:** Configuración de canales y timers PWM, resolución de bits y actualización dinámica del ciclo de trabajo (*duty cycle*).
* **Lo más destacado:** Uso de desplazamiento de bits para límites de resolución y gestión de tres canales independientes.

### 4. [ADC & LED Bar](./04-adc-led-bar) 🎚️

Lectura de señales analógicas para controlar una barra de LEDs según el valor de un potenciómetro.

* **Conceptos aprendidos:** Configuración de ADC1, atenuación de señal (12dB) y mapeo de valores digitales (12 bits) a lógica de control.
* **Lo más destacado:** Creación de una interfaz física reactiva que segmenta el rango del ADC en umbrales de activación.

### 5. [FreeRTOS Multitasking](./05-freertos-tasks) 🧠

Exploración de la concurrencia real mediante la creación de múltiples hilos de ejecución.

* **Conceptos aprendidos:** Creación de tareas con `xTaskCreate`, gestión de prioridades y asignación de memoria (*stack size*).
* **Lo más destacado:** Ejecución simultánea de tareas para los canales R, G y B con diferentes frecuencias de refresco sin interferencias.

---

## Cómo ejecutar estos proyectos

1. **Instalar ESP-IDF:** Sigue la [guía oficial de Espressif](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).
2. **Clonar el repositorio:**
```bash
git clone https://github.com/smarsel0/ESP32-PROJECTS.git

```


3. **Compilar y Flashear:**
Entra en la carpeta del proyecto específico y ejecuta:
```bash
idf.py build
idf.py -p [PUERTO_COM] flash monitor

```
