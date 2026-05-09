# Lectura de ADC y Control de Barra de LEDs con ESP32

Este proyecto demuestra el uso del convertidor analógico-digital (**ADC**) del ESP32 para leer el voltaje variable de un potenciómetro y representar dicha lectura de forma visual mediante una "barra de LEDs". Se utiliza el framework **ESP-IDF** y el sistema operativo **FreeRTOS**.

## Objetivos del Proyecto

* **Configuración del ADC1:** Implementar la lectura de señales analógicas con una resolución de 12 bits.
* **Atenuación de Señal:** Configurar el ADC para soportar rangos de voltaje de hasta 3.3V mediante atenuación de 12dB.
* **Multitarea con Software Timers:** Realizar el muestreo del sensor y la actualización de los actuadores (LEDs) de forma asíncrona utilizando temporizadores de FreeRTOS.
* **Lógica de Umbrales:** Implementar un sistema de decisión basado en rangos para activar secuencialmente una serie de indicadores.

## Especificaciones Técnicas

### 1. Periférico ADC (Analog to Digital Converter)

* **Unidad:** ADC1.
* **Canal:** Canal 4 (correspondiente al GPIO 32 en la mayoría de DevKits).
* **Resolución:** 12 bits (Valores de 0 a 4095).
* **Atenuación:** `ADC_ATTEN_DB_12` (Permite leer el rango completo de 0V a 3.3V).

### 2. Lógica de Control (Barra de LEDs)

El valor leído (0-4095) se divide por 1000 para segmentar la respuesta en 4 estados principales:

* **Caso 0 (0 - 999):** Todos los LEDs externos apagados.
* **Caso 1 (1000 - 1999):** Se enciende el LED 2.
* **Caso 2 (2000 - 2999):** Se encienden LED 2 y LED 3.
* **Caso 3 (3000 - 4095):** Se encienden LED 2, LED 3 y LED 4.

*Nota: El LED 1 (interno) se mantiene parpadeando de forma constante en cada ciclo del timer como indicador de "sistema vivo" (heartbeat).*

## Estructura del Código

### Funciones Principales

* `set_adc()`: Configura la resolución del ancho de bits y la atenuación del canal 4.
* `init_led()`: Inicializa los pines GPIO seleccionados como salidas digitales.
* `vTimerCallback()`: Función periódica (cada 50ms) que:
1. Hace parpadear el LED de diagnóstico.
2. Realiza la lectura cruda (*raw*) del ADC.
3. Evalúa mediante un `switch-case` qué LEDs deben activarse.


* `set_timer()`: Crea y arranca el temporizador de software de FreeRTOS.

## Conexiones (Pinout Sugerido)

* **Potenciómetro:** Pin central al GPIO 32 (ADC1_CH4), extremos a 3.3V y GND.
* **LED 1:** GPIO 2 (LED integrado).
* **LEDs Externos:** Conectados a los GPIOs definidos en el código (asegurar que cada LED tenga su propia resistencia limitadora).