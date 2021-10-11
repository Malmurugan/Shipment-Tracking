#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(10, 11);
String latlongtab[5];
#define DEBUG true
String state, timegps, latitude, longitude;
boolean pin2=LOW,pin3=LOW,pin4=LOW,pin5=LOW,pin6=LOW; 
float temp=0.0;
//float latitude=22.794952;
//float longitude=75.855545;
int val = 0;
int flag=0;
const int ap1 = A5; 
const int ap2 = A4;
const int ap3 = A3;
int x=0,y=0,z=0,i=0;
int sv1 = 0,ov1=0,sv2=0,ov2=0,sv3=0,ov3=0;       
void setup()
{
  sendData("AT",1000,DEBUG);
  delay(1000);
  sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",1000,DEBUG);
  delay(1000);
  sendData("AT+SAPBR=3,1,\"APN\",\"\"",1000,DEBUG);
  delay(1000);
  mySerial.begin(9600);               // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);  
  pinMode(6,INPUT);  
  delay(1000);
}
 
void loop()
{ sendTabData("AT+CIPGSMLOC=1,1",1000,DEBUG);
  delay(3000);
  
  
    Serial.println("Latitude:"+latitude);
    Serial.println("Longitude:"+longitude);
    delay(500);   
  
  
  analogReference(EXTERNAL);
  sv1 = analogRead(ap1);          
  ov1 = map(sv1, 0, 1023, 0, 255);
  delay(2);                     
  sv2 = analogRead(ap2);            
  ov2 = map(sv2, 0, 1023, 0, 255); 
  delay(2);                 
  sv3 = analogRead(ap3);            
  ov3 = map(sv3, 0, 1023, 0, 255); 
  
   if(i!=0)
  {
  if(x>(ov1+10) || ov1<(x-10) || y>(ov2+10) || ov2<(y-10) || z>(ov3+10) || ov3<(z-10))
  { 
    Serial.println("FALLLLLLLLL");
    flag=1;
  }}
  x=ov1;
  y=ov2;
  z=ov3;
  i++;
      
      
      analogReference(DEFAULT);
      temp=analogRead(A0);
      temp=temp*0.4887;  
      delay(2);          
       Send2Pachube();
   
  if (mySerial.available())
    Serial.write(mySerial.read());
}
void Send2Pachube()
{
  mySerial.println("AT");
  delay(1000);

  mySerial.println("AT+CPIN?");
  delay(1000);

  mySerial.println("AT+CREG?");
  delay(1000);

  mySerial.println("AT+CGATT?");
  delay(1000);

  mySerial.println("AT+CIPSHUT");
  delay(1000);

  mySerial.println("AT+CIPSTATUS");
  delay(2000);

  mySerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+CSTT=\"internet\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET http://api.thingspeak.com/update?api_key=SEA0DTC89QCRDBGO&field1="+ String(temp)+"&field2="+String(latitude)+"&field3="+String(longitude)+"&field4="+String(flag);
  mySerial.println(str);//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str1="GET http://api.thingspeak.com/update?api_key=N1OZCQRN83TOZYC7&field2="+ String(latitude);
  mySerial.println(str);//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str2="GET http://api.thingspeak.com/update?api_key=N1OZCQRN83TOZYC7&field3="+ String(longitude);
  mySerial.println(str);//begin send data to remote server
  delay(4000);
  ShowSerialData();


  mySerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  mySerial.println();
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
  flag=0;
} 
void ShowSerialData()
{
  while(mySerial.available()!=0)
    Serial.write(mySerial.read());
}
void sendTabData(String command,const int timeout,boolean debug)
{
  mySerial.println(command);
  long int time=millis();
  int i=0;
  while((time+timeout)>millis()){
    while(mySerial.available()){
      char c=mySerial.read();
      if(c!=','){
        latlongtab[i]+=c;
        delay(100);
      }
      else{
        i++;
      }
      if(i==3)
      {
        delay(100);
        goto exitL;
      }
      
    }
  }exitL:
  if(debug){
    latitude=latlongtab[1];
    longitude=latlongtab[2];
    }
}
String sendData(String command,const int timeout,boolean debug)
{
  String response="";
  mySerial.println(command);
  long int time=millis();
  int i=0;
  while((time+timeout)>millis()){
   while(mySerial.available()){
      char c=mySerial.read();
      response+=c;
   }
  }
  if(debug){
    Serial.print(response);
  }
  return response;
}