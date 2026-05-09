#include <stdio.h>             // libreria para funciones de entrada/salida (ejemplo: printf)
#include "driver/gpio.h"       // libreria para controlar los pines GPIO
#include "freertos/FreeRTOS.h" // libreria para el sistema operativo en tiempo real FreeRTOS
#include "freertos/task.h"     // libreria para las tareas de FreeRTOS
#include "freertos/timers.h"   // libreria para los temporizadores de FreeRTOS
#include "esp_log.h"           // libreria para el sistema de log de ESP-IDF
#include "driver/ledc.h"       // libreria para controlar el LEDC (LED Control) de ESP32

#define LED1 2 // definimos el número del pin GPIO al que está conectado el LED (LED incorporado en la placa)

uint8_t led_level = 0;            // variable para almacenar el estado del LED
uint8_t count = 0;                // variable para contar el numero de veces que el LED ha cambiado de estado
static const char *TAG = "Main";  // etiqueta para las funciones de logging, útil para identificar el origen de los mensajes en la consola
TimerHandle_t timer;              // variable para almacenar el temporizador de FreeRTOS que se utilizará para hacer parpadear el LED cada cierto tiempo
TimerHandle_t xTimer;             // variable para almacenar el handle del temporizador

int interval = 50;  // intervalo de tiempo en milisegundos para hacer parpadear el LED (500 ms = 0.5 segundos)
int timerId = 1;    // identificador del temporizador, se puede utilizar para diferenciar entre múltiples temporizadores si es necesario

// Valor inicial de cada una de las señales PWM para cada uno de los canales (R,G,B) del LED
int dutyR = 0;
int dutyG = 300;
int dutyB = 600;

// Prototipos de las funciones que se utilizarán en el programa
esp_err_t init_led(void);     // Definimios una función para inicializar el LED, configurando el pin GPIO como salida
esp_err_t blink_led(void);    // Definimios una función para hacer parpadear el LED, alternando su estado entre encendido y apagado
esp_err_t set_timer(void);    // Aqui estamos definiendo una función para configurar el temporizador
esp_err_t set_pwm(void);      // Definimios una función para configurar el PWM (Pulse Width Modulation) del LED, lo que nos permitirá controlar la intensidad del LED
esp_err_t set_pwm_duty(void); // Definimos una funcion para configurar la señal (duty) que el LED va a recibir, lo que nos permitirá controlar la intensidad del LED de forma dinámica


// Esta es la función de callback que se ejecutará cada vez que el temporizador expire, haciendo parpadear el LED
void vTimerCallback(TimerHandle_t xTimer){

    dutyR += 10;// incremeentamos el valor de duty del canal
    
    // Tenemos que evitar que el valor de la señal PWM  exceda el límite máximo permitido por la resolución del timer (en este caso, 10 bits, lo que significa que el valor máximo es 1023)
    if(dutyR >= ((1<<10)-1)){ // 2^10 = 1024 -> como es potencia de base 2 podemos calcularlo desplazando el 1 10 veces
        dutyR = 0; 
    }

    dutyG += 10;
    if(dutyG >= ((1<<10)-1)){ // 2^10 = 1024 -> como es potencia de base 2 podemos calcularlo desplazando el 1 10 veces
        dutyG = 0; 
    }

    dutyB += 10;
    if(dutyB >= ((1<<10)-1)){ // 2^10 = 1024 -> como es potencia de base 2 podemos calcularlo desplazando el 1 10 veces
        dutyB = 0; 
    }

    blink_led(); // llamamos a la función para hacer parpadear el LED
    set_pwm_duty(); 
}

void app_main(void)
{
    init_led(); // Llamamos a la función para inicializar el LED
    set_pwm();
    set_timer();
}

/* ¿QUE ES EL GPIO?
 * El GPIO (General Purpose Input/Output) es un pin de la placa que puede ser configurado por el usuario para realizar funciones de entrada o salida.
 * Este pin puede ser utilizado para controlar dispositivos externos, como LEDs, botones, sensores, etc. En este caso, estamos utilizando el GPIO 
 * para controlar un LED conectado al pin 2 de la placa.
 */

// Funcion para inicializar el LED (configuramos el pin GPIO como salida)
esp_err_t init_led(void){
    gpio_reset_pin(LED1);                       // reseteamos el pin GPIO para asegurarnos de que esté en un estado conocido(un estado conocido seria, por ejemplo, que el pin esté apagado)
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT); // configuramos el pin GPIO como salida para poder controlar el LED
    return ESP_OK;                              // retornamos un valor indicando que la función se ejecutó correctamente
}

// Funcion para hacer parpadear el LED (cambiamos el valor del LED y lo aplicamos al pin GPIO)
esp_err_t blink_led(void){
    led_level = !led_level;          // alternamos el estado del LED (si estaba apagado, lo encendemos; si estaba encendido, lo apagamos)
    gpio_set_level(LED1, led_level); // establecemos el nivel lógico del pin GPIO para controlar el estado del LED
    return ESP_OK;                   // devolvemos un valor indicando que la funcion se ejecuto correctamente
}

// Funcion para configurar el temporizador
esp_err_t set_timer(void)
{
    ESP_LOGI(TAG, "Timer init configuration");           // imprimimos un mensaje de log para indicar que estamos configurando el temporizador

    xTimer = xTimerCreate("Timer",                       // Nombre del temporizador (solo para fines de depuración, no es utilizado por el kernel)
                              (pdMS_TO_TICKS(interval)), // periodo del temporizador en ticks (convertimos el intervalo de tiempo en milisegundos a ticks utilizando la macro pdMS_TO_TICKS)
                              pdTRUE,                    // el temporizador se reiniciará automáticamente cada vez que expire (modo auto-reload)
                              (void *)timerId,           // identificador del temporizador (se puede usar cualquier valor único, se utiliza para identificar el temporizador en la función de callback)
                              vTimerCallback             // función de callback que se ejecutará cada vez que el temporizador expire (en este caso, la función vTimerCallback que hace parpadear el LED)
    );

    if (xTimer == NULL)
    {
        // The timer was not created.
        ESP_LOGE(TAG, "The timer was not created.");
    }
    else
    {
        if (xTimerStart(xTimer, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(TAG, "The timer could not be set into the Active state.");
        }
    }

    return ESP_OK;
}

// Funcion para configurar el PWM del LED (esto nos permitirá controlar la intensidad del LED)
esp_err_t set_pwm(void){

    // 1)CONFIGURAMOS LOS CANALES

    // creamos el canal R y lo configuramos
    ledc_channel_config_t channelConfigR = {0}; 
    channelConfigR.gpio_num = 33;                     // seleccionamos el pin GPIO al que está conectado el LED (en este caso, GPIO 33)
    channelConfigR.speed_mode = LEDC_HIGH_SPEED_MODE; // seleccionamos el modo de velocidad para el canal de LEDC (en este caso, modo de alta velocidad)
    channelConfigR.channel = LEDC_CHANNEL_0;          // seleccionamos el canal de LEDC que vamos a utilizar (en este caso, canal 0)
    channelConfigR.intr_type = LEDC_INTR_DISABLE;     // ya que no vamos a usar interrupciones en este ejemplo
    channelConfigR.timer_sel = LEDC_TIMER_0;          // seleccionamos el timer que se utilizará para generar la señal PWM (en este caso, timer 0)
    channelConfigR.duty = 0;                          // la señal de trabajo que el LED va a recibir

    // creamos el canal G y lo configuramos
    ledc_channel_config_t channelConfigG = {0};
    channelConfigG.gpio_num = 25;                     // seleccionamos el pin GPIO al que está conectado el LED (en este caso, GPIO 33)
    channelConfigG.speed_mode = LEDC_HIGH_SPEED_MODE; // seleccionamos el modo de velocidad para el canal de LEDC (en este caso, modo de alta velocidad)
    channelConfigG.channel = LEDC_CHANNEL_1;          // seleccionamos el canal de LEDC que vamos a utilizar (en este caso, canal 0)
    channelConfigG.intr_type = LEDC_INTR_DISABLE;     // ya que no vamos a usar interrupciones en este ejemplo
    channelConfigG.timer_sel = LEDC_TIMER_0;          // seleccionamos el timer que se utilizará para generar la señal PWM (en este caso, timer 0)
    channelConfigG.duty = 0;                          // la señal de trabajo que el LED va a recibir
    
    // creamos el canal B y lo configuramos
    ledc_channel_config_t channelConfigB = {0};
    channelConfigB.gpio_num = 26;                     // seleccionamos el pin GPIO al que está conectado el LED (en este caso, GPIO 33)
    channelConfigB.speed_mode = LEDC_HIGH_SPEED_MODE; // seleccionamos el modo de velocidad para el canal de LEDC (en este caso, modo de alta velocidad)
    channelConfigB.channel = LEDC_CHANNEL_2;          // seleccionamos el canal de LEDC que vamos a utilizar (en este caso, canal 0)
    channelConfigB.intr_type = LEDC_INTR_DISABLE;     // ya que no vamos a usar interrupciones en este ejemplo
    channelConfigB.timer_sel = LEDC_TIMER_0;          // seleccionamos el timer que se utilizará para generar la señal PWM (en este caso, timer 0)
    channelConfigB.duty = 0;                          // la señal de trabajo que el LED va a recibir

    // configuramos el PWM para cada uno de los canales (R,G,B)
    ledc_channel_config(&channelConfigR);  // Usamos puntero al canal R
    ledc_channel_config(&channelConfigG);  // Usamos puntero al canal G
    ledc_channel_config(&channelConfigB);  // Usamos puntero al canal B


    // 2) CONFIGURAMOS LOS TIMERS DE LOS CANALES

    ledc_timer_config_t timerConfg = {0}; // creamos el el timer global para todos los canales (R,G,B)
    timerConfg.speed_mode = LEDC_HIGH_SPEED_MODE;
    timerConfg.duty_resolution = LEDC_TIMER_10_BIT; // es la cantdad de bits del ADC. En este caso trabajaremos con 10 bits de resolucion
    timerConfg.timer_num = LEDC_TIMER_0; 
    timerConfg.freq_hz = 20000; // trabajaremos con 20 kHz

    ledc_timer_config(&timerConfg); // Usamos un puntero al timer que hemos creado

    return ESP_OK;
}

// Funcion para setear y actualzar el valor de la señal PWM para cada uno de los canales (R,G,B)
esp_err_t set_pwm_duty(void){ // Usaremos la funcion ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

    // Configuramos la señal PWM para cada uno de los canales
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, dutyR);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, dutyG);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, dutyB);

    // Para que el PWM actualice el valor tenemos que usar la siguiente funcion: ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

    return ESP_OK;
}