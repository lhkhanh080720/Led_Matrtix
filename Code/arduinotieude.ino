#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <String.h>
#include <Wire.h>
#include <stdio.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define DISPLAYS_WIDE 3
#define DISPLAYS_HIGH 1

SoftwareSerial mySoftwareSerial(2,3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

int addr = 0; //addr of EEPROM

SoftDMD dmd(DISPLAYS_WIDE,DISPLAYS_HIGH);
DMD_TextBox box(dmd, 0, 0, DISPLAYS_WIDE*32, 16);

char pData[15];
int index_str[10];
String sData = ""; 
String sLine1 = "";
String sLine2 = "";
String charValue = "Road to Bali";
int i = 0;
int volume=0;
void setup() 
{
  Wire.begin(1);                /* join i2c bus with address 1 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Serial.begin(9600);
  dmd.setBrightness(255);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  int count = charValue.length();
  int keySpace = 0;
  for (int i=0;i<(DISPLAYS_WIDE*32);i++)
  {
    dmd.setPixel(i,15);
  }
  dmd.drawString(indexCenter(96, 12), 5, charValue);
}

void loop() 
{
}

void receiveEvent(int howMany) 
{
  i=0;
  while (0 < Wire.available()) 
  {
    pData[i] = Wire.read();      /* receive byte as a character */
    Serial.print(pData[i++]);
  }
  Serial.println("=============================");
  if (pData[0]=='B')
  {
    CharToString();
    dmd.setBrightness(sData.toInt());
    //send data to arduino ("Bxx")
    Serial.println(sData.toInt());
  }
  else if (pData[0]=='T')
  {
    //send data to arduino ("Txx")//textTarget
    if(i >= 33) return;
    else if(i > 16)
    {
      int m = 0;
      int kArr = 0;
      for(int l = 1; l < i; l++)
        if(pData[l] == ' ') index_str[m++] = l;
      for(int l = 0; l < m; l++)
        if(index_str[l] < 17) kArr = index_str[l];
      sLine1 = "";
      sLine2 = "";
      int l = 1;
      while(l <= (kArr - 1))
        sLine1 += pData[l++];
      while(1 < i)
        sLine2 += pData[l++];
      dmd.drawString(indexCenter(96, sLine1.length()), 0, sLine1);
      dmd.drawString(indexCenter(96, sLine2.length()), 8, sLine1);
    }
    else
    {
      CharToString();
      dmd.drawString(indexCenter(96, i), 5, sData);
    }
  }
  else if (pData[0]=='V')
  {
    CharToString();
    
    //Add code modify volume here with value = sData.toInt()
    volume=sData.toInt();
  }
  else if(pData[0]=='-')
  {
    switch(pData[1])
    {
      case 'M':
         Serial.println("==========>  Music 1 : ON");
         playMusic(1,volume);
         break;
      case 'Q':
         Serial.println("==========>  Music 2 : ON");
         playMusic(2,volume);
         break;
      case 'Y':
         Serial.println("==========>  Music 3 : ON");
         playMusic(3,volume);
         break;
    }
  }
}

int indexCenter(unsigned long distance, int longStr)
{
  if( (int)((distance - (longStr*5 - 1))/2 - 1)  < 0 ) return 0;
  else return (int)((distance - (longStr*5 - 1))/2 - 1);
}

void CharToString()
{
  sData = "";
  for(int j = 1; j < i; j++)
  {
      sData += pData[j];
  }
}
void playMusic(int number,int volume)
{
    if (!myDFPlayer.begin(mySoftwareSerial))  //Use softwareSerial to communicate with mp3.
      while(true);
    myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
    myDFPlayer.volume(volume);  //Set volume value (0~30).
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); 
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    switch(number)
    {
      case 1:
        myDFPlayer.play(1);
        delay(77000);
        break;
      case 2:
        myDFPlayer.play(2);
        delay(196000);
        break;
      case 3:
        myDFPlayer.play(3);
        delay(315000);
        break;
    }
    myDFPlayer.pause();  //pause the mp3  
  }
