# 🧤 SignGlove — Guante Háptico para Traducción del Abecedario en Lenguaje de Señas (MVP)

[![Estado](https://img.shields.io/badge/Estado-Release%201%20·%20MVP-blue)]()
[![Plataforma](https://img.shields.io/badge/Plataforma-ESP32-green)]()
[![OS](https://img.shields.io/badge/OS-FreeRTOS-orange)]()
[![Licencia](https://img.shields.io/badge/Licencia-MIT-yellow)]()

> **Materia:** Sensores y Actuadores / Internet de las Cosas (IoT)

---

## 📋 Tabla de Contenidos

- [Visión del Proyecto](#-visión-del-proyecto)
- [El Problema con los Sensores y la Solución "Hombre de Paja"](#-el-problema-con-los-sensores-y-la-solución-hombre-de-paja)
- [Implementación de los Temas del Curso](#-implementación-de-los-temas-del-curso)
- [Arquitectura y Restricciones](#-arquitectura-y-restricciones)
- [Especificaciones de Hardware](#-especificaciones-de-hardware)
- [Presupuesto Económico](#-presupuesto-económico)
- [Definición del MVP](#-definición-del-mvp)
- [Equipo](#-equipo)

---

## 🎯 Visión del Proyecto

### Problema
En Colombia, aproximadamente **500.000 personas** padecen algún grado de discapacidad auditiva (DANE, 2018). La comunicación con personas oyentes que no dominan la lengua de señas colombiana (LSC) representa una barrera significativa. Las soluciones existentes —como intérpretes presenciales o aplicaciones de video— no siempre están disponibles en tiempo real ni son portátiles.

### Solución Propuesta
**SignGlove** es un guante instrumentado con **sensores flex** que detecta las posiciones de los dedos correspondientes a cada letra del abecedario en lenguaje de señas. Los datos capturados son procesados por un **ESP32**, transmitidos vía **Wi-Fi** a un servidor/aplicación, y traducidos a texto legible en una interfaz gráfica.

### Usuarios Objetivo
| Segmento | Descripción |
|---|---|
| **Primario** | Personas con discapacidad auditiva que se comunican con lenguaje de señas |
| **Secundario** | Familiares, compañeros de clase/trabajo y personal de atención al público |
| **Terciario** | Instituciones educativas que buscan herramientas de inclusión |

---

## 🚧 El Problema con los Sensores y la Solución "Hombre de Paja"

**El plan original:**
La idea principal era utilizar sensores Flex de 2.2" conectados a los pines 32 (índice) y 33 (pulgar) del ESP32, aprovechando divisores de voltaje para medir la flexión de los dedos.

**El problema técnico encontrado:**
1. **Falla de hardware:** Los sensores flex sufrieron daños físicos durante el desarrollo y las pruebas iniciales.
2. **Limitación física de detección:** Las letras **L**, **G** y **Q** en lenguaje de señas requieren que el dedo índice y el pulgar estén completamente rectos. La diferencia real entre estas letras radica en la *rotación de la muñeca*. Dado que un sensor flex solo mide la flexión, físicamente entregaría la misma resistencia para las tres letras, haciendo imposible diferenciarlas solo con flexómetros.

**La solución (Simulador / "Hombre de Paja"):**
Para continuar con el desarrollo de la infraestructura IoT (FreeRTOS, MQTT, Web Server), implementamos un "hombre de paja" en el firmware:
*   Se crearon umbrales de resistencia (valores de ADC) imaginarios para simular la detección:
    *   **L:** Valores de ADC alrededor de 10,000 (Rango de simulación)
    *   **G:** Valores de ADC alrededor de 15,000 (Rango de simulación)
    *   **Q:** Valores de ADC alrededor de 20,000 (Rango de simulación)
*   Se dejó un mecanismo de **"override" por consola serial** donde el desarrollador puede enviar datos (ej. `10000,10000`) para simular instantáneamente las lecturas, forzando al ESP32 a procesar la letra correspondiente sin depender del hardware físico averiado.

---

## 💻 Implementación de los Temas del Curso

El código del ESP32 (`main.ino`) integra los siguientes temas abordados en la bibliografía y clases, adaptados a la realidad del producto final:

### 1. FreeRTOS (vs. MicroPython)
*   Se optó por C++ (Arduino Core) impulsado por **FreeRTOS** en lugar de MicroPython. Esto permite un *Preemptive Scheduler* nativo que aprovecha los dos núcleos del ESP32.
*   **Aplicación:** El sistema se dividió en 3 tareas concurrentes (`TaskSensors`, `TaskNetwork`, `TaskWebServer`). Mientras el núcleo 0 maneja el pesado cifrado TLS de la red, el núcleo 1 lee los sensores sin bloqueos. Se usan colas de FreeRTOS (`xQueueCreate`) para transferir de manera segura la letra detectada entre las tareas.

### 2. Aprovisionamiento de red Wi-Fi
*   Los dispositivos IoT no deben tener contraseñas "quemadas" (hardcoded). 
*   **Aplicación:** Se integró la librería `WiFiManager`. Si el ESP32 no encuentra su red habitual, levanta un Access Point (AP) temporal llamado **"GuanteIoT_AP"**. El usuario se conecta y mediante un portal cautivo le proporciona las nuevas credenciales de Wi-Fi, quedando guardadas en memoria no volátil.

### 3. MQTT y Certificados
*   Transmitir telemetría IoT en texto plano es inseguro.
*   **Aplicación:** Se utiliza `PubSubClient` y `WiFiClientSecure` conectándose al puerto **8883 (TLS)** de HiveMQ. El código valida criptográficamente la conexión incluyendo el certificado Root CA (*ISRG Root X1* de Let's Encrypt). La letra detectada se publica bajo un formato JSON en el tópico `guante/iot/letras`.

### 4. Servidor Web Local
*   **Aplicación:** Una tarea de FreeRTOS levanta un `WebServer` HTTP en el puerto 80 del ESP32. Sirve un Dashboard (HTML/CSS/JS) estéticamente moderno (modo oscuro y colores neón) que, usando peticiones asíncronas (`fetch` / AJAX), se actualiza en tiempo real reflejando la seña sin recargar la página.

### 5. Cliente HTTP
*   **Aplicación:** Cuando se procesa una letra desde la cola, la tarea de red instancia temporalmente la librería `HTTPClient`, hace un GET a un servidor de pruebas remoto (`httpbin.org/get?letra=X`), imprime el código de respuesta en consola y destruye la conexión para liberar memoria.

---

## 🏗 Arquitectura y Flujo de Datos

```
┌───────────────────────────────────────────────────────────────────────┐
│                     CAPA DE PROCESAMIENTO (ESP32)                     │
│                                                                       │
│  ┌────────────────┐     ┌──────────────┐     ┌─────────────────────┐  │
│  │  Sensores /    │     │  Clasificador│     │  Módulos de Red     │  │
│  │  Simulador     │ --► │ (Umbrales)   │ --► │ (Wi-Fi, MQTT, HTTP) │  │
│  │ (ADC Pines 32,33)│   │              │     │  FreeRTOS Queues    │  │
│  └────────────────┘     └──────────────┘     └────────────┬────────┘  │
└───────────────────────────────────────────────────────────┼───────────┘
                                                            │ Wi-Fi (HTTP/TLS)
                                                            ▼
┌───────────────────────────────────────────────────────────────────────┐
│                    CAPA DE APLICACIÓN                                 │
│                                                                       │
│  ┌─────────────────────────┐     ┌──────────────────────────────┐     │
│  │  Broker MQTT Seguro     │     │   Interfaz Web Dashboard     │     │
│  │  (TLS / Certificados)   │     │  (Embebida en el ESP32)      │     │
│  └─────────────────────────┘     └──────────────────────────────┘     │
└───────────────────────────────────────────────────────────────────────┘
```

---

## 🔩 Especificaciones de Hardware (Referencia MVP Físico)

*   **Microcontrolador:** ESP32-WROOM-32 (Dual-core, Wi-Fi 802.11 b/g/n, 3.3V).
*   **Sensores Flex:** Spectra Symbol SEN-10264 (2.2"). Resistencia plana de 10 kΩ, variable a ≥20 kΩ en flexión.
*   **Acondicionamiento:** Divisor de voltaje con resistencia fija de 10 kΩ. El voltaje resultante (1.65V a 2.64V) es leído por los ADC (0-4095) del ESP32.

---

## 💰 Presupuesto Económico Estimado

| Componente | Cantidad | Precio Unitario (COP) | Subtotal (COP) |
|---|---|---|---|
| Sensor Flex 2.2" (SEN-10264) | 5 (Referencia completa) | $25.000 | $125.000 |
| ESP32-WROOM-32 DevKit | 1 | $35.000 | $35.000 |
| Resistencias 10 kΩ (1/4W) | 10 | $200 | $2.000 |
| Guante base, protoboard, cables | Varios | $23.000 | $23.000 |
| Batería LiPo 3.7V + TP4056 | 1 | $21.000 | $21.000 |
| **TOTAL** | | | **$206.000** |

---

## 🚀 Definición del MVP y Funcionalidades Logradas

| ID | Feature | Estado en Firmware Actual |
|---|---|---|
| F-01 | Lectura de sensores | ✅ Completado (Pines 32 y 33 / Simulación por Serial) |
| F-02 | Clasificación por umbrales | ✅ Completado (Lógica en `TaskSensors` para L, G, Q) |
| F-03 | Aprovisionamiento Wi-Fi | ✅ Completado (`WiFiManager` integrado) |
| F-04 | Transmisión Segura | ✅ Completado (MQTT con TLS `WiFiClientSecure`) |
| F-05 | Interfaz web básica | ✅ Completado (Servidor Web asíncrono en puerto 80) |
| F-06 | Peticiones HTTP | ✅ Completado (`HTTPClient` implementado) |
| F-07 | RTOS (Concurrencia) | ✅ Completado (Tres tareas separadas y colas de mensajes) |

---

## 👥 Equipo

| Nombre | Rol | GitHub |
|---|---|---|
| Daniel Felipe Forero| Técnico | [DanielForero14](https://github.com/DanielForero14) |
| Juan David Orozco | Arquitecto | [David OrozcoJ](https://github.com/David-OrozcoJ) |
| Joao Alexandre Muño Obando | Desarrollador | [JoaoALT](https://github.com/JoaoALT) |
| Sergio Gabriel Nieto Mendez | Desarrollador | [seralnime](https://github.com/seralnime) |

---

<p align="center">
  <em>SignGlove — Comunicación sin barreras 🤟</em>
</p>
