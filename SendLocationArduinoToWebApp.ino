//mejiamanuel57@gmail.com

#include <SoftwareSerial.h>   // libreria de multiples puertos seriales.
#include <TinyGPS.h>         //libreria para el GPS
#include <string.h>         //para manipulacion de string

SoftwareSerial GSM_RxTx(2,3);  //declaramos: Pin 2 como rx y  Pin 3 como tx (GSM-Arduino) (Solo funciona con estos pines el GSM ).
SoftwareSerial GPS_RxTx(5,4); //declaramos: Pin 4 como rx y  Pin 5 como tx (GPS-Arduino)
TinyGPS gps; //declaramos la variable tipo TinyGPS

float latitud ;
float longitud ;
boolean repetir = false; // para repetir el proceso de enviar la data


void setup()
{ 
  Serial.begin(115200);   // velocidad para el serial monitor (baud)
  GSM_RxTx.begin(9600);  // velocidad de transmision entre el mudulo GSM y el arduino (baud)

  Serial.println("Iniciando modulo GSM y esperando valores del GPS...");
  char c;
  while(c!='4') // para asegurar que exista conexion con sms
  {
    if(GSM_RxTx.available()>0)
    {
      c = GSM_RxTx.read(); 
      Serial.print(c);
    }
  }    
  Serial.println("");
  Serial.println("Listo!");

  GPS_RxTx.begin(9600);// velocidad de transmision entre el GPS y el arduino (baud)
  
}


void loop()
{

  GPS_RxTx.listen();
  funcionGPS();

  GSM_RxTx.listen();
  funcionGPRS(); //Entra a la funcion y chequea alguna solitud
  
}


//----------------------------------------------------------------------------------------------------------------------------------funcionGPRS()
void funcionGPRS()
{   
        
  char temporal1[15]; // variable temporal para almacenar
  char temporal2[15];

  String parte1 = "GET http://compass-1.apphb.com/Lectura.aspx?latitud=";
  String lat = String(dtostrf(latitud,1,5,temporal1));
  String parte2 = "&longitud=";
  String lon = String(dtostrf(longitud,1,5,temporal2));
  String parte3 = " HTTP/1.0";
  //String stringDato="GET http://compass-1.apphb.com/Lectura.aspx?latitud=18.467281&longitud=-69.962204 HTTP/1.0"; // mi casa
  //String stringDato="GET http://compass-1.apphb.com/Lectura.aspx?latitud=18.449975&longitud=-69.975167 HTTP/1.0"; // plaza 

 if( repetir == false)
 {

    GSM_RxTx.println("AT+CGDCONT=1,\"IP\",\"orangenet.com.do\""); //apn orange
    //GSM_RxTx.println("AT+CGDCONT=1,\"IP\",\"internet.ideasclaro.com.do\"");  // apn claro
    Serial.println("APN listo");
    delay(1000);
    while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }


    GSM_RxTx.println("AT+CGPCO=0,\"\",\"\",1"); 
    Serial.println("Usuario y password del APN listo");
    delay(1000);
    while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }
  }
  
    GSM_RxTx.println("AT+CGACT=1,1"); 
    Serial.println("APN adjunto listo");
    delay(2000);
    while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }
  
  
    GSM_RxTx.println("AT+SDATACONF=1,\"TCP\",\"deliveriando.apphb.com\",80");
    Serial.println("Comunicacion TCP con la pagina (puerto 80) lista");
    delay(2000);
    while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }
  

  GSM_RxTx.println("AT+SDATASTART=1,1");
  Serial.println("Comenzar conecxion TCP/UDP");
  delay(2000);
  while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }
    

  GSM_RxTx.println("AT+SDATASTATUS=1");
  Serial.println("Consultar el estado de conexion de socket");
  delay(3000);
  while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
    }



  int x10=(parte1.length()+lat.length()+parte2.length()+lon.length()+parte3.length()+4);
  Serial.println(x10);
  GSM_RxTx.print("AT+SDATATSEND=1,");
  GSM_RxTx.println(x10);
  delay(1000);
  GSM_RxTx.print(parte1);
  GSM_RxTx.print(lat);
  GSM_RxTx.print(parte2);
  GSM_RxTx.print(lon);
  GSM_RxTx.print(parte3);
  GSM_RxTx.print((char)13);
  GSM_RxTx.print((char)10);

  GSM_RxTx.print((char)13);                        
  GSM_RxTx.print((char)10);                         

  GSM_RxTx.print((char)26);
  Serial.println("finalizo");
  delay(2000);
  repetir = true;
  
  
  while(GSM_RxTx.available()>0) // para vereficar si funciono.
    {
      char c = GSM_RxTx.read();  Serial.print(c);
      if (c == '4' || c == '2')
      {
        Serial.println("reinicia el arudino hay problemas!!!!!!!!!!!!!!!!!!!!!!");
      }
    }
 
}

//----------------------------------------------------------------------------------------------------------------------------------funcionGPS()
void funcionGPS()
{
  bool sentenciaValida = false;

  for (unsigned long start = millis(); millis() - start < 1000;)// por un segundo analizamos la data del GPS (necesario)
  {
    while (GPS_RxTx.available()) // si escucha algo
    {
      char c = GPS_RxTx.read(); // guardalo en c

      if (gps.encode(c)) // es una sentencia validad?
      {
        sentenciaValida = true;
      }
    }
  }

  if (sentenciaValida == true)
  {
    gps.f_get_position(&latitud, &longitud); // dame latitud y longitud
    Serial.print("Lat/Long: "); 
    Serial.print(latitud,5); 
    Serial.print(", "); 
    Serial.println(longitud,5); 
  }


}








