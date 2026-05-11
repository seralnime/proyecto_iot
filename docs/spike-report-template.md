# 🔬 Reporte de Spike — Release 1

## Información General

| Campo | Valor |
|---|---|
| **Fecha de ejecución** | ___/___/2026 |
| **Participantes** | Daniel Felipe Forero, Juan David Orozco, Sergio Gabriel Nieto, Joao Alexandre Muñoz |
| **Duración** | Sprint 1 (Semana 1) |
| **Objetivo** | Validar la lectura de sensores flex en ESP32 con Wi-Fi activo simultáneamente |

---

## 1. Objetivo del Spike

Responder la pregunta:

> **¿Es posible leer 5 sensores flex de manera confiable en el ESP32 mientras Wi-Fi está activo, con suficiente resolución y velocidad para clasificar letras del abecedario en tiempo real?**

### Sub-preguntas a resolver:
1. ¿Funcionan las lecturas ADC1 con Wi-Fi habilitado?
2. ¿Cuánto ruido introduce el Wi-Fi en las lecturas ADC?
3. ¿Cuál es la latencia de transmisión de datos por Wi-Fi?
4. ¿Son las lecturas suficientemente repetibles para distinguir posiciones de dedos?

---

## 2. Setup Experimental

### 2.1 Hardware Utilizado
| Componente | Especificación |
|---|---|
| Microcontrolador | ESP32-WROOM-32 DevKit v1 |
| Sensor flex | Spectra Symbol SEN-10264 (2.2") |
| Resistencia fija | 10 kΩ (divisor de voltaje) |
| Alimentación | USB 5V → regulador 3.3V del DevKit |

### 2.2 Software / IDE
- **IDE:** [Arduino IDE / PlatformIO]
- **Framework:** Arduino-ESP32 v[X.X.X]
- **Librerías:** WiFi.h, WebServer.h / AsyncWebSocket

### 2.3 Circuito
> [Insertar foto del circuito armado y/o esquemático]

```
3.3V — [10kΩ] — Punto medio → GPIO [XX] (ADC1) — [Flex Sensor] — GND
```

### 2.4 Código de Prueba
> [Enlace al archivo de código usado para el spike, ej: `/firmware/spike/spike_test.ino`]

---

## 3. Resultados

### 3.1 Lectura ADC sin Wi-Fi

| Posición del dedo | R_flex estimada | Lectura ADC (crudo) | Voltaje calculado |
|---|---|---|---|
| Extendido (0°) | ~___ kΩ | ___ | ___ V |
| Semi-flexionado (90°) | ~___ kΩ | ___ | ___ V |
| Totalmente flexionado (180°) | ~___ kΩ | ___ | ___ V |

### 3.2 Lectura ADC con Wi-Fi Activo

| Pregunta | Resultado |
|---|---|
| ¿Funciona ADC1 con Wi-Fi? | [Sí / No] |
| ¿Hay ruido adicional? | [Sí / No — cuantificar] |
| Diferencia promedio vs. sin Wi-Fi | ±___ unidades ADC |

**Tabla comparativa (10 lecturas en posición extendida):**

| Lectura # | Sin Wi-Fi | Con Wi-Fi | Diferencia |
|---|---|---|---|
| 1 | | | |
| 2 | | | |
| ... | | | |
| 10 | | | |
| **Promedio** | | | |
| **Desv. Std** | | | |

### 3.3 Transmisión Wi-Fi

| Métrica | Valor |
|---|---|
| Protocolo | [HTTP GET / WebSocket / UDP] |
| Tamaño del payload | ___ bytes |
| Latencia promedio (10 envíos) | ___ ms |
| Latencia máxima | ___ ms |
| Latencia mínima | ___ ms |
| Paquetes perdidos | ___/10 (___%) |

### 3.4 Repetibilidad

**10 lecturas consecutivas en la misma posición (dedo extendido):**

| Lectura # | Valor ADC |
|---|---|
| 1 | |
| 2 | |
| ... | |
| 10 | |

- **Promedio:** ___
- **Desviación estándar:** ___
- **Coeficiente de variación:** ___%
- **¿Dentro de ±5% del rango?** [Sí / No]

---

## 4. Criterios de Éxito — Evaluación

| # | Criterio | ¿Cumple? | Notas |
|---|---|---|---|
| 1 | Lectura estable de 1 sensor en ADC1 | [✅/❌] | |
| 2 | ADC1 funciona con Wi-Fi activo | [✅/❌] | |
| 3 | Se distinguen ≥3 posiciones del dedo | [✅/❌] | |
| 4 | Transmisión Wi-Fi < 500 ms | [✅/❌] | |
| 5 | Lecturas repetibles (±5% del rango) | [✅/❌] | |

**Resultado global:** [✅ SPIKE EXITOSO / ❌ SPIKE FALLIDO — requiere mitigación]

---

## 5. Conclusiones

[Escribir las conclusiones basadas en los datos recolectados]

- ¿El proyecto es viable técnicamente?
- ¿Qué limitaciones se descubrieron?
- ¿Qué ajustes al plan original son necesarios?

---

## 6. Decisiones Tomadas

| # | Decisión | Justificación |
|---|---|---|
| 1 | [Ej: Usar GPIO 32–36 exclusivamente] | [ADC2 confirmado incompatible con Wi-Fi] |
| 2 | [Ej: Usar WebSocket en lugar de HTTP] | [Latencia HTTP > 300ms, WS < 50ms] |
| 3 | | |

---

## 7. Próximos Pasos

- [ ] [Acción derivada del spike]
- [ ] [Acción derivada del spike]
- [ ] [Acción derivada del spike]

---

## 8. Evidencia

> Adjuntar o enlazar fotos, capturas de pantalla del serial monitor, y/o videos de las pruebas.

| Tipo | Archivo / Enlace |
|---|---|
| Foto del circuito | |
| Captura Serial Monitor | |
| Video de prueba | |
