# 📋 Backlog del Producto — SignGlove

> Este documento complementa el tablero de GitHub Projects. Contiene la definición detallada de cada feature.

---

## Features Must-Have (MVP)

### F-01: Lectura de 5 Sensores Flex
- **Descripción:** Leer los valores analógicos de los 5 sensores flex (uno por dedo) usando los canales ADC1 del ESP32.
- **Criterios de aceptación:**
  - [ ] Se leen 5 valores analógicos simultáneamente
  - [ ] Se utilizan exclusivamente pines ADC1 (GPIO 32, 33, 34, 35, 36)
  - [ ] La tasa de muestreo es ≥ 50 Hz (20 ms por ciclo de lectura)
  - [ ] Los valores se imprimen por Serial Monitor para verificación
- **Etiquetas:** `must-have`, `hardware`, `sprint-2`

---

### F-02: Calibración Inicial
- **Descripción:** Al encender el dispositivo, ejecutar una rutina donde el usuario flexiona y extiende cada dedo para registrar los valores mínimo y máximo de cada sensor.
- **Criterios de aceptación:**
  - [ ] Al encender, el ESP32 entra en modo calibración
  - [ ] Se indica al usuario (vía Serial o LED) que extienda y flexione los dedos
  - [ ] Se almacenan valores min/max por cada sensor
  - [ ] Los valores se usan para normalizar las lecturas a un rango 0–100%
- **Etiquetas:** `must-have`, `firmware`, `sprint-2`

---

### F-03: Clasificación por Umbrales
- **Descripción:** Implementar un algoritmo que, dado un vector de 5 lecturas normalizadas, determine qué letra del abecedario (A–Z, letras estáticas) corresponde a la posición de la mano.
- **Criterios de aceptación:**
  - [ ] Se define una tabla de mapeo: [flex1, flex2, flex3, flex4, flex5] → letra
  - [ ] Se clasifican correctamente al menos 15 letras estáticas del abecedario
  - [ ] La clasificación se ejecuta en < 10 ms por ciclo
  - [ ] Se reporta un nivel de confianza o "match score"
- **Etiquetas:** `must-have`, `firmware`, `sprint-3`

---

### F-04: Transmisión Wi-Fi
- **Descripción:** Enviar la letra clasificada desde el ESP32 a un servidor local vía Wi-Fi.
- **Criterios de aceptación:**
  - [ ] El ESP32 se conecta a una red Wi-Fi configurada
  - [ ] Envía la letra como payload JSON: `{"letter": "A", "confidence": 0.95, "timestamp": ...}`
  - [ ] Se usa WebSocket para comunicación bidireccional de baja latencia
  - [ ] Latencia end-to-end < 500 ms
- **Etiquetas:** `must-have`, `firmware`, `networking`, `sprint-3`

---

### F-05: Interfaz Web Básica
- **Descripción:** Página web que se conecta al servidor y muestra la letra traducida en tiempo real.
- **Criterios de aceptación:**
  - [ ] Muestra la letra actual en grande y visible
  - [ ] Actualiza en tiempo real (WebSocket)
  - [ ] Muestra historial de las últimas 10 letras
  - [ ] Funciona en navegadores modernos (Chrome, Firefox, Edge)
- **Etiquetas:** `must-have`, `frontend`, `sprint-4`

---

### F-06: Alimentación Portátil
- **Descripción:** Alimentar el ESP32 y sensores con una batería LiPo recargable.
- **Criterios de aceptación:**
  - [ ] Batería LiPo 3.7V + módulo TP4056 para carga USB
  - [ ] El sistema funciona al menos 3 horas continuas
  - [ ] Indicador de batería baja (LED o en interfaz web)
- **Etiquetas:** `must-have`, `hardware`, `sprint-5`

---

### F-07: Montaje en Guante
- **Descripción:** Integrar físicamente los sensores y la electrónica en un guante.
- **Criterios de aceptación:**
  - [ ] Los 5 sensores flex están correctamente adheridos a los dedos
  - [ ] El cableado es seguro y no restringe el movimiento
  - [ ] El ESP32 y batería están alojados en el dorso de la mano o la muñeca
  - [ ] El guante es cómodo para uso prolongado (>30 min)
- **Etiquetas:** `must-have`, `hardware`, `sprint-5`

---

## Features Nice-to-Have

### F-08: Clasificación con ML (TFLite Micro)
- **Descripción:** Entrenar un modelo de Machine Learning (ej. Random Forest o Red Neuronal pequeña) y desplegarlo en el ESP32 usando TensorFlow Lite Micro.
- **Criterios de aceptación:**
  - [ ] Dataset de al menos 100 muestras por letra
  - [ ] Modelo con accuracy ≥ 85% en validación
  - [ ] Modelo convertido a TFLite y ejecutado en el ESP32
  - [ ] Tamaño del modelo < 100 KB
- **Etiquetas:** `nice-to-have`, `ml`, `firmware`

---

### F-09: Formación de Palabras
- **Descripción:** Acumular letras detectadas para formar palabras, con detección de pausa entre señas.
- **Etiquetas:** `nice-to-have`, `firmware`, `frontend`

---

### F-10: Text-to-Speech
- **Descripción:** Reproducir en audio la letra o palabra traducida.
- **Etiquetas:** `nice-to-have`, `frontend`

---

### F-11: App Móvil
- **Descripción:** Aplicación nativa Android/iOS que reemplace la interfaz web.
- **Etiquetas:** `nice-to-have`, `mobile`

---

### F-12: Dashboard de Métricas
- **Descripción:** Panel web con estadísticas de uso, precisión y historial.
- **Etiquetas:** `nice-to-have`, `frontend`

---

### F-13: Modo Offline (BLE)
- **Descripción:** Transmisión por Bluetooth Low Energy como alternativa a Wi-Fi.
- **Etiquetas:** `nice-to-have`, `firmware`, `networking`

---

### F-14: Sensor IMU (MPU6050)
- **Descripción:** Añadir acelerómetro/giroscopio para detectar orientación de la mano y cubrir letras dinámicas.
- **Etiquetas:** `nice-to-have`, `hardware`, `firmware`
