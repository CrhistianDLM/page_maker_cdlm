#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
SoftwareSerial blue(2, 3);
int temp=0;
char val;

const uint8_t EchoPin = 5;//sensor ultrasonido
const uint8_t TriggerPin = 6;//sensor ultrasonido
uint8_t distancia=0;
uint8_t analog_pin = A0;//lectura de temp

uint8_t humedad_pin = A1;
float temperatura;
uint8_t led = 13; //señal de rele para activar riego

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0x98, 0x4F, 0xEE, 0x01, 0x44, 0x4D};
IPAddress ip(192, 168, 1, 176);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

//Variable para detectar cuando presionan el botón
String readString;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
    blue.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(led, OUTPUT);     

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac,ip);
  server.begin();
}


void loop() {
  distancia=conversion(TriggerPin, EchoPin);
 temperatura = analogRead(analog_pin);
  temperatura = (500.0*temperatura)/1024;
  // listen for incoming clients
  blue.println(temp);
  EthernetClient client = server.available();
  if (client) {
    Serial.println("nc");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (readString.length() < 100) {readString += c;}
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh:10");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE html>");
          client.println("<html><head><title>Sistema de Control de Cultivos Autosustentables</title>");
          client.println("<link rel='stylesheet' type='text/css' href='http://cdlm-dweb.net23.net/toques.css'/>");

          client.println("</head>");
          // output the value of each analog input pin
       
          //Controles para regar la planta
client.println("<body onload='drawPict();'>");
client.println("<div class='body-cdlm row'>");
client.println("<div class='title-body'><span class='glyphicon glyphicon-globe'><H1>Intel</H1></span></div><br/>");
         client.println("<canvas id='example' width='500' height='500'>");
         client.println("t");
client.println("</canvas>");
          //mostrar datos
          client.println("Nivel del tanque: "); 
          client.print(distancia);
          client.println("Temp planta: ");
          client.print(temperatura);
          client.println(" oC");
          client.println("Nivel de humedad: ");
          client.println(humedad_pin);
          client.println("<script type='text/javascript'>");
          if (analogRead(humedad_pin) > 400){
            client.println("sessionStorage.setItem('nCara', 3);"); 
          }
          else{
            client.println("sessionStorage.setItem('nCara', 1);"); 
          }
          client.println("</script>"); 

          //delay(300);
          client.println("<button type= 'button' class='btn btn-success' onclick='actBomba()'>Dar Agua</button>");
          client.println("<p>Intel</p>");
          client.println("</div>");
          client.println("<script type='text/javascript' src='http://cdlm-dweb.net23.net/maker.js'></script>");
          client.println("<script type='text/javascript'>sessionStorage.setItem('nCara', 1);cara(sessionStorage.getItem('nCara'));</script>");
          client.println("</body></html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
   client.stop();
   if (readString.indexOf("?btnregar") >0)   {
      digitalWrite(led, HIGH);
      delay (2000);}
   if (readString.indexOf("?btnparar") >0)   {
      digitalWrite(led, LOW);
      delay (2000);}
      readString=""; 
    Ethernet.maintain();
  }
}

//funcion para leer datos del sensor de ultrasonido

float conversion(uint8_t TriggerPin, uint8_t EchoPin) {
  float duracion, distancia;
  
  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duracion = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  distancia = duracion * 0.01715;//343m/s*100cm/1000000us=0.01715convertimos a distancia, en cm
  return distancia;
}

