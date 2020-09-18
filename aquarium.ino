#include <OneWire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
Servo servoNesnesi;//pin i�in ve servo i�lemleri i�in nesne tan�mlamas�.
SoftwareSerial Genotronex(0, 1); // RX, TX
int data,deger=0;
int DS18S20_Pin = 2;// S�cakl�k sens�r� ba�lanan dijital pin 2 olarak belirlendi.
// S�cakl�k Sens�r� Giri�-��k���
OneWire ds(DS18S20_Pin);  // 2. Dijital pinde.
int BluetoothData; 
int seviye = 0;
float sicaklik = 0.0;
void setup()
{
    OneWire ds(DS18S20_Pin);  // 2. Dijital pinde.
    servoNesnesi.attach(12);
    Genotronex.begin(9600);//Seri ileti�imi ba�lat�r
    Serial.begin(9600); // Seri ileti�imi ba�lat�r.
}
void loop()
{
    float temperature = getTemp();// temperature de�i�keni s�cakl�k de�erini alma fonksiyonuna ba�land�.
    sicaklik = temperature;
    Serial.print("#"+(String)sicaklik+"#"+(String)seviye+"\r\n");//Telefonda yazd�rma i�lemi.
    delay(1000);
    deger=analogRead(A0); //Seviye i�in tan�mlanan analog pin.
    if(deger>=10)//analog pinden okunan de�ere g�re seviye i�lemleri.
    {
      seviye = 1;
      digitalWrite(13,HIGH);
      Serial.print("#"+(String)sicaklik+"#"+(String)seviye+"\r\n");
    }
    else
    {
      seviye = 0;
      digitalWrite(13,LOW);
      Serial.print("#"+(String)sicaklik+"#"+(String)seviye+"\r\n");
    }
    if (Genotronex.available())
    {
      BluetoothData=Genotronex.read();//bluetooth degerini okuma.
      if (BluetoothData=='1') //Butona bas�ld�ysa gelen de�er 1 ise yemleme i�lemi ger�ekle�tirme.
      {
        servoNesnesi.write(180);
        delay(150);
        servoNesnesi.write(90);
      }
    }
}
float getTemp()// Bu fonksiyon DS18B20 s�cakl�k sens�r�nden gelen verileri Celcius cinsinden s�cakl�k de�erlerine �evirmek i�in kullan�l�yor.
{
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr))
  {
      //Sens�r� s�f�rlama.
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7])
  {
      Serial.println("Ge�erli de�il!");//Hata mesaj�.
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28)
  {
      Serial.print("Cihaz tan�nmad�.");//Cihaz tan�nmad� ise.
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // D�n���m ba�lamas�.
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); 
  for (int i = 0; i < 9; i++)
  { 
      data[i] = ds.read();
  }
  ds.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}
