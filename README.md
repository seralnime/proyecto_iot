# 🧤 SignGlove — Guante Háptico para Traducción del Abecedario en Lenguaje de Señas

[![Estado](https://img.shields.io/badge/Estado-Release%201%20·%20Fundamentos-blue)]()
[![Plataforma](https://img.shields.io/badge/Plataforma-ESP32-green)]()
[![Licencia](https://img.shields.io/badge/Licencia-MIT-yellow)]()

> **Materia:** Sensores y Actuadores / Internet de las Cosas (IoT)

---

## 📋 Tabla de Contenidos

- [Visión del Proyecto](#-visión-del-proyecto)
- [Arquitectura y Restricciones](#-arquitectura-y-restricciones)
- [Especificaciones de Hardware](#-especificaciones-de-hardware)
- [Presupuesto Económico](#-presupuesto-económico)
- [Definición del MVP](#-definición-del-mvp)
- [Cronograma de Sprints y Releases](#-cronograma-de-sprints-y-releases)
- [Spike Arquitectónico](#-spike-arquitectónico--gestión-de-riesgos)
- [Reporte del Spike](#-reporte-del-spike)
- [Equipo](#-equipo)

---

## 🎯 Visión del Proyecto

### Problema

En Colombia, aproximadamente **500.000 personas** padecen algún grado de discapacidad auditiva (DANE, 2018). La comunicación con personas oyentes que no dominan la lengua de señas colombiana (LSC) representa una barrera significativa en entornos educativos, laborales y sociales. Las soluciones existentes —como intérpretes presenciales o aplicaciones de video— no siempre están disponibles en tiempo real ni son portátiles.

### Solución Propuesta

**SignGlove** es un guante instrumentado con **sensores flex** que detecta las posiciones de los dedos correspondientes a cada letra del abecedario en lenguaje de señas. Los datos capturados son procesados por un **ESP32**, transmitidos vía **Wi-Fi** a un servidor/aplicación, y traducidos a texto legible en una interfaz gráfica.

### Usuarios Objetivo

| Segmento | Descripción |
|---|---|
| **Primario** | Personas con discapacidad auditiva que se comunican con lenguaje de señas |
| **Secundario** | Familiares, compañeros de clase/trabajo y personal de atención al público |
| **Terciario** | Instituciones educativas que buscan herramientas de inclusión |

### Propuesta de Valor

- **Portabilidad:** Dispositivo vestible que no requiere cámaras ni infraestructura especial.
- **Tiempo real:** Traducción con latencia menor a 500 ms (objetivo).
- **Accesibilidad:** Costo significativamente menor que soluciones comerciales existentes.
- **Conectividad IoT:** Transmisión inalámbrica que permite integración con múltiples dispositivos.

---

## 🏗 Arquitectura y Restricciones

### Diagrama de Bloques

```
┌─────────────────────────────────────────────────────────────────────┐
│                        CAPA FÍSICA (GUANTE)                         │
│                                                                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────┐ │
│  │ Flex Sen.│  │ Flex Sen.│  │ Flex Sen.│  │ Flex Sen.│  │Flex S│ │
│  │  Pulgar  │  │  Índice  │  │  Medio   │  │  Anular  │  │Meñiq.│ │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘  └──┬───┘ │
│       │              │              │              │            │     │
│  ┌────▼──────────────▼──────────────▼──────────────▼────────────▼──┐ │
│  │              Divisores de Voltaje (10kΩ c/u)                   │ │
│  └────────────────────────────┬────────────────────────────────────┘ │
└───────────────────────────────┼──────────────────────────────────────┘
                                │ Señales Analógicas (0–3.3V)
                                ▼
┌───────────────────────────────────────────────────────────────────────┐
│                     CAPA DE PROCESAMIENTO                            │
│                                                                      │
│  ┌────────────────────────────────────────────────────────────────┐  │
│  │                     ESP32-WROOM-32                             │  │
│  │  ┌──────────┐  ┌──────────────┐  ┌─────────────────────────┐  │  │
│  │  │ ADC 12b  │→ │ Clasificador │→ │ Módulo Wi-Fi (802.11)  │  │  │
│  │  │ (5 can.) │  │ (Umbrales /  │  │ TCP/UDP → Servidor     │  │  │
│  │  │          │  │  ML on-edge) │  │                         │  │  │
│  │  └──────────┘  └──────────────┘  └────────────┬────────────┘  │  │
│  └───────────────────────────────────────────────┼───────────────┘  │
└──────────────────────────────────────────────────┼──────────────────┘
                                                   │ Wi-Fi (HTTP/WS)
                                                   ▼
┌───────────────────────────────────────────────────────────────────────┐
│                    CAPA DE APLICACIÓN                                 │
│                                                                      │
│  ┌─────────────────────────┐     ┌──────────────────────────────┐   │
│  │   Servidor / Backend    │     │   Interfaz de Usuario (UI)   │   │
│  │  (Python / Node.js)     │────▶│  - Visualización en tiempo   │   │
│  │  - Recibe datos Wi-Fi   │     │    real de la letra/texto    │   │
│  │  - Log de traducciones  │     │  - Web App / App móvil       │   │
│  └─────────────────────────┘     └──────────────────────────────┘   │
└───────────────────────────────────────────────────────────────────────┘
```

### Flujo de Datos

1. **Captura:** Los 5 sensores flex miden la curvatura de cada dedo.
2. **Acondicionamiento:** Divisores de voltaje convierten variación de resistencia → voltaje (0–3.3V).
3. **Digitalización:** El ADC de 12 bits del ESP32 convierte las señales (resolución: 3.3V/4096 ≈ 0.8 mV).
4. **Clasificación:** Un algoritmo en el ESP32 mapea las lecturas a una letra del abecedario.
5. **Transmisión:** La letra identificada se envía vía Wi-Fi al servidor/aplicación.
6. **Visualización:** La interfaz muestra la letra traducida en tiempo real.

### Restricciones del Hardware

| Recurso | Especificación | Impacto en el Diseño |
|---|---|---|
| **Memoria Flash ESP32** | 4 MB | Limita el tamaño del modelo de clasificación (~100 KB máx. para TFLite Micro) |
| **RAM ESP32** | 520 KB SRAM | Debe compartirse entre stack Wi-Fi (~100 KB), buffers ADC y lógica de clasificación |
| **ADC ESP32** | 12 bits, 2 canales SAR | Solo ADC1 (GPIO 32–39) es usable con Wi-Fi activo; ADC2 tiene conflicto con Wi-Fi |
| **Consumo ESP32** | ~240 mA (Wi-Fi activo) | Requiere batería LiPo de mínimo 1000 mAh para ~4h de uso continuo |
| **Sensores Flex** | 10kΩ plano, ≥20kΩ flexionado, ±30% tolerancia | Necesita calibración individual por dedo; el rango ±30% exige rutina de calibración al encender |
| **Ciclo de vida sensor** | >1M flexiones | Viable para uso extensivo (~500 señas/día × 365 días = 182K flexiones/año) |
| **Voltaje operación** | 3.3V (ESP32) | Compatible directo con divisores de voltaje de los flex sensors |
| **Temperatura** | -35°C a +80°C (sensor) | Sin restricción para uso en interiores |

---

## 🔩 Especificaciones de Hardware

### Sensor Flex — Spectra Symbol SEN-10264

| Parámetro | Valor |
|---|---|
| Longitud | 2.2" (55.37 mm) |
| Resistencia plana | 10 kΩ (±30%) |
| Resistencia en flexión (180°) | ≥ 20 kΩ (mínimo 2× la plana) |
| Ciclo de vida | > 1.000.000 flexiones |
| Potencia nominal | 0.5 W continuo / 1 W pico |
| Temperatura de operación | -35°C a +80°C |
| Tipo | Sensor pasivo, analógico, resistivo |
| Sensibilidad | ΔR/Δθ — proporcional al ángulo |
| Repetibilidad | ±2% en resistencia nominal |

### Microcontrolador — ESP32-WROOM-32

| Parámetro | Valor |
|---|---|
| Procesador | Dual-core Xtensa LX6, hasta 240 MHz |
| Flash | 4 MB |
| SRAM | 520 KB |
| Wi-Fi | 802.11 b/g/n |
| ADC | 2× SAR ADC de 12 bits (18 canales) |
| GPIO | 34 pines |
| Voltaje de operación | 3.3V |
| Consumo típico | ~80 mA (activo), ~240 mA (Wi-Fi TX) |

### Acondicionamiento de Señal

Cada sensor flex se conecta en un **divisor de voltaje** con una resistencia fija de **10 kΩ**:

```
         3.3V
          │
       ┌──┴──┐
       │ R_fija│ 10 kΩ
       │      │
       └──┬──┘
          │
          ├──────── → GPIO (ADC1) del ESP32
          │
       ┌──┴──┐
       │R_flex │ 10–40 kΩ (variable)
       │      │
       └──┬──┘
          │
         GND
```

**Fórmula del voltaje de salida:**

```
V_out = 3.3V × R_flex / (R_fija + R_flex)
```

| Estado del dedo | R_flex (típico) | V_out (típico) |
|---|---|---|
| Extendido | ~10 kΩ | ~1.65 V |
| Flexionado 90° | ~22 kΩ | ~2.27 V |
| Flexionado 180° | ~40 kΩ | ~2.64 V |

---

## 💰 Presupuesto Económico

| Componente | Cantidad | Precio Unitario (COP) | Subtotal (COP) |
|---|---|---|---|
| Sensor Flex 2.2" (SEN-10264) | 5 | $25.000 | $125.000 |
| ESP32-WROOM-32 DevKit | 1 | $35.000 | $35.000 |
| Resistencias 10 kΩ (1/4W) | 10 | $200 | $2.000 |
| Protoboard / PCB perforada | 1 | $8.000 | $8.000 |
| Cables Dupont / jumper | 1 paquete | $5.000 | $5.000 |
| Guante base (tela) | 1 | $10.000 | $10.000 |
| Batería LiPo 3.7V 1000mAh | 1 | $18.000 | $18.000 |
| Módulo cargador TP4056 | 1 | $3.000 | $3.000 |
| Soldadura, termoencogible, insumos | — | $10.000 | $10.000 |
| **TOTAL** | | | **$216.000** |

> **Nota:** Precios aproximados del mercado colombiano (2026). No incluye envío.

---

## 🚀 Definición del MVP

### Features Must-Have (MVP)

Estas son las funcionalidades **estrictamente necesarias** para que el proyecto funcione:

| ID | Feature | Descripción | Prioridad |
|---|---|---|---|
| F-01 | Lectura de 5 sensores | Leer simultáneamente los 5 sensores flex vía ADC del ESP32 | 🔴 Must-Have |
| F-02 | Calibración inicial | Rutina de calibración al encender para compensar la tolerancia ±30% de cada sensor | 🔴 Must-Have |
| F-03 | Clasificación por umbrales | Algoritmo que mapea combinaciones de lecturas a letras del abecedario (A–Z estáticas) | 🔴 Must-Have |
| F-04 | Transmisión Wi-Fi | Envío de la letra identificada al servidor vía HTTP o WebSocket | 🔴 Must-Have |
| F-05 | Interfaz web básica | Página web que muestra la letra traducida en tiempo real | 🔴 Must-Have |
| F-06 | Alimentación portátil | Alimentación del sistema con batería LiPo recargable | 🔴 Must-Have |
| F-07 | Montaje en guante | Integración física de sensores y electrónica en un guante vestible | 🔴 Must-Have |

### Features Nice-to-Have

Mejoras opcionales que aportan valor adicional:

| ID | Feature | Descripción | Prioridad |
|---|---|---|---|
| F-08 | Clasificación con ML (TFLite Micro) | Reemplazar umbrales fijos por un modelo de Machine Learning embebido | 🟡 Nice-to-Have |
| F-09 | Formación de palabras | Acumular letras para formar palabras con detección de pausa entre señas | 🟡 Nice-to-Have |
| F-10 | Text-to-Speech | Reproducir audio de la letra/palabra traducida | 🟡 Nice-to-Have |
| F-11 | App móvil | Aplicación Android/iOS en lugar de interfaz web | 🟡 Nice-to-Have |
| F-12 | Dashboard de métricas | Panel con estadísticas: precisión, uso, historial de traducciones | 🟢 Nice-to-Have |
| F-13 | Modo offline (BLE) | Transmisión por Bluetooth Low Energy cuando no haya Wi-Fi | 🟢 Nice-to-Have |
| F-14 | Sensor IMU (acelerómetro) | Añadir MPU6050 para detectar orientación de la mano (letras dinámicas) | 🟢 Nice-to-Have |

---

## 📅 Cronograma de Sprints y Releases

### Vista General

```
Semana:  1    2    3    4    5    6    7    8
        ├────┼────┼────┼────┼────┼────┼────┼────┤
        │ S1 │ S2 │ S3 │ S4 │ S5 │ S6 │ S7 │ S8 │
        ├─────────┼─────────┼─────────┼─────────┤
        │Release 1│Release 2│Release 3│Release 4│
        │Fundament│ Core MVP│  Integr.│  Pulido │
        │ + Spike │         │         │ + Demo  │
```

---

### 📦 Release 1 — Fundamentos y Viabilidad (Semanas 1–2)

**Objetivo:** Garantizar la viabilidad técnica del proyecto y establecer la base.

#### Sprint 1 (Semana 1) — Spike y Setup

| Tarea | Asignado a | Estado |
|---|---|---|
| Configurar repositorio GitHub + tablero de proyecto | Todo el equipo | ⬜ Por hacer |
| Redactar README.md (visión, arquitectura, presupuesto) | Todo el equipo | ⬜ Por hacer |
| **SPIKE:** Conectar 1 sensor flex al ESP32 y leer valores ADC | Todo el equipo | ⬜ Por hacer |
| **SPIKE:** Verificar lectura ADC1 con Wi-Fi activo simultáneamente | Todo el equipo | ⬜ Por hacer |
| **SPIKE:** Probar transmisión de datos por Wi-Fi (HTTP básico) | Todo el equipo | ⬜ Por hacer |
| Documentar resultados del spike | Todo el equipo | ⬜ Por hacer |

#### Sprint 2 (Semana 2) — Validación y Planning

| Tarea | Asignado a | Estado |
|---|---|---|
| Conectar los 5 sensores flex al ESP32 (validar pines ADC1) | Todo el equipo | ⬜ Por hacer |
| Implementar rutina básica de calibración (min/max por sensor) | Todo el equipo | ⬜ Por hacer |
| Definir tabla de mapeo inicial: lecturas → letras (al menos 5 letras) | Todo el equipo | ⬜ Por hacer |
| Preparar retrospectiva Release 1 | Todo el equipo | ⬜ Por hacer |
| 📋 **Retrospectiva Release 1 con profesor** | Todo el equipo | ⬜ Por hacer |

**Entregable Release 1:** Spike completado + README.md + Backlog priorizado + Certeza de viabilidad técnica.

---

### 📦 Release 2 — Core MVP (Semanas 3–4)

**Objetivo:** Construir el núcleo funcional del guante.

#### Sprint 3 (Semana 3)

| Tarea | Asignado a | Estado |
|---|---|---|
| Implementar clasificación completa por umbrales (A–Z estáticas) | — | ⬜ Por hacer |
| Diseñar protocolo de comunicación Wi-Fi (formato de mensaje JSON) | — | ⬜ Por hacer |
| Crear servidor backend básico (Python/Node.js) que reciba datos | — | ⬜ Por hacer |

#### Sprint 4 (Semana 4)

| Tarea | Asignado a | Estado |
|---|---|---|
| Desarrollar interfaz web básica (visualización de letra en tiempo real) | — | ⬜ Por hacer |
| Integrar ESP32 → servidor → interfaz web (flujo end-to-end) | — | ⬜ Por hacer |
| Pruebas de latencia y precisión del sistema completo | — | ⬜ Por hacer |
| 📋 **Retrospectiva Release 2 con profesor** | Todo el equipo | ⬜ Por hacer |

**Entregable Release 2:** Sistema funcional que traduce al menos 10 letras del abecedario en tiempo real.

---

### 📦 Release 3 — Integración y Mejoras (Semanas 5–6)

**Objetivo:** Completar el abecedario, integrar batería, montar en guante físico.

#### Sprint 5 (Semana 5)

| Tarea | Asignado a | Estado |
|---|---|---|
| Completar clasificación de todo el abecedario estático | — | ⬜ Por hacer |
| Integrar alimentación por batería LiPo + cargador TP4056 | — | ⬜ Por hacer |
| Montar sensores y electrónica en el guante físico | — | ⬜ Por hacer |

#### Sprint 6 (Semana 6)

| Tarea | Asignado a | Estado |
|---|---|---|
| Mejorar interfaz web (diseño, historial, UX) | — | ⬜ Por hacer |
| Pruebas de usabilidad con usuarios reales | — | ⬜ Por hacer |
| Implementar nice-to-have seleccionados (si hay tiempo) | — | ⬜ Por hacer |
| 📋 **Retrospectiva Release 3 con profesor** | Todo el equipo | ⬜ Por hacer |

**Entregable Release 3:** Guante funcional completo con abecedario, batería, y web UI lista.

---

### 📦 Release 4 — Pulido y Entrega Final (Semanas 7–8)

**Objetivo:** Feature Freeze. Estabilizar, documentar, preparar pitch.

#### Sprint 7 (Semana 7)

| Tarea | Asignado a | Estado |
|---|---|---|
| 🧊 **Feature Freeze** — No nuevas funcionalidades | Todo el equipo | ⬜ Por hacer |
| Corrección de errores y estabilización | Todo el equipo | ⬜ Por hacer |
| Optimizar latencia y precisión de clasificación | — | ⬜ Por hacer |
| Escribir documentación técnica final | — | ⬜ Por hacer |

#### Sprint 8 (Semana 8)

| Tarea | Asignado a | Estado |
|---|---|---|
| Preparar demostración / pitch | Todo el equipo | ⬜ Por hacer |
| Grabar video demo del proyecto | Todo el equipo | ⬜ Por hacer |
| Actualizar README.md final con resultados | Todo el equipo | ⬜ Por hacer |
| 📋 **Entrega final del curso** | Todo el equipo | ⬜ Por hacer |

**Entregable Release 4:** Producto terminado + documentación + pitch.

---

## 🔬 Spike Arquitectónico — Gestión de Riesgos

### Incertidumbre Técnica Identificada

La **mayor incertidumbre técnica** del proyecto es:

> **¿Es posible leer 5 sensores flex de manera confiable en el ESP32 mientras Wi-Fi está activo, con suficiente resolución y velocidad para clasificar letras del abecedario en tiempo real?**

Esta incertidumbre se compone de tres riesgos clave:

| # | Riesgo | Severidad | Probabilidad | Mitigación |
|---|---|---|---|---|
| R1 | **ADC2 no funciona con Wi-Fi activo** — El ESP32 comparte recursos entre ADC2 y el módulo Wi-Fi | 🔴 Alta | 🔴 Alta (documentado) | Usar exclusivamente pines **ADC1** (GPIO 32, 33, 34, 35, 36) para los 5 sensores |
| R2 | **Tolerancia ±30% impide distinguir letras** — Variación entre sensores dificulta umbrales fijos | 🔴 Alta | 🟡 Media | Implementar calibración individual por sensor al encendido |
| R3 | **Latencia Wi-Fi excede requisito de 500ms** — Transmisión lenta arruina experiencia de tiempo real | 🟡 Media | 🟢 Baja | Usar WebSocket en lugar de HTTP polling; enviar solo la letra (payload mínimo) |
| R4 | **Señas ambiguas con solo 5 sensores flex** — Algunas letras requieren orientación de la mano, no solo flexión de dedos | 🟡 Media | 🟡 Media | Limitar MVP a letras estáticas distinguibles solo por flexión; evaluar IMU en Release 3 |

### Criterios de Éxito del Spike

El Spike se considera **exitoso** si se cumplen TODOS los siguientes criterios:

- [ ] Se leen valores estables de al menos 1 sensor flex en un pin ADC1 del ESP32
- [ ] La lectura ADC funciona correctamente **con Wi-Fi activo simultáneamente**
- [ ] Se distinguen al menos **3 posiciones distintas** del sensor (extendido, medio, flexionado)
- [ ] El dato se transmite vía Wi-Fi y se recibe en un script/servidor en menos de **500 ms**
- [ ] La lectura es **repetible** (misma posición produce lecturas dentro de ±5% del rango)

### Plan del Spike (Sprint 1)

1. **Día 1–2:** Setup del entorno de desarrollo (Arduino IDE / PlatformIO + ESP32)
2. **Día 2–3:** Circuito divisor de voltaje con 1 sensor flex → lectura ADC1 (serial monitor)
3. **Día 3–4:** Activar Wi-Fi STA y verificar que ADC1 sigue funcionando → enviar dato por HTTP
4. **Día 4–5:** Medir latencia, repetibilidad, rango de valores → documentar resultados

---

## 📊 Reporte del Spike

> ⚠️ **Este apartado se actualizará al final del Sprint 1 con los resultados reales de las pruebas.**

### Formato del Reporte

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
         REPORTE DE SPIKE — Release 1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Fecha de ejecución: ___/___/2026
Participantes: [Nombres del equipo]

1. OBJETIVO
   Validar la lectura de sensores flex en ESP32
   con Wi-Fi activo simultáneamente.

2. SETUP EXPERIMENTAL
   - Hardware utilizado: [detallar]
   - Software/IDE: [detallar]
   - Circuito: [foto o esquemático]

3. RESULTADOS
   3.1 Lectura ADC sin Wi-Fi:
       - Valor dedo extendido: ___
       - Valor dedo flexionado 90°: ___
       - Valor dedo flexionado 180°: ___

   3.2 Lectura ADC con Wi-Fi activo:
       - ¿Funciona en ADC1? [Sí/No]
       - ¿Ruido adicional? [Sí/No, cuánto]
       - Valores comparados: [tabla]

   3.3 Transmisión Wi-Fi:
       - Protocolo usado: [HTTP/WebSocket]
       - Latencia medida: ___ ms
       - Paquetes perdidos: ___/%

   3.4 Repetibilidad:
       - 10 lecturas misma posición: [datos]
       - Desviación estándar: ___

4. CONCLUSIÓN
   [¿El spike fue exitoso? ¿El proyecto es viable?]
   [¿Qué ajustes se necesitan?]

5. DECISIONES TOMADAS
   - [Decisión 1 basada en resultados]
   - [Decisión 2 basada en resultados]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## 👥 Equipo

| Nombre | Rol | GitHub |
|---|---|---|
| Daniel Felipe Forero| Tecnico | DanielForero14 |
| Juan David Orozco | Arquitecto | David OrozcoJ |
| Joao Alexandre Muño Obando | desarrollador | JoaoALT |
| Sergio Gabriel Nieto Mendez | Desarrollador | [@usuario] |

---

## 📚 Referencias

- [Datasheet Spectra Symbol Flex Sensor 2.2"](https://www.spectrasymbol.com/wp-content/uploads/2020/10/Flex-Sensor-Datasheet-v2020.pdf)
- [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [ESP32 ADC Limitations with Wi-Fi](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- [Lengua de Señas Colombiana — INSOR](https://www.insor.gov.co/)

---

<p align="center">
  <em>SignGlove — Comunicación sin barreras 🤟</em>
</p>
