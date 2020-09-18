#include <OneWire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
Servo servoNesnesi;//pin için ve servo iþlemleri için nesne tanýmlamasý.
SoftwareSerial Genotronex(0, 1); // RX, TX
int data,deger=0;
int DS18S20_Pin = 2;// Sýcaklýk sensörü baðlanan dijital pin 2 olarak belirlendi.
// Sýcaklýk Sensörü Giriþ-Çýkýþý
OneWire ds(DS18S20_Pin);  // 2. Dijital pinde.
int BluetoothData; 
int seviye = 0;
float sicaklik = 0.0;
void setup()
{
    OneWire ds(DS18S20_Pin);  // 2. Dijital pinde.
    servoNesnesi.attach(12);
    Genotronex.begin(9600);//Seri iletiþimi baþlatýr
    Serial.begin(9600); // Seri iletiþimi baþlatýr.
}
void loop()
{
    float temperature = getTemp();// temperature deðiþkeni sýcaklýk deðerini alma fonksiyonuna baðlandý.
    sicaklik = temperature;
    Serial.print("#"+(String)sicaklik+"#"+(String)seviye+"\r\n");//Telefonda yazdýrma iþlemi.
    delay(1000);
    deger=analogRead(A0); //Seviye için tanýmlanan analog pin.
    if(deger>=10)//analog pinden okunan deðere göre seviye iþlemleri.
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
      if (BluetoothData=='1') //Butona basýldýysa gelen deðer 1 ise yemleme iþlemi gerçekleþtirme.
      {
        servoNesnesi.write(180);
        delay(150);
        servoNesnesi.write(90);
      }
    }
}
float getTemp()// Bu fonksiyon DS18B20 sýcaklýk sensöründen gelen verileri Celcius cinsinden sýcaklýk deðerlerine çevirmek için kullanýlýyor.
{
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr))
  {
      //Sensörü sýfýrlama.
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7])
  {
      Serial.println("Geçerli deðil!");//Hata mesajý.
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28)
  {
      Serial.print("Cihaz tanýnmadý.");//Cihaz tanýnmadý ise.
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // Dönüþüm baþlamasý.
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
