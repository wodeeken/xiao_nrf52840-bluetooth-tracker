#include "Adafruit_TinyUSB.h"
#include <bluefruit.h>

// Setup Custom Buzzer Service and Characteristic.
BLEService        buzzerService = BLEService("94cb3945-9846-40ab-8209-0f4883f9b105");
BLECharacteristic buzzerCharacteristic = BLECharacteristic("f8785bbd-0742-42cb-8490-cfbf1a4c9a2b");

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas batteryService;    // Battery service

int buzzerPin = 10;
bool buzzerOn = false;
bool firstLoopAfterConnection = true;
int loopCount = 0;
void setup()
{
  Serial.begin(115200);
  Serial.println("Now setting up buzzer pin.");
  // SETUP BUZZER PIN.
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  // Perform initial beep to alert user the board is starting up..
  digitalWrite(buzzerPin, HIGH);
  delay(500);
  digitalWrite(buzzerPin, LOW);
  Serial.println("Buzzer pin setup complete.");
  Serial.println("Now setting up battery-related pins.");
  // Setup battery voltage read.
  analogReadResolution(ADC_RESOLUTION);
  pinMode(VBAT_ENABLE, OUTPUT);
  digitalWrite(VBAT_ENABLE, LOW);
  pinMode(PIN_VBAT, INPUT);
  Serial.println("Battery-related pn setup complete.");
  Serial.println("Now setting up BLE and its services.");

  
  Bluefruit.begin();
  Bluefruit.setName("Buzz");
  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure and Start the Device Information Service
  Serial.println("Now configuring the Device Information Service");
  bledis.setManufacturer("MyManufacturer");
  bledis.setModel("MyBoard");
  bledis.begin();
  Serial.println("Device Information Service setup is complete.");
  Serial.println("Now configuring the Battery Service");
  batteryService.begin();
  batteryService.write(100);
  Serial.println("Battery Service setup is complete.");
  
  Serial.println("Now configuring the custom Buzzer Service and Characteristic.");
  buzzerService.begin();
  buzzerCharacteristic.setProperties(CHR_PROPS_WRITE);
  buzzerCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  buzzerCharacteristic.setFixedLen(1);
  buzzerCharacteristic.begin();
  Serial.println("Buzzer Service and Characteristic setup is complete.");
  // Setup the advertising packet(s)
  Serial.println("Now setting up BLE Advertising.");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include Buzzer Service.
  Bluefruit.Advertising.addService(buzzerService);
  Bluefruit.Advertising.addName();
  
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); 
  Bluefruit.Advertising.setFastTimeout(30);      
  Bluefruit.Advertising.start(0);             

  Serial.println("Advertising setup is complete.");
  Serial.println("Setup and configuration is complete.");
}


void connect_callback(uint16_t conn_handle)
{
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
  Serial.print("Connected to ");
  Serial.println(central_name);
  firstLoopAfterConnection = true;
}


void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Resuming Advertising.");
  if(buzzerOn){
        Serial.println("Shutting off buzzer.");
        buzzerOn = false;
        digitalWrite(buzzerPin, LOW);
  }
}

void loop()
{
  // Read bat level only every ~30 seconds.
  if(loopCount % 30 == 0){
    Serial.println("Time to read battery.");
    int vbatt = analogRead(PIN_VBAT);
    float adcVoltage = 2.961 * 3.6 * vbatt / 4096;
    //very rough assumptions: assume full bat = 3.6V,  dead battery = 2 V.
    int batteryLevel = (2.25 * (adcVoltage - 2)/3.6) * 100;
    Serial.print("Battery level: ");Serial.println(batteryLevel);
    batteryService.write(batteryLevel);

  }
  if ( Bluefruit.connected() ) {
    char buffer[1];
    // Toggle buzzer when a non-zero value is written to the buzzer characteristic.
    if ( buzzerCharacteristic.read(&buffer, sizeof(buffer)) && buffer[0] != 0 && !firstLoopAfterConnection){
      Serial.println("Non-zero value read from the buzzer characteristic."); 
      Serial.println(buffer[0], HEX);
      buzzerCharacteristic.write8(0);
      if(buzzerOn){
        Serial.println("Shutting off buzzer.");
        buzzerOn = false;
        digitalWrite(buzzerPin, LOW);
      }else{
        Serial.println("Turning on buzzer.");
        buzzerOn = true;
        digitalWrite(buzzerPin, HIGH);
      }
    }else{
      if(firstLoopAfterConnection){
        // Write zero to buzzer characteristic to start off with clean slate.
        buzzerCharacteristic.write8(0);
        firstLoopAfterConnection = false;
      }
    }
  }
  delay(1000);
  loopCount++;
}