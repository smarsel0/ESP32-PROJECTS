# Control de Intensidad RGB mediante PWM (LEDC) y Software Timers

Este proyecto profundiza en el uso de periféricos avanzados del ESP32, específicamente el controlador **LEDC (LED Control)** para generar señales **PWM (Pulse Width Modulation)**. El objetivo es controlar de forma independiente la intensidad de los tres canales de un LED RGB (Red, Green, Blue) creando un efecto de transición suave.

## Objetivos del Proyecto

* **Dominar el periférico LEDC:** Configuración de timers y canales independientes para PWM.
* **Control de Resolución:** Uso de una resolución de 10 bits para un control preciso de la intensidad.
* **Asincronismo con FreeRTOS:** Uso de un *Software Timer* para actualizar los ciclos de trabajo (duty cycle) sin bloquear el flujo principal.
* **Cálculos Eficientes:** Aplicación de desplazamiento de bits (`1 << 10`) para calcular límites de resolución.

## Especificaciones Técnicas

* **Frecuencia PWM:** 20 kHz (proporciona un parpadeo imperceptible al ojo humano).
* **Resolución:** 10 bits (valores de 0 a 1023).
* **Canales utilizados:**
* **Rojo:** GPIO 33 (Canal 0)
* **Verde:** GPIO 25 (Canal 1)
* **Azul:** GPIO 26 (Canal 2)


* **Intervalo de actualización:** 50 ms.

---

## Arquitectura del Software

### 1. Configuración del LEDC (`set_pwm`)

El controlador LEDC en el ESP-IDF se divide en dos pasos fundamentales:

1. **Configuración del Timer:** Se define la frecuencia (20kHz), la resolución (10 bits) y qué timer de hardware se usará.
2. **Configuración de Canales:** Se asocia cada GPIO a un canal específico y se vincula al timer configurado previamente.

### 2. Gestión de Intensidad Dinámica

En lugar de un bucle `while(1)`, el proyecto utiliza un **Timer de FreeRTOS** en modo *auto-reload*. En cada expiración del timer:

* Se incrementan los valores de `dutyR`, `dutyG` y `dutyB`.
* Se aplica una lógica de *wrap-around*: si el valor alcanza el máximo ($2^{10}-1 = 1023$), vuelve a 0.
* Se llama a `ledc_set_duty()` y `ledc_update_duty()` para hacer efectivos los cambios en el hardware.

### 3. Funciones de Soporte

* `init_led()`: Inicializa el LED integrado de la placa (GPIO 2) para diagnóstico visual.
* `blink_led()`: Alterna el estado del LED de diagnóstico.
* `set_pwm_duty()`: Encapsula la lógica de actualización de los tres canales para mantener el código limpio.

---

## Aprendizajes Clave

* **Diferencia entre Set y Update:** En ESP-IDF, `ledc_set_duty` solo cambia el valor en memoria; es necesario llamar a `ledc_update_duty` para que el cambio se refleje físicamente en la señal de salida.
* **Independencia de Canales:** Cómo un solo Timer de hardware puede gobernar múltiples canales con diferentes ciclos de trabajo pero compartiendo la misma frecuencia.
* **Optimización de Recursos:** Al usar timers de software, la tarea `app_main` queda libre tras la configuración inicial, permitiendo que el sistema sea más eficiente energéticamente.

---

**Siguiente paso:** Implementar una curva de corrección gamma para que la transición de brillo sea percibida de forma lineal por el ojo humano.