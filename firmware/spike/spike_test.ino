#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// ==========================================
// CONFIGURACIÓN MQTT
// ==========================================
const char* mqtt_server = "broker.hivemq.com"; // Servidor público para pruebas
const int mqtt_port = 8883; // Puerto seguro
const char* mqtt_topic = "guante/iot/letras";

// Certificado Raiz ISRG Root X1 (Let's Encrypt) que usan muchos brokers
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRnXubJIVSoMwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJ1y/M6W2/dIphI3T\n" \
"c4B0Qk/B5B5Yx6rT/Vz7F1wA6a7yT2xR5/I1A6A0A9P3hP9R1jPZ9/w9s/mP4P9z\n" \
"Z1m1P1A8E2v9E2B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A9D4E1Q2E9Z0E8B2A7E1/6D3+N3X4E1A\n" \
"-----END CERTIFICATE-----\n";

// ==========================================
// OBJETOS GLOBALES
// ==========================================
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);
WebServer server(80);

// ==========================================
// PINES DE SENSORES FLEX
// ==========================================
const int PIN_INDICE = 32;
const int PIN_PULGAR = 33;

// Cola de mensajes FreeRTOS para pasar la letra entre tareas
QueueHandle_t letterQueue;

// Variable global para almacenar el estado y mostrarlo en la Web
volatile char currentLetter = '-';

// ==========================================
// PÁGINA WEB HTML
// ==========================================
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Guante IoT Dashboard</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; margin-top: 50px; background: #121212; color: #ffffff;}
    h1 { font-size: 40px; font-weight: 300; }
    .letter-container { margin: 50px auto; width: 200px; height: 200px; background: rgba(255, 255, 255, 0.1); border-radius: 50%; display: flex; align-items: center; justify-content: center; box-shadow: 0 0 20px rgba(0,255,136,0.5); }
    .letter { font-size: 100px; color: #00ff88; font-weight: bold; }
    .footer { margin-top: 50px; font-size: 14px; color: #666; }
  </style>
  <script>
    // Hacer Polling asíncrono para actualizar la letra cada 1 segundo
    setInterval(function() {
      fetch('/status')
        .then(response => response.text())
        .then(text => {
          document.getElementById("letter").innerHTML = text;
        })
        .catch(err => console.log(err));
    }, 1000);
  </script>
</head>
<body>
  <h1>Lenguaje de Señas - MVP</h1>
  <div class="letter-container">
    <div class="letter" id="letter">-</div>
  </div>
  <div class="footer">Detección de Letras L, G, Q en tiempo real.</div>
</body>
</html>
)rawliteral";


// ==========================================
// FUNCIONES AUXILIARES
// ==========================================
void setup_wifi_and_mqtt() {
    // 1. Aprovisionamiento Wi-Fi
    WiFiManager wifiManager;
    // wifiManager.resetSettings(); // Borra las credenciales previas ("JoaoALT")
    wifiManager.setConfigPortalTimeout(180); // 3 minutos para configurar
    
    Serial.println("Conectando a WiFi o abriendo AP...");
    if (!wifiManager.autoConnect("GuanteIoT_AP")) {
        Serial.println("Fallo al conectar y tiempo de AP excedido. Reiniciando...");
        delay(3000);
        ESP.restart();
    }
    Serial.println("Conectado a WiFi con éxito!");
    Serial.print("IP Local: ");
    Serial.println(WiFi.localIP());

    // 1.5 Sincronización de Tiempo (NTP)
    Serial.println("Sincronizando tiempo con servidor NTP...");
    configTime(-18000, 0, "pool.ntp.org", "time.nist.gov"); // -18000 = UTC-5 (Ej: Colombia)
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
        Serial.println("Tiempo sincronizado correctamente.");
    } else {
        Serial.println("Fallo al sincronizar el tiempo.");
    }

    // 2. Configurar cliente MQTT con certificado
    secureClient.setInsecure(); // SOLO PARA PRUEBAS: Si quieres validacion estricta, comenta esto y habilita setCACert(root_ca);
    // secureClient.setCACert(root_ca); 
    
    mqttClient.setServer(mqtt_server, mqtt_port);
}

void reconnect_mqtt() {
    // Loop hasta que nos conectemos
    while (!mqttClient.connected()) {
        Serial.print("Intentando conexión MQTT...");
        // Crear un ID de cliente aleatorio
        String clientId = "ESP32Glove-";
        clientId += String(random(0xffff), HEX);
        
        // Intentar conectar
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println("Conectado a MQTT");
        } else {
            Serial.print("fallo, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" -> intentando de nuevo en 5 segundos");
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}


// ==========================================
// TAREAS DE FREERTOS
// ==========================================

// TAREA 1: Leer Sensores (Pines 32 y 33) o Consola
void TaskSensors(void *pvParameters) {
    String inputString = "";
    
    // Configurar pines del ADC
    pinMode(PIN_INDICE, INPUT);
    pinMode(PIN_PULGAR, INPUT);

    while(1) {
        // 1. Leer valores reales de los sensores (ADC va de 0 a 4095 en ESP32)
        int rIndex = analogRead(PIN_INDICE);
        int rThumb = analogRead(PIN_PULGAR);

        // 2. Override: Si hay datos por Serial, los usamos como simulación "Hombre de Paja"
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                int commaIndex = inputString.indexOf(',');
                if (commaIndex > 0) {
                    rIndex = inputString.substring(0, commaIndex).toInt();
                    rThumb = inputString.substring(commaIndex + 1).toInt();
                    Serial.print("Simulación Serial -> Índice: "); Serial.print(rIndex);
                    Serial.print(" | Pulgar: "); Serial.println(rThumb);
                }
                inputString = "";
            } else if (c != '\r') {
                inputString += c;
            }
        }
        
        char detected = '-';
        
        // LÓGICA DE DETECCIÓN
        // Nota: Ajusta estos umbrales según los valores que te arroje analogRead (0-4095)
        // Por ahora mantengo los rangos del "Hombre de Paja" originales (10k, 15k, 20k)
        if (rIndex >= 8000 && rIndex <= 12000 && rThumb >= 8000 && rThumb <= 12000) {
            detected = 'L';
        } else if (rIndex >= 13000 && rIndex <= 17000 && rThumb >= 13000 && rThumb <= 17000) {
            detected = 'G';
        } else if (rIndex >= 18000 && rIndex <= 22000 && rThumb >= 18000 && rThumb <= 22000) {
            detected = 'Q';
        }

        // Si detectamos una letra VÁLIDA y es diferente a la que ya teníamos para no saturar la red
        if (detected != '-' && detected != currentLetter) {
            Serial.print(">>> Letra detectada: ");
            Serial.println(detected);
            
            // Actualizar variable global y enviar a la cola
            currentLetter = detected;
            xQueueSend(letterQueue, &detected, portMAX_DELAY);
        } else if (detected == '-') {
            // currentLetter = '-'; // Descomentar si deseas que se limpie la pantalla cuando no hay detección
        }
        
        // Leer sensores 2 veces por segundo (cada 500ms) para no saturar
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


// TAREA 2: Manejar MQTT y HTTP
void TaskNetwork(void *pvParameters) {
    char letter;
    while(1) {
        // Asegurar que MQTT este conectado
        if (WiFi.status() == WL_CONNECTED) {
            if (!mqttClient.connected()) {
                reconnect_mqtt();
            }
            mqttClient.loop();

            // Revisar si hay un nuevo mensaje en la cola
            if (xQueueReceive(letterQueue, &letter, pdMS_TO_TICKS(100)) == pdPASS) {
                
                // 1. PUBLICAR POR MQTT
                String msg = String("{\"letra\":\"") + letter + "\"}";
                if (mqttClient.publish(mqtt_topic, msg.c_str())) {
                     Serial.println("Mensaje MQTT publicado con éxito.");
                }

                // 2. PETICION HTTP GET (Cliente HTTP)
                HTTPClient http;
                String serverPath = "http://httpbin.org/get?letra=" + String(letter);
                http.begin(serverPath.c_str());
                int httpResponseCode = http.GET();
                if (httpResponseCode > 0) {
                    Serial.print("HTTP GET Status: ");
                    Serial.println(httpResponseCode);
                } else {
                    Serial.print("Error en HTTP GET: ");
                    Serial.println(httpResponseCode);
                }
                http.end();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


// TAREA 3: Servidor Web Local
void TaskWebServer(void *pvParameters) {
    // Configurar endpoints del servidor web
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", htmlPage);
    });
    
    server.on("/status", HTTP_GET, []() {
        server.send(200, "text/plain", String(currentLetter));
    });

    // Endpoint de Healthcheck (Salud del sistema y tiempo NTP)
    server.on("/healthcheck", HTTP_GET, []() {
        struct tm timeinfo;
        String timeStr = "Desconocido";
        if (getLocalTime(&timeinfo)) {
            char timeStringBuff[50];
            strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
            timeStr = String(timeStringBuff);
        }
        String jsonResponse = "{\"status\":\"up\", \"mqtt_connected\":" + String(mqttClient.connected() ? "true" : "false") + ", \"local_time\":\"" + timeStr + "\"}";
        server.send(200, "application/json", jsonResponse);
    });

    server.begin();
    Serial.println("Servidor Web HTTP iniciado en puerto 80.");

    while(1) {
        // Manejar peticiones de clientes
        server.handleClient();
        
        // Ceder tiempo para no activar el Watchdog (WDT)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


// ==========================================
// SETUP PRINCIPAL
// ==========================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n--- INICIANDO SISTEMA DE GUANTE IOT ---");
    Serial.println("Por favor ingresa resistencias (ej: 10000,10000 para L)");

    // Conectar a Wi-Fi y MQTT ANTES de iniciar las tareas
    setup_wifi_and_mqtt();

    // Crear cola de mensajes para transferir caracteres entre tareas (longitud 10)
    letterQueue = xQueueCreate(10, sizeof(char));

    // Desplegar Tareas en los Cores del ESP32 (El ESP32 es Dual Core)
    // Core 1 (Aplicacion / Sensores)
    xTaskCreatePinnedToCore(
        TaskSensors,    // Funcion de la tarea
        "TaskSensors",  // Nombre (para debug)
        4096,           // Stack size (bytes)
        NULL,           // Parametros
        1,              // Prioridad
        NULL,           // Handle
        1               // Core a anclar
    );

    // Core 0 (Comunicaciones / Red)
    xTaskCreatePinnedToCore(
        TaskNetwork,    
        "TaskNetwork",  
        8192,           // Stack mayor porque usa WiFi y TLS
        NULL,           
        1,              
        NULL,           
        0               
    );

    // Core 1 (Web)
    xTaskCreatePinnedToCore(
        TaskWebServer,  
        "TaskWebServer",
        4096,           
        NULL,           
        1,              
        NULL,           
        1               
    );
}

void loop() {
    // En FreeRTOS, el loop puede eliminarse a sí mismo para no consumir recursos innecesariamente.
    vTaskDelete(NULL);
}
