# Introduccion a FreeRTOS con ESP32

Este proyecto consiste en una introduccion practica a los sistemas operativos de tiempo real (RTOS) utilizando FreeRTOS sobre el framework ESP-IDF. El objetivo principal es comprender el funcionamiento de las tareas concurrentes y la gestion de recursos en un microprocesador multihilo.

## Conceptos Teoricos

### ¿Que es un RTOS?

Un RTOS (Real Time Operating System) es un componente de software que cambia rapidamente entre tareas, dando la impresion de que varios programas se ejecutan al mismo tiempo. A diferencia de un sistema operativo convencional, un RTOS proporciona una respuesta rapida y determinista, asegurando que los tiempos de respuesta sean predecibles y garantizados.

### Tareas (Tasks)

Una tarea es una unidad de ejecucion independiente. En este proyecto, cada LED (Rojo, Verde y Azul) es gestionado por una tarea distinta que se ejecuta de manera concurrente con las demas.

Estados de una tarea:

* Ready: La tarea esta lista para ejecutarse.
* Running: La tarea esta actualmente en ejecucion.
* Blocked: La tarea esta esperando un evento o un tiempo de retardo.
* Suspended: La tarea ha sido pausada explicitamente.

## Descripcion del Proyecto

El programa crea tres tareas independientes para controlar un LED RGB. Cada tarea gestiona un color diferente con tiempos de parpadeo distintos, demostrando como el planificador de FreeRTOS alterna la ejecucion para que todos los LEDs funcionen simultaneamente sin bloquearse entre si.

### Configuracion de Hardware

Se han definido los siguientes pines GPIO para los LEDs:

* LED Rojo: GPIO 33
* LED Verde: GPIO 25
* LED Azul: GPIO 26

### Configuracion de Tareas

Cada tarea ha sido configurada con los siguientes parametros:

* Tamaño de pila (Stack): 2048 bytes.
* Prioridad: Nivel 1 (todas las tareas tienen la misma prioridad para este ejercicio).
* Delays (Retardos):
* LED Rojo: 1000 ms
* LED Verde: 2000 ms
* LED Azul: 4000 ms



## Funciones Principales

### xTaskCreate

Es la funcion encargada de instanciar las tareas en el sistema. Recibe como parametros la funcion de la tarea, un nombre descriptivo, el tamaño de la pila, parametros de entrada, la prioridad y un handle para su control posterior.

### vTaskDelay

A diferencia de un retardo simple (como delay() en otros frameworks), vTaskDelay coloca la tarea en estado Blocked. Esto permite que el procesador ejecute otras tareas mientras transcurre el tiempo de espera, optimizando el uso de la CPU.

### init_leds

Configura la direccion de los pines GPIO seleccionados como salida y realiza un reset inicial para asegurar un estado conocido del hardware.

## Logs y Depuracion

El proyecto utiliza el sistema de logging de ESP-IDF para monitorizar la ejecucion en consola:

* ESP_LOGI: Mensajes informativos para el LED Rojo.
* ESP_LOGW: Mensajes de advertencia para el LED Verde.
* ESP_LOGE: Mensajes de error para el LED Azul.

Ademas, la tarea principal (app_main) permanece activa enviando un mensaje de confirmacion cada 500 milisegundos para demostrar que el hilo principal sigue funcionando junto con las tareas creadas.