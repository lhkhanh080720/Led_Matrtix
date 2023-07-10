#include <Arduino.h>
#include <WiFiClient.h> 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

const char *ssid = "Anh Lam";
const char *password = "lan466455";
const char *mqtt_server = "192.168.1.9";
const int mqtt_port = 1883;
const char *mqtt_id = "esp8266";
const char *topic_publish="from-esp8266";
const char *topic_subscribe="to-esp8266";
char dataReceive[25]; 
int i = 0;

WiFiClient client;
PubSubClient mqtt_client(client);
  
void setup()
{
  Serial.begin(9600);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  Serial.print("Connecting to Wifi... ");
  Serial.print(ssid);
  Serial.println();
  WiFi.begin(ssid, password);
  WiFi.reconnect();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to Wifi ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  delay(1000);

  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);

  Serial.println("Connecting to mqtt... ");
  while (!mqtt_client.connect(mqtt_id))
  {
    delay(500);
  }
  Serial.println("Connected to mqtt ");
  mqtt_client.subscribe("to-esp8266");
}

void loop()
{
  bool index = true;
  while (Wire.available()) 
  { 
    dataReceive[i] = Wire.read();
    Serial.print(dataReceive[i++]);
    index = false;
  }
  //Serial.println("Done");
  if(index == false)
  {
    Serial.print("dataReceive");
    mqtt_client.publish("from-esp8266", dataReceive);
    Serial.println(dataReceive);
    i = 0;
    index = true;
    clearArr(dataReceive,sizeof(dataReceive)/sizeof (char)); 
  } 
  mqtt_client.loop(); 
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Recived data from: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (size_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char )payload[0]=='B')
  {
    Serial.print("--3");
    //send data to arduino ("Bxx")
    Wire.beginTransmission(8); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */
    Wire.beginTransmission(1); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */  
  }
  else if ((char )payload[0]=='T' || (char )payload[0]=='V')
  {
    //send data to arduino ("Txx")//textTarget
    Wire.beginTransmission(1); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */ 
  }
  else if ((char )payload[0]=='R')
  {
    Serial.print("--0");
    //send data to arduino ("Rxx")//realTime
    Wire.beginTransmission(8); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */ 
  }
  else if ((char )payload[0]=='O')
  {
    Serial.print("--4");
    //send data to arduino ("O/../../..")//updateData
    Wire.beginTransmission(8); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */ 
  }
  else if((char )payload[0]=='-')
  {
    switch((char )payload[1])
    {
      case 'M':
        Wire.beginTransmission(1); /* begin with device address 8 */
        for (size_t i = 0; i < length; i++)
        {
          Wire.write((char)payload[i]);
        }
        Wire.endTransmission();    /* stop transmitting */ 
        break;
      case 'Q':
        Wire.beginTransmission(1); /* begin with device address 8 */
        for (size_t i = 0; i < length; i++)
        {
          Wire.write((char)payload[i]);
        }
        Wire.endTransmission();    /* stop transmitting */ 
        break;
      case 'Y':
        Wire.beginTransmission(1); /* begin with device address 8 */
        for (size_t i = 0; i < length; i++)
        {
          Wire.write((char)payload[i]);
        }
        Wire.endTransmission();    /* stop transmitting */ 
        break;
    }
  }
  else if((char )payload[0]=='K')
  {
    Serial.print("--2");
    //send data to arduino ("Date + vnd")
    Wire.beginTransmission(8); /* begin with device address 8 */
    for (size_t i = 0; i < length; i++)
    {
      if((char)payload[i] == '.' || (char)payload[i] == ',' ) continue;
      Wire.write((char)payload[i]);
    }
    Wire.endTransmission();    /* stop transmitting */ 
    Wire.requestFrom(8, 22);    
  }
  Serial.println();
  Serial.println("------------------------------------------");
}  
void clearArr(char arr[], int lengthArr)
{
  for(int i = 0; i < lengthArr; i++)
    arr[i] = '\0';
}
