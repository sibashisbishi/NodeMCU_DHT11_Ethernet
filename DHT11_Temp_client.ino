#include <SPI.h>
#include <Ethernet.h>
String message="";
byte mac[] = {  0x12, 0x8A, 0xAB, 0xAB, 0x2E, 0xA2 }; //giving the macID to the device
byte server[] = {13,233,237,38};  //specifying 
#define port 7808     //random port number to publish
EthernetClient client; //making an object named client, of the class EthernetClient
int stastus;
void setup() {
  Serial.begin(9600);
  if(Ethernet.begin(mac)==0)
  {
     Serial.print("error");
  }
  else{
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  }  
 }

int read(int pin, byte* ptemperature, byte* phumidity, byte pdata[40]);
int confirm(int pin, int us, byte level);
byte bits2byte(byte data[8]);
int sample(int pin, byte data[40]);
int parse(byte data[40], byte* ptemperature, byte* phumidity);
int pinDHT11 = 2;

//void setup(){
//  Serial.begin(9600);
//}

int confirm(int pin, int us, byte level) {
    // wait one more count to ensure.
    int cnt = us / 10 + 1;
    bool ok = false;
    for (int i = 0; i < cnt; i++) {
        if (digitalRead(pin) != level) {
            ok = true;
            break;
        }
        delayMicroseconds(10);
    }
    if (!ok) {
        return -1;
    }
    return 0;
}

byte bits2byte(byte data[8]) {
    byte v = 0;
    for (int i = 0; i < 8; i++) {
        v += data[i] << (7 - i);
    }
    return v;
}

int read(int pin, byte* ptemperature, byte* phumidity, byte pdata[40]) {
    int ret = 0;

    byte data[40] = {0};

   //collecting 40 bits of data from DHT11

      // empty output data.
    memset(data, 0, 40);

    // notify DHT11 to start: 
    //    1. PULL LOW 20ms.
    //    2. PULL HIGH 20-40us.
    //    3. SET TO INPUT.
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(20);
    digitalWrite(pin, HIGH);
    delayMicroseconds(30);
    pinMode(pin, INPUT);

    // DHT11 starting:
    //    1. PULL LOW 80us
    //    2. PULL HIGH 80us
    if (confirm(pin, 80, LOW)) {
        return 100;
    }
    if (confirm(pin, 80, HIGH)) {
        return 101;
    }

    // DHT11 data transmite:
    //    1. 1bit start, PULL LOW 50us
    //    2. PULL HIGH 26-28us, bit(0)
    //    3. PULL HIGH 70us, bit(1)
    for (int j = 0; j < 40; j++) {
        if (confirm(pin, 50, LOW)) {
            return 102;
        }

        // read a bit, should never call method,
        // for the method call use more than 20us,
        // so it maybe failed to detect the bit0.
        bool ok = false;
        int tick = 0;
        for (int i = 0; i < 8; i++, tick++) {
            if (digitalRead(pin) != HIGH) {
                ok = true;
                break;
            }
            delayMicroseconds(10);
        }
        if (!ok) {
            return 103;
        }
        data[j] = (tick > 3? 1:0);
    }

    // DHT11 EOF:
    //    1. PULL LOW 50us.
    if (confirm(pin, 50, LOW)) {
        return 104;
    }
   //end of sample collection
    
   //collection of temperature and humidity from 40 bit data array and checksum calculation
    
    byte humidity = bits2byte(data);
    byte humidity2 = bits2byte(data + 8);
    byte temperature = bits2byte(data + 16);
    byte temperature2 = bits2byte(data + 24);
    byte check = bits2byte(data + 32);
    byte expect = humidity + humidity2 + temperature + temperature2;
    if (check != expect) {
        return 105;
    }
    *ptemperature = temperature;
    *phumidity = humidity;
 
   //end of parse

    if (pdata) {
        memcpy(pdata, data, 40);
    }
    if (ptemperature) {
        *ptemperature = temperature;
    }
    if (phumidity) {
        *phumidity = humidity;
    }

    return ret;
}

void loop(){
   Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  if (read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %");
  
  // DHT11 sampling rate is 1HZ.
  delay(1000);

  if( client.connect(server,port))
 {
  //if connection is established then do the following opoeration
    message = ""+String(temperature)+"\n";
    Serial.println("message sent to server:"+message);
    //message = "IOT Client";
    //Serial.println("message sent to srver:"+message);
    client.print(message);
    message="";
    char ch=0;
    while(1)
    {
      //Serial.println(".");
      if(client.available())
      {
        ch=client.read();
        if(ch=='\n')
           break;
        message+=ch;   
        //Serial.print(ch);
      }
    }
    Serial.print("Reply from Server: "+message);
    client.flush();//clearing the buffer
    client.stop();//closing the connection
    Serial.println("");
    delay(1000);//giving some delay before throwing the next value
 }
 else
 {
  //If connection is not established then this part will be executed
  Serial.println("the socket connection is failed to connect to the specified port");
 }

}

