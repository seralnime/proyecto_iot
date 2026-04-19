# 📅 Cronograma Tentativo — SignGlove

## Visión General por Releases

| Release | Semanas | Sprint | Objetivo Principal | Entregable |
|---------|---------|--------|--------------------|------------|
| **Release 1** | 1–2 | S1, S2 | Fundamentos y viabilidad | Spike resuelto + README + Backlog priorizado |
| **Release 2** | 3–4 | S3, S4 | Core MVP funcional | Sistema que traduce ≥10 letras end-to-end |
| **Release 3** | 5–6 | S5, S6 | Integración y mejoras | Guante completo con batería + abecedario completo |
| **Release 4** | 7–8 | S7, S8 | Feature Freeze + Pulido | Producto estable + pitch + documentación final |

---

## Detalle por Sprint

### Sprint 1 — Setup y Spike (Semana 1)
| Día | Actividad |
|-----|-----------|
| L–M | Configuración del repositorio, entorno de desarrollo (Arduino IDE / PlatformIO) |
| M–J | Circuito divisor de voltaje con 1 sensor flex → lectura ADC1 |
| J–V | Activar Wi-Fi + verificar ADC1 funcional → enviar dato por HTTP |
| V–S | Documentar spike, medir latencia y repetibilidad |

### Sprint 2 — Validación Multi-sensor (Semana 2)
| Día | Actividad |
|-----|-----------|
| L–M | Conectar 5 sensores a pines ADC1, validar lecturas simultáneas |
| M–J | Implementar rutina de calibración (min/max por sensor) |
| J–V | Definir tabla de mapeo para 5 letras de prueba |
| V–S | Preparar retrospectiva Release 1 |

> 📋 **Retrospectiva Release 1** — Fin de Semana 2

### Sprint 3 — Clasificación y Backend (Semana 3)
| Día | Actividad |
|-----|-----------|
| L–M | Extender tabla de mapeo a ≥15 letras estáticas |
| M–J | Definir protocolo JSON, implementar envío por WebSocket |
| J–V | Crear servidor backend (Python Flask / Node.js Express) |
| V–S | Pruebas de integración ESP32 → servidor |

### Sprint 4 — Interfaz Web y End-to-End (Semana 4)
| Día | Actividad |
|-----|-----------|
| L–M | Diseñar e implementar interfaz web (HTML/CSS/JS + WebSocket client) |
| M–J | Integración end-to-end: guante → ESP32 → Wi-Fi → servidor → web |
| J–V | Medir latencia total, identificar y corregir cuellos de botella |
| V–S | Preparar retrospectiva Release 2 |

> 📋 **Retrospectiva Release 2** — Fin de Semana 4

### Sprint 5 — Abecedario Completo y Hardware (Semana 5)
| Día | Actividad |
|-----|-----------|
| L–M | Completar clasificación para todo el abecedario estático |
| M–J | Integrar batería LiPo + TP4056, probar autonomía |
| J–V | Montar sensores y electrónica en el guante físico |
| V–S | Pruebas de usabilidad del guante armado |

### Sprint 6 — UX y Nice-to-Have (Semana 6)
| Día | Actividad |
|-----|-----------|
| L–M | Mejorar UI web (diseño, animaciones, historial) |
| M–J | Pruebas con usuarios, recopilar feedback |
| J–V | Implementar 1–2 nice-to-have si hay tiempo (ej. formación de palabras) |
| V–S | Preparar retrospectiva Release 3 |

> 📋 **Retrospectiva Release 3** — Fin de Semana 6

### Sprint 7 — Feature Freeze y Bug Fixes (Semana 7)
| Día | Actividad |
|-----|-----------|
| L–V | 🧊 **FEATURE FREEZE** — Solo corrección de errores y optimización |
| L–M | Estabilizar firmware, corregir bugs de clasificación |
| M–J | Optimizar latencia Wi-Fi, mejorar confiabilidad |
| J–V | Escribir documentación técnica completa |

### Sprint 8 — Demo y Entrega (Semana 8)
| Día | Actividad |
|-----|-----------|
| L–M | Preparar presentación / pitch |
| M–J | Grabar video demostración |
| J–V | Actualizar README.md final con resultados y métricas |
| V | 🎓 **ENTREGA FINAL** |

> 📋 **Entrega Final / Retrospectiva Release 4** — Fin de Semana 8

---

## Hitos Clave

| Fecha (Semana) | Hito | Criterio de Éxito |
|----------------|------|--------------------|
| Sem. 1 | Spike completado | 1 sensor + Wi-Fi funcionando |
| Sem. 2 | Release 1 | 5 sensores calibrados, viabilidad confirmada |
| Sem. 4 | Release 2 | ≥10 letras traducidas end-to-end |
| Sem. 6 | Release 3 | Guante completo funcional |
| Sem. 8 | Release 4 | Producto final + demo |

---

## Notas

- Sprints de **1 semana** con releases cada **2 semanas**
- Las asignaciones específicas por miembro se definirán al inicio de cada sprint
- El cronograma es **tentativo** y se ajustará en cada retrospectiva
- Los nice-to-have solo se abordarán si el MVP está completo y estable
