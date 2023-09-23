
#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("NODE");
}
char out = 'C';
void loop() { 
    Serial2.write(out);
    delay(1000);
}
