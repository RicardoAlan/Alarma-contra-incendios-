#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h> //Importamos la libreria para usar el WiFi

#define WIFISID "" //Nombre de la red
#define WIFIPASS ""  //Contraseña de la red

#define HOSTIFTTT "maker.ifttt.com" //Definimos los parámetros para el IFTTT
#define EVENTO ""
#define IFTTTKEY ""
ESP8266WiFiMulti WiFiMulti;
WiFiClientSecure client;
bool ejecutado = false; // Variable que permite ejecutar una sola vez la función

int pin = A0; //Análogico, sensor de temperatura al pin A0
int pin1 = D5; //Digital, Buzzer al pin D5
int pin2 = D2; //Digital, sensor de llama al pin D2
int pin3 = D1; //Digital, sensor de humo al pin D0
int d=0;

void setup()
{
  Serial.begin(9600); //configura el puerto serie a 9600bps
  WiFiMulti.addAP(WIFISID, WIFIPASS); //Comando tomado de la librería, "enciende" el WiFi
  while(WiFi.status()!=WL_CONNECTED) //De nuevo se usan comandos de la libreria importada, mientras se establece conexión aparece el mensaje de abajo
  {
    delay(500);
    Serial.print("Esperando establecer conexión con el WiFi .");
  }
  Serial.print("Se ha establecido la conexión "); //Ya se hizo conexión 
  Serial.print("La dirección IP es: ");
  Serial.print(WiFi.localIP()); //Nos muestra la IP a la que se conectó 
  
  pinMode(pin1, OUTPUT); //Sacamos una señal, que suene el Buzzer
  pinMode(pin2, INPUT); //Recibimos una señal, hay fuego o no lo hay
  pinMode(pin3, INPUT); //Recibimos una señal, hay humo o no
  
}

void loop()
{
  float temperatura = ((analogRead(pin)*330)/1024); //Conversión de los datos recibidos por el sensor a grados centigrados
  Serial.println(temperatura); //Envia valor análogico
  delay(500); //Retraso en el recibimiento de datos
  
  int fuego = (digitalRead(pin2)); //Recibimos la señal del sensor de fuego
  delay(500);

  int humo = (digitalRead(pin3)); //Recibimos una señal del sensor de humo
  delay(500);
  
  if (fuego == LOW && temperatura > 40.0 && humo == LOW )//&& d==0)//Si la temperatura es mayor a 45°C, detecta fuego y además detecta humo entonces suena
  {
    digitalWrite(pin1, HIGH);
    if(d==0){
    alerta_de_incendio(temperatura);
    d = 1;
    }
    
  }
  else 
  {
    digitalWrite(pin1,LOW);
    d = 0;
  }
}

void alerta_de_incendio(int temperatura)
{
  // Cerramos cualquier conexión anterior
  if (client.connected())
  {
    client.stop();
  }
 
  // Esperamos hasta que se hayan enviado todos los datos
  client.flush();
 
  // Hacemos la petición mediante SSL
  if (client.connect(HOSTIFTTT, 443)) {
    // Construimos la petición HTTP
    String toSend = "GET /trigger/";
    toSend += EVENTO;
    toSend += "/with/key/";
    toSend += IFTTTKEY;
    toSend += "?value1=";
    toSend += temperatura;
    toSend += " HTTP/1.1\r\n";
    toSend += "Host: ";
    toSend += HOSTIFTTT;
    toSend += "\r\n";
    toSend += "Connection: close\r\n\r\n";
 
    client.print(toSend);
  }
 
  // Esperamos hasta que se hayan enviado todos los datos
  client.flush();
  // Desconectamos del cliente
  client.stop();
}
//https://programarfacil.com/blog/arduino-blog/como-ifttt-arduino-nodemcu/
