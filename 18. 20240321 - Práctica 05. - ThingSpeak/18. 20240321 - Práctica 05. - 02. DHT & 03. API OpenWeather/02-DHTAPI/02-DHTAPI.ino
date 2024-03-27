#include <DHT.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>
#include <WiFi.h>

// Temperature Data from sensors
struct TemperatureData {
    float dht;  // in Celsius
    String getAllTemperatureData() {
        return "\nDHT: " + (String)dht + "*C";
    }
};

// DHT sensor setup
const uint8_t dhtPin = 15; // Pin for the DHT sensor
const uint8_t dhtType = DHT22;
DHT dht(dhtPin, dhtType);

// Data storage
TemperatureData temperatureData;

// Timer for periodic updates
uint32_t startMillis;
const uint16_t timerDuration = 10000;

// WiFi credentials
const char *ssid = "MyWiFi"; // Your WiFi SSID
const char *password = "password"; // Your WiFi password

WiFiClient client;

// OpenWeather API temperature storage
String apiTemperature;
String serverPath = "https://api.openweathermap.org/data/2.5/weather?lat=20.6024&lon=-103.2714&appid=#&units=metric"; // Path for OpenWeather API request

// ThingSpeak configuration
uint32_t channelNumber = 2483363; // Your ThingSpeak channel ID
const char *writeAPIKey = "#"; // Your ThingSpeak write API key

void setup() {
    Serial.begin(115200);
    dht.begin();
    ThingSpeak.begin(client); // Initialize ThingSpeak
    connectWiFi();

    // Start the update timer
    startMillis = millis();
}

void loop() {
    if (millis() >= timerDuration + startMillis) {
        // Get sensor readings
        temperatureData.dht = getDHTTemperatureC();
        // Get temperature from API
        apiTemperature = getTemperatureFromAPI();

        // Debug output to serial monitor
        Serial.println(temperatureData.getAllTemperatureData());
        Serial.println("Temperatura de la API: " + apiTemperature + "*C");

        delay(50);
        sendDataToThingSpeak();

        // Restart the timer
        startMillis = millis();
    }
}

// Helper functions
void connectWiFi() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println();
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void sendDataToThingSpeak() {
    // Ensure WiFi is connected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No internet connection detected");
        connectWiFi();
    }

    // Set the ThingSpeak fields
    ThingSpeak.setField(1, temperatureData.dht);
    ThingSpeak.setField(2, apiTemperature);

    // Determine the status message based on some logic
    float indoorTempAverage = temperatureData.dht;
    float outdoorTemp = apiTemperature.toFloat();
    String status;
    if (indoorTempAverage > 30)
        status = "High Temperature";
    else if (indoorTempAverage <= 30 && indoorTempAverage >= 20)
        status = "Normal Temperature";
    else if (indoorTempAverage < 20)
        status = "Low Temperature";

    // Set the status
    ThingSpeak.setStatus(status);

    // Write to the ThingSpeak channel
    int code = ThingSpeak.writeFields(channelNumber, writeAPIKey);
    if (code == 200)
        Serial.println("ThingSpeak channel updated successfully.");
    else
        Serial.println("Problem updating channel. HTTP error code: " + String(code));
}

float getDHTTemperatureC() {
    float t = dht.readTemperature();
    if (isnan(t)) {
        t = 0;
        Serial.println("Failed to read from DHT sensor");
    }
    return t;
}

String getTemperatureFromAPI() {
    // Ensure WiFi is connected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No internet connection detected");
        connectWiFi();
    }

    HTTPClient http;
    http.begin(serverPath.c_str());
    String temperature;
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        String payload = http.getString();
        // Parse the temperature from the payload
        int index = payload.indexOf("\"temp\":");
        temperature = payload.substring(index + 7, payload.indexOf(",\"feels_"));
    } else {
        Serial.print("GET request failed, error code: ");
        Serial.println(httpResponseCode);
        temperature = "0.0"; // Default if request fails
    }

    http.end();
    return temperature;
}
