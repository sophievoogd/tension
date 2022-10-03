/*
  Bluetooth LE MIDI example for the MKR1010
  Uses ArduinoBLE library
  created 22 Feb 2019
  by Tom Igoe
  Based on work by Don Coleman
*/
#include <ArduinoBLE.h>


// data meten voor rssi
bool isDebug = false;

const int  alength = 7;
int values[alength] = {0, 0, 0, 0, 0, 0, 0};

String uuid = String("180f");
int strength = 0;
int previousStrength = -1;
long distance = 0;

int cnt = 0;

byte midiData[] = {0x80, 0x80, 0x00, 0x00, 0x00};

// set up the MIDI service and MIDI message characteristic:
BLEService midiService("03B80E5A-EDE8-4B33-A751-6CE34EC4C700");
BLECharacteristic midiCharacteristic("7772E5DB-3868-4112-A1A9-F2669D106BF3",
                                     BLEWrite | BLEWriteWithoutResponse |
                                     BLENotify | BLERead, sizeof(midiData));


int bpm = 120;  // beats per minute
int velocity = 1;

// duration of a beat in ms
float beatDuration = 60.0 / bpm * 1000;
unsigned long myTime;
unsigned long noteEnd = 0L;


// the melody sequence:
int melody[] = {40, 40, 40, 40};
// which note of the melody to play:
int noteCounter = 0;
int mappedVal = 0;


void setup() {
  // initialize serial communication
  Serial.begin(9600);
  // otherwise won't show in serial port
  delay(6000);

  Serial.print("beat duration");
  Serial.println(beatDuration);

  Serial.println(noteEnd);

  Serial.println("setup start");
  // initialize built in LED:
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize BLE:
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (true);
  }
  // set local name and advertised service for BLE:
  BLE.setLocalName("MIDI_BLE");
  BLE.setAdvertisedService(midiService);

  // add the characteristic and service:
  midiService.addCharacteristic(midiCharacteristic);
  BLE.addService(midiService);

  // start advertising midi service
  BLE.advertise();
  // start scanning for peripheral
  BLE.scan(true);

  Serial.println("eerste deel");
}

void loop() {

  //eerste stuk van de loop: alleen RSSI meting.

  //BLE.available = Query for a discovered BLE device that was found during scanning.
  BLEDevice peripheral = BLE.available();



  if (peripheral.localName() == "peripheral 1") {

    strength  = peripheral.rssi();
    
    //https://iotandelectronics.wordpress.com/2016/10/07/how-to-calculate-distance-from-the-rssi-value-of-the-ble-beacon/

    int teller = (-66-(strength));
    Serial.print("teller:");
    Serial.println(teller);
    float noemer = 10*2.2;
    Serial.print("noemer:");
    Serial.println(noemer);
    float breuk = (float)teller / (float)noemer;
    Serial.print("breuk:");
    Serial.println(breuk);
    Serial.print("hard distance:");
    Serial.println(pow(10,breuk) );
    
    float d = pow(10,breuk);
    
    //Distance = pow(10, ((-69-(strength))/(10*2)));
    
    Serial.print("Distance:");
    Serial.println(d);

    
    Serial.print("rssi strength:");
    Serial.println(strength);
    values[cnt] = d;
    int totaal = values[0] + values[1] + values[2] + values[3] + values[4] + values[5] + values[6];
    int gemiddelde = totaal / 7;
    Serial.println("gemiddelde:");
    Serial.println(gemiddelde);

    velocity = gemiddelde ;// * -1;
    Serial.print("velocity:");
    Serial.println(velocity);

    //Serial.println("hier gaan we");
    mappedVal = map(gemiddelde, 0, 20, 1, 80);
    Serial.print("mappedVal:");
    Serial.println(mappedVal);
    //mappedVal = velocity;



    cnt++;
    if (cnt >= 7) {
      cnt = 0;
    }


    previousStrength = strength;

    BLE.stopScan();
    delay(100);
    BLE.scan(true);



  }

  if (peripheral.localName() == "peripheral 2") {
    
  }









  // tweede stuk van de loop connect naar computer en stuur midi









  // wait for a BLE central
  BLEDevice central = BLE.central();


  // if a central is connected to the peripheral:
  if (central) {

    // turn on LED to indicate connection:
    digitalWrite(LED_BUILTIN, HIGH);






    // all the notes in this are sixteenth notes,
    // which is 1/4 of a beat, so:
    int noteDuration = beatDuration/3;
    
    myTime = millis();
    Serial.println(myTime);
    // play a note from the melody:
    Serial.println(noteEnd, DEC);
    
    if (noteEnd == 0) {
      Serial.println("Playing a note");
      midiCommand(0x90, melody[noteCounter], mappedVal);
      //Serial.println(myTime);
      //Serial.println(noteDuration*10);
      noteEnd=myTime+(noteDuration*10);
      //Serial.println(noteEnd);
    } else {
      //Serial.println(noteEnd);
    }


    // keep it on for the appropriate duration:
    //delay(noteDuration);

    // turn the note off:
    if (myTime >= noteEnd) {
      Serial.println("doing note end");
      midiCommand(0x80, melody[noteCounter], mappedVal);
      noteEnd = 0L;
    }else{
      Serial.println("no note end");
      //Serial.print("myTime:");
      //Serial.println(myTime);
      //Serial.print("noteEnd:");
      //Serial.println(noteEnd);
    }
    // increment the note number for next time through the loop:
    noteCounter++;
    //velocity = velocity + 10;
    // keep the note in the range from 0 - 11 using modulo:
    noteCounter = noteCounter % 4;
    //velocity = velocity % 127;

    //delay(1000);



  }
  // when the central disconnects, turn off the LED:
  digitalWrite(LED_BUILTIN, LOW);
}

// send a 3-byte midi message
void midiCommand(byte cmd, byte data1, byte  data2) {
  // MIDI data goes in the last three bytes of the midiData array:
  midiData[2] = cmd;
  midiData[3] = data1;
  midiData[4] = data2;

  midiCharacteristic.setValue(midiData, sizeof(midiData));







}
