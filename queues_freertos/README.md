Piensa en una cola como un **Buffer FIFO (First-In, First-Out)** protegido por el sistema operativo. Sirve para que dos hilos de ejecución (tareas) se hablen sin "chocar" entre ellos.

Aquí se explican tres momentos clave del codigo:

---

### 1. La reserva de memoria (El buzón)

En `app_main`, se ejecuta esta línea:
`GlobalQueue = xQueueCreate(10, sizeof(uint32_t));`

Aquí le estamos diciendo al ESP32: *"Resérvame un espacio en la RAM para un buzón que pueda guardar hasta 10 paquetes, y cada paquete será del tamaño de un `uint32_t` (4 bytes)"*.

Sin esto, si la **Tarea A** intentara escribir en una variable mientras la **Tarea B** la lee, tendrías una **condición de carrera** (datos corruptos). La cola evita esto siendo "Thread-Safe".

---

### 2. El Productor: Enviar datos (`vTask_Sender`)

Dentro de tu tarea emisora tienes:
`xQueueSend(GlobalQueue, &count, pdMS_TO_TICKS(1000));`

* **¿Qué hace?**: Toma el valor de la variable `count`, hace una **copia** y la mete en el buzón.
* **¿Por qué el tiempo?**: Ese `pdMS_TO_TICKS(1000)` es vital. Dice: *"Si la cola está llena (porque el receptor es lento), espera hasta 1000ms a que se vacíe un hueco. Si en ese tiempo no hay sitio, sigue adelante y devuelve un error"*.

---

### 3. El Consumidor: Recibir datos (`vTask_Receiver`)

En la tarea receptora haces:
`xQueueReceive(GlobalQueue, &receivedValue, pdMS_TO_TICKS(500));`

Aquí ocurre la verdadera magia del RTOS. Esta línea hace que la **Tarea B** se "duerma" (estado **Blocked**). No gasta ni un ciclo de CPU mientras la cola esté vacía.

En el momento en que la Tarea A hace un `xQueueSend`, el planificador de FreeRTOS "despierta" inmediatamente a la Tarea B porque detecta que ya hay un paquete en su buzón.

---

### ¿Para qué sirve esto en la vida real? (Tu perfil de Teleco)

Imagina tu proyecto de **Visión Artificial** o de **RF**:

1. **Tarea A (Adquisición):** Captura muestras de una señal a altísima velocidad. Su único trabajo es leer el sensor y meter el dato en la cola. No puede perder tiempo procesando.
2. **Tarea B (Procesamiento):** Saca los datos de la cola y aplica un filtro (ej. una FFT o un algoritmo de detección). Si el algoritmo tarda un poco más de lo normal, los datos no se pierden, se quedan "haciendo cola" en el buffer.

**En resumen:** Usamos colas para **desacoplar** tareas. La Tarea A puede ir a su ritmo y la Tarea B al suyo, siempre que la cola tenga tamaño suficiente para absorber las diferencias de velocidad.

En tu código actual, estás enviando un simple contador, pero podrías estar enviando una estructura compleja con lecturas de sensores, estados de error o comandos de control.

Si usaras una variable global, la Tarea B tendría que estar preguntando todo el rato: *"¿ya cambió el valor?"*, desperdiciando muchísima CPU. La cola le permite descansar hasta que hay trabajo real.