#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
//Projeto baseado em https://www.youtube.com/watch?v=UkIt1wvq-Tk
//https://labdegaragem.com/forum/topics/high-e-low-no-esp8266
//Se o LED integrado na placa, estiver em current  a lógica de acionamento é invertida, mas se for source é logica direta (normalmente usada).?
#define HI LOW
#define LO HIGH

#define pinLED 2  //GP2 ESP-01


//WiFi
const char* SSID = "Nome_da_sua_rede";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "senha_da_sua_rede";           // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "mqtt.eclipseprojects.io";  //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                               // Porta do Broker MQTT

#define ID_MQTT  "WPSN9999"                           //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_SUBSCRIBE "WPSN1111"                    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
#define TOPIC_PUBLISH "WPSN0000"                      //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);                        // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();                                //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();                                   //Faz conexão com WiFi
void conectaMQTT();                                   //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {

  pinMode(2, OUTPUT);                                 // Initialize the BUILTIN_LED pin as an output         

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(recebePacote); 
}

void loop() {
  mantemConexoes();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi();                                    //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}


void piscaLed(int pino, int qntd){
int controle = 0;
MQTT.publish(TOPIC_PUBLISH,"Estou piscando");
for (int i=0;i<qntd;i++){
  digitalWrite(pino, HI);
  delay(500); 
  digitalWrite(pino, LO); 
  delay(500);
  controle +=1; 
}
if (controle == qntd){
MQTT.publish(TOPIC_PUBLISH, "Parei de piscar. Estou apagado.");  
}
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    Serial.println(msg);

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
       Serial.println("msg dentro do for: "+ msg);
    }

    if (msg == "0") {
        Serial.println("Entrei no if == 0");
        digitalWrite(2, LO);  
        MQTT.publish(TOPIC_PUBLISH, "Estou apagado");                        
    }                     

    if (msg == "1") {
       Serial.println("Entrei no if == 1");
       digitalWrite(2, HI);
       MQTT.publish(TOPIC_PUBLISH, "Estou acesso");
    }

    if (msg == "2"){piscaLed(2, 2);}
    if (msg == "3"){piscaLed(2, 3);}
    if (msg == "4"){piscaLed(2, 4);}
    if (msg == "5"){piscaLed(2, 5);}
    if (msg == "6"){piscaLed(2, 6);}
    if (msg == "7"){piscaLed(2, 7);}
    if (msg == "8"){piscaLed(2, 8);}
    if (msg == "9"){piscaLed(2, 9);}
    
}
