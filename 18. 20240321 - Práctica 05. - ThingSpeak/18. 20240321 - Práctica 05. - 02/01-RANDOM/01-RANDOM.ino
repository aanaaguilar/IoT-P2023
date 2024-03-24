#include <DHT.h>
DHT dht15(15, DHT11);

void setup() {
  Serial.begin(9600);
  delay(2000); // This was mentioned to be added, but it's unclear what "agregar al código de easycoding.tn" means
  dht15.begin(); // This also had some extra text that seemed out of place
}

void loop() {
  Serial.println("Temperatura en el sensor");
  Serial.println(dht15.readTemperature());

  Serial.println("Humedad en el sensor");
  Serial.println(dht15.readHumidity());

  delay(5000);
}
