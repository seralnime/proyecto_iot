/*
 * ============================================================
 *  SignGlove — Spike Test: Flex Sensor + Wi-Fi
 * ============================================================
 *  Objetivo: Validar la lectura de 1 sensor flex en ADC1
 *  mientras Wi-Fi está activo, y transmitir el dato por HTTP.
 *
 *  Hardware:
 *    - ESP32-WROOM-32
 *    - Sensor Flex 2.2" (Spectra Symbol SEN-10264)
 *    - Resistencia 10kΩ (divisor de voltaje)
 *
 *  Circuito:
 *    3.3V → [10kΩ] → Punto medio → GPIO 34 (ADC1_CH6)
 *                     Punto medio → [Flex Sensor] → GND
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>

// ============================================================
//  CONFIGURACIÓN — MODIFICAR SEGÚN SU RED
// ============================================================
const char* WIFI_SSID     = "TU_RED_WIFI";
const char* WIFI_PASSWORD = "TU_CONTRASEÑA";

// Pin del sensor flex (DEBE ser ADC1: GPIO 32, 33, 34, 35, 36, 39)
const int FLEX_PIN = 34;  // ADC1_CH6

// Parámetros del ADC
const int ADC_RESOLUTION = 4095;  // 12 bits
const float V_REF = 3.3;          // Voltaje de referencia

// Parámetros del divisor de voltaje
const float R_FIJA = 10000.0;  // 10 kΩ

// Servidor web en puerto 80
WebServer server(80);

// Variables globales
int rawADC = 0;
float voltage = 0.0;
float flexResistance = 0.0;
unsigned long lastReadTime = 0;
const int READ_INTERVAL_MS = 100;  // Leer cada 100ms (10 Hz)

// ============================================================
//  FUNCIONES
// ============================================================

void readFlexSensor() {
  rawADC = analogRead(FLEX_PIN);
  voltage = (rawADC / (float)ADC_RESOLUTION) * V_REF;
  
  // Calcular resistencia del flex sensor a partir del divisor de voltaje
  // V_out = V_ref * R_flex / (R_fija + R_flex)
  // R_flex = R_fija * V_out / (V_ref - V_out)
  if (voltage < V_REF) {
    flexResistance = R_FIJA * voltage / (V_REF - voltage);
  } else {
    flexResistance = 999999.0;  // Saturado
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<title>SignGlove Spike Test</title>";
  html += "<style>";
  html += "body{font-family:monospace;background:#1a1a2e;color:#eee;padding:40px;text-align:center;}";
  html += "h1{color:#e94560;}";
  html += ".value{font-size:48px;color:#0f3460;background:#e94560;padding:20px;border-radius:10px;display:inline-block;margin:10px;}";
  html += "table{margin:20px auto;border-collapse:collapse;}";
  html += "td,th{padding:10px 20px;border:1px solid #444;text-align:left;}";
  html += "th{background:#0f3460;}";
  html += "</style></head><body>";
  html += "<h1>🧤 SignGlove — Spike Test</h1>";
  html += "<div class='value'>ADC: " + String(rawADC) + "</div>";
  html += "<div class='value'>V: " + String(voltage, 3) + "V</div>";
  html += "<div class='value'>R: " + String(flexResistance/1000.0, 1) + "kΩ</div>";
  html += "<table>";
  html += "<tr><th>Parámetro</th><th>Valor</th></tr>";
  html += "<tr><td>Lectura ADC (crudo)</td><td>" + String(rawADC) + " / " + String(ADC_RESOLUTION) + "</td></tr>";
  html += "<tr><td>Voltaje calculado</td><td>" + String(voltage, 3) + " V</td></tr>";
  html += "<tr><td>Resistencia flex</td><td>" + String(flexResistance/1000.0, 1) + " kΩ</td></tr>";
  html += "<tr><td>Pin GPIO</td><td>" + String(FLEX_PIN) + " (ADC1)</td></tr>";
  html += "<tr><td>Wi-Fi activo</td><td>✅ Sí</td></tr>";
  html += "<tr><td>RSSI</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  html += "<tr><td>IP</td><td>" + WiFi.localIP().toString() + "</td></tr>";
  html += "</table>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleAPI() {
  // Endpoint JSON para pruebas de latencia
  String json = "{";
  json += "\"adc_raw\":" + String(rawADC) + ",";
  json += "\"voltage\":" + String(voltage, 4) + ",";
  json += "\"resistance_ohm\":" + String(flexResistance, 0) + ",";
  json += "\"timestamp_ms\":" + String(millis()) + ",";
  json += "\"wifi_rssi\":" + String(WiFi.RSSI());
  json += "}";
  
  server.send(200, "application/json", json);
}

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("============================================");
  Serial.println("  SignGlove — Spike Test");
  Serial.println("  Flex Sensor + Wi-Fi Simultáneo");
  Serial.println("============================================");
  
  // Configurar ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);  // Rango completo 0-3.3V
  
  // Conectar a Wi-Fi
  Serial.print("Conectando a Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("   RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n❌ Error: No se pudo conectar a Wi-Fi");
    Serial.println("   Continuando solo con Serial Monitor...");
  }
  
  // Configurar servidor web
  server.on("/", handleRoot);
  server.on("/api", handleAPI);
  server.begin();
  Serial.println("📡 Servidor web iniciado en puerto 80");
  
  Serial.println("============================================");
  Serial.println("  Formato Serial: ADC_RAW, VOLTAGE, R_FLEX");
  Serial.println("============================================");
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  server.handleClient();
  
  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();
    readFlexSensor();
    
    // Imprimir por Serial (CSV para fácil análisis)
    Serial.print(rawADC);
    Serial.print(",");
    Serial.print(voltage, 3);
    Serial.print(",");
    Serial.println(flexResistance, 0);
  }
}
