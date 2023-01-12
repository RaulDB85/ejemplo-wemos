#include <WiFiClientSecure.h>    // Biblioteca WiFi
#include <UniversalTelegramBot.h> // Biblioteca Telegram
#include "SparkFunBME280.h"   // Biblioteca Sensor BME280
#define relePin 8
 
BME280 mySensorB; // I2C address 0x76 
 
// Datos de conexión con la red WiFi
const char* ssid     = "RED_Lab_1";
const char* password = "mICroLab_2020";
 
float p;   
float t;  
float h;
char msg[30];
 
// Inicia el bot de telegram con el bot token generado por Botfather
#define BOTtoken "442837211:ABFlea9Ya-JI_SOzxiP4AGSw8FFKwhyaAAM" 
  
 
WiFiClientSecure client;      // Instancia para la red WiFi
UniversalTelegramBot bot(BOTtoken, client); // Instancia para el Bot Telegram
 
int Bot_mtbs = 200; // Espera entre mensajes
long Bot_lasttime;   
bool Start = false, bandera = false;
 
int releStatus = 0;
void Leer_Sensor(void); // Función para leer el sensor BME280
String chat_id;
String text;
String from_name;
 
void handleNewMessages(int numNewMessages) {
  
  for (int i=0; i<numNewMessages; i++) {
     chat_id = String(bot.messages[i].chat_id);
     text = bot.messages[i].text;
 
    from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";
 
    if (text == "/encender") {
      digitalWrite(relePin, HIGH);   // Rele encendido
      releStatus = 1;
      bot.sendMessage(chat_id, "Calefaccion encendida!!", "");
      bandera = true;
    }
 
    if (text == "/apagar") {
      releStatus = 0;
      digitalWrite(relePin, LOW);    // Rele apagado
      bot.sendMessage(chat_id, "Calefaccion Apagada!!", "");
      bandera = true;
    }
 
    //Comando para conocer el estado de la calefaccion
    if (text == "/estado") {
      if(releStatus){
        bot.sendMessage(chat_id, "La calefaccion esta encendida", "");
        bandera = true;
      } else {
        bot.sendMessage(chat_id, "La calefaccion esta apagada", "");
        bandera = true;
      }
    }

    //Comando para conocer los datos del sensor
    if (text == "/sensor") {  //envia los datos del sensor
       Leer_Sensor();   // Rutina del sensor BME280
       bandera = true;
    }
    
    //Comando para iniciar el chat-bot
    if (text == "/start") { // Mensaje al inicar el chat con el bot
      String welcome = "Bienvenido al Bot Telegram Calefacción, " + from_name + ".\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
      bandera = true;
    }
 
    else // Si el comando no es correcto envía mensaje de error con emoticon
         // Emoticones https://apps.timwhitlock.info/emoji/tables/unicode
      {
        if(bandera == false){
        String welcome = "Comando invalido \xF0\x9F\x98\x9E";
        bot.sendSimpleMessage(chat_id, welcome, "");
        }
      }
      bandera = false;
  }
}
 
void setup() {
pinMode(relePin, OUTPUT); // relePin es salida
  delay(10);
  digitalWrite(relePin, LOW); // inicia apagado
  
  Serial.begin(115200);
  
  // Conectando con la red Wifi:
  Serial.print("Conectando con Wifi: ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
 
  mySensorB.setI2CAddress(0x76); // Conectando con el sensor BME280
  if(mySensorB.beginI2C() == false) Serial.println("ERROR! Sensor no encontrado");
  }
 
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
 
    while(numNewMessages) {
      Serial.println("LLegó un mensaje!!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
      Bot_lasttime = millis();
  }
}

//Funcion para leer datos y mandarlos por telegram
void Leer_Sensor(void)
{
  t = mySensorB.readTempC();
  snprintf (msg, 28, "Temperatura: %2.1f  \xF0\x9F\x86\x97 \n", t); 
  bot.sendMessage(chat_id, msg, "Markdown");
  
  p = mySensorB.readFloatPressure()/100;
  snprintf (msg, 25, "Presión: %3.1f  \xF0\x9F\x86\x97 \n", p);
  bot.sendMessage(chat_id, msg, "Markdown"); 
 
  h = mySensorB.readFloatHumidity();
  snprintf (msg, 28, "Humedad: %2.1f %%  \xF0\x9F\x86\x97 \n", h);  
  bot.sendMessage(chat_id, msg, "Markdown");
}
