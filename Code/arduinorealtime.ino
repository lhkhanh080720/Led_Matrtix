#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <Wire.h>
#include <stdio.h>
#include <String.h>

#define DISPLAYS_WIDE 6
#define DISPLAYS_HIGH 1

SoftDMD dmd(DISPLAYS_WIDE,DISPLAYS_HIGH);
DMD_TextBox box(dmd, 0, 0, DISPLAYS_WIDE*32, 16);

char pData[23];
char bufferNum[6];
String sData = ""; 
String mData = "";
String yData = "";
String nData = "";
char cData[] = "";
char dData[8];
int i=0;
String kData= "";
unsigned long T0; String sT0 = "";
unsigned long Q0; String sQ0 = "";
unsigned long Y0; String sY0 = "";
unsigned long dem;
bool flag = false;

void setup() 
{
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Serial.begin(9600);
  Wire.onRequest(requestEvent);

  dmd.setBrightness(255);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  dmd.drawString(98,4,"M: 6");
  dmd.drawString(130,4,"Q:2");
  dmd.drawString(157,4,"Y:2022");
  dmd.drawString(2,4,"0000");
  dmd.drawString(28,4,"00000");
  dmd.drawString(60,4,"000000");
  dmd.setPixel(7,10);
  dmd.setPixel(39,10);
  dmd.setPixel(77,10);

  //Cột
  for (int i=0;i<16;i++)
  {
    dmd.setPixel(26,i);
    dmd.setPixel(58,i);
    dmd.setPixel(122,i);
    dmd.setPixel(154,i);
  }
  //Hàng
  for (int i=0;i<192;i++)
  {
    dmd.setPixel(i,15);
  }  
}

void loop() 
{
}

void receiveEvent(int howMany) 
{
  clearArr(pData,15);
  Serial.println("Da nhan duoc data");
  i=0;
  while (0 < Wire.available()) 
    pData[i++] = Wire.read();      /* receive byte as a character */
  Serial.println("======>From ESP<======");
  Serial.println(pData);
  if (pData[0]=='B')
  {
    CharToString1();
    dmd.setBrightness(sData.toInt());
    Serial.println(sData.toInt());
  }
  else if (pData[0]=='R')
    processRealTime();
  else if (pData[0]=='O')
  {
    Serial.println("========Update Data=======");
    sT0 = "";T0 = 0;
    sQ0 = "";Q0 = 0;
    sY0 = "";Y0 = 0;
    //send data to arduino ("O/../../..")//updateData
    int count = 1;
    while(pData[count] != '/')
      sT0 += pData[count++];
    count++;
    while(pData[count] != '/')
      sQ0 += pData[count++];
    count++;
    while(count < i)
      sY0 += pData[count++];
    T0 = sT0.toInt();Serial.print("sT0 = ");Serial.println(T0);
    Q0 = sQ0.toInt();Serial.print("sQ0 = ");Serial.println(Q0);
    Y0 = sY0.toInt();Serial.print("sY0 = ");Serial.println(Y0);
    Serial.println("==========================");
  }
  else if(pData[0] == 'K')
  {
    Serial.println("========Calc Data=======");
    nData="";
    CharToString1();
    nData = sData;
    nData += "/";
    T0 += sData.toInt();
    Q0 += sData.toInt();
    Y0 += sData.toInt();
    itoa (T0,bufferNum,10);
    kData ="";
    for(int l = 0; l < checkCount(T0); l++)
    {
      kData += bufferNum[l];
      nData += bufferNum[l];
    }
    dmd.drawString(2,4,kData);                    //M
    nData += "/";
    itoa (Q0,bufferNum,10);
    kData ="";
    for(int l = 0; l < checkCount(Q0); l++)
    {
      kData += bufferNum[l];
      nData += bufferNum[l];
    }
    dmd.drawString(28,4,kData);                    //Q
    nData += "/";
    itoa (Y0,bufferNum,10);
    kData ="";
    for(int l = 0; l < checkCount(Y0); l++)
    {
      kData += bufferNum[l];
      nData += bufferNum[l];
    }
    dmd.drawString(60,4,kData);                    //Y
    nData += "-";                                  //Done
    clearArr(pData, 23);
    nData.toCharArray(pData, nData.length() + 1);
    flag = true;
    Serial.println("==========================");
  }
  dmd.setPixel(7,10);
  dmd.setPixel(39,10);
  dmd.setPixel(77,10);
}

void CharToString()
{
  sData = "";
  for(int j = 0; j < i; j++)
  {
      sData += pData[j];
  }
}

void CharToString1()
{
  sData = '\0';
  for(int j = 1; j < i; j++)
  {
      sData += pData[j];
  }
}

void processRealTime()
{
  Serial.println("=============");
  Serial.println(pData);
  T0 = 0;
  mData = "";
  yData = "";
  if(pData[1] == '0')
  {
    dmd.drawString(98,4,"M:"); 
    mData = pData[2] ;
    dmd.drawString(108,4,mData);
  }
  else
  {  
    dmd.drawString(98,4,"M:");
    mData = pData[1] + pData[2] + '\0';
    dmd.drawString(108,4,mData);
  }  
  for(int k = 4; k < 8; k++)
    yData += pData[k];
  dmd.drawString(157,4,"Y:");
  dmd.drawString(167,4,yData);
  Serial.print("M=");
  Serial.println(mData);
  Serial.print("Y=");
  Serial.println(yData);
  switch(mData.toInt())
  {
    case 2:
    case 3:
      dmd.drawString(130,4,"Q:1");
      break;
    case 5:
    case 6:
      dmd.drawString(130,4,"Q:2");
      break;
    case 8:
    case 9:
      dmd.drawString(130,4,"Q:3");
      break; 
    case 11:
    case 12:
      dmd.drawString(130,4,"Q:4");
      break;
    case 1:
      Q0 = 0;Y0 = 0;
      dmd.drawString(130,4,"Q:1");
      dmd.drawString(28,4,"00000");
      dmd.drawString(60,4,"000000");
      break;
    case 4:
      Q0 = 0;
      dmd.drawString(130,4,"Q:2");
      dmd.drawString(28,4,"00000");
      break;
    case 7:
      Q0 = 0;
      dmd.drawString(130,4,"Q:3");
      dmd.drawString(28,4,"00000");
      break;
    case 10:
      Q0 = 0;
      dmd.drawString(130,4,"Q:4");
      dmd.drawString(28,4,"00000");
      break;
  }
  dmd.drawString(2,4,"0000");
  dmd.setPixel(7,10);
  dmd.setPixel(39,10);
  dmd.setPixel(77,10);
    //Cột
  for (int i=0;i<16;i++)
  {
    dmd.setPixel(26,i);
    dmd.setPixel(58,i);
    dmd.setPixel(122,i);
    dmd.setPixel(154,i);
  }
  //Hàng
  for (int i=0;i<192;i++)
  {
    dmd.setPixel(i,15);
  }
}

int checkCount(unsigned long num)
{
  if(num == 0) return 1;
  else if(num < 10) return 1;
  else if(num < 100) return 2;
  else if(num < 1000) return 3;
  else if(num < 10000) return 4;
  else if(num < 100000) return 5;
  else if(num < 1000000) return 6;
}

void requestEvent() 
{
  if(flag == true)
  {
    Wire.write(pData);
    Serial.println("=====Send Data to ESP=====");
    Serial.println(pData);
    flag = false;
    Serial.println("==========================");
  }
}

void clearArr(char arr[], int lengthArr)
{
  for(int i = 0; i < lengthArr; i++)
    arr[i] = '\0';
}
