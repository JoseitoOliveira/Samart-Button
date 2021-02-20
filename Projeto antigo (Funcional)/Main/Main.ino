#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Endereços na EEPROM de todas as variáveis
// O primeiro byte de cada endereço, diz o tamanho da palavra
// O primeiro byte da palavra esta no próximo, ou seja, no byte _Endereço+1
#define _SIID     0       // Máximo de 32 caracteres ASCII + \0 
#define _Pass     34      // Máximo de 32 caracteres ASCII + \0
#define _ServerIP 68      // Máximo de 15 caracteres ASCII + \0
#define _Port     85      // (16 bits seriam suficientes)
#define _Ident    92      // Máximo de 32 caracteres ASCII + \0
#define _IsConfigured 126 // 1 byte Variável que indica se o painél já foi ou não configurado
//Até aqui 129 bytes da memória foram utilizados
#define _EEPROMLenght 127

// Pinagem da placa
#define _rst  2   // D4
#define _bt1  12  // D6
#define _bt2  3   // RX
#define _bt3  13  // D7
#define _vd3  15  // D8
#define _vd2  16  // D0
#define _vd1  4   // D2
#define _vm1  14  // D5
#define _vm2  0   // D3
#define _vm3  5   // D1
//******************

union value {
  uint8_t bytes[2];
  uint16_t doisBytes;
};

class Porta
{

  public:
    value Dados;
    void initialize()
    {
      EEPROM.begin(_EEPROMLenght);
      Dados.bytes[0] = EEPROM.read(_Port);
      Dados.bytes[1] = EEPROM.read(_Port + 1);
      EEPROM.end();
    }
    void toEEPROM(String dado)
    {
      value dadoInt;
      dadoInt.doisBytes = std::atoi(dado.c_str());
      EEPROM.begin(_EEPROMLenght);
      EEPROM.write(_Port, dadoInt.bytes[0]);
      EEPROM.write(_Port + 1, dadoInt.bytes[1]);
      EEPROM.end();
    }
};

// Variáveis globais - Essas variáveis são lidas da EEPROM
char* SIID;
char* Pass;
char* ServerIP;
Porta Port;
char* Ident;
uint8_t IsConfigured;

// Flags de interrupções
bool f_rst = false;
bool f_bt1 = false;
bool f_bt2 = false;
bool f_bt3 = false;

// Temporizadores para debouncing por software
unsigned long t_bt1 = millis();
unsigned long t_bt2 = millis();
unsigned long t_bt3 = millis();

// Temporizador para resetar
unsigned long t_rst;

// Temporizadores para os leds
unsigned long t_vd1 = millis();
unsigned long t_vd2 = millis();
unsigned long t_vd3 = millis();

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando...");
  EEPROM.begin(_EEPROMLenght);
  IsConfigured = EEPROM.read(_IsConfigured);
  EEPROM.end();

  pinMode(_rst, INPUT_PULLUP);
  pinMode(_bt1, INPUT_PULLUP);
  pinMode(_bt2, INPUT_PULLUP);
  pinMode(_bt3, INPUT_PULLUP);
  pinMode(_vd1, OUTPUT);
  pinMode(_vd2, OUTPUT);
  pinMode(_vd3, OUTPUT);
  pinMode(_vm1, OUTPUT);
  pinMode(_vm2, OUTPUT);
  pinMode(_vm3, OUTPUT);

  //  Se o painel já está configurado
  if (IsConfigured == 1) {
    //  Lendo variaveis da EEPROM
    SIID = lerEEPROM(_SIID);
    Pass = lerEEPROM(_Pass);
    ServerIP = lerEEPROM(_ServerIP);
    Ident = lerEEPROM(_Ident);
    Port.initialize();

    //  Connect to the WiFi network
    WiFi.begin(SIID, Pass);
    Serial.println("Painel configurado, conectando a rede externa.");
    Serial.print("Conectando");

    // Interrupções dos botões com debouncing
    attachInterrupt(digitalPinToInterrupt(_bt1), interruptBt1, FALLING);
    attachInterrupt(digitalPinToInterrupt(_bt2), interruptBt2, FALLING);
    attachInterrupt(digitalPinToInterrupt(_bt3), interruptBt3, FALLING);

    //  Esperando a conexão com a rede

    waitWileIsNotConnect();

    digitalWrite(_vd1, LOW);
    digitalWrite(_vd2, LOW);
    digitalWrite(_vd3, LOW);


    //  Print the local IP
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());
  }

  //  Se o painel ainda não está configurado
  if (IsConfigured != 1) {
    //  Connect to the WiFi network
    WiFi.softAP("Painel", "");
    Serial.println("Painel não configurado, iniciando rede WIFI");

    //  Indexando a página inicial
    server.on("/", handleMain);

    //  Start the server
    server.begin();
    Serial.println("Server listening");
  }
}

void waitWileIsNotConnect() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(_vd1, !digitalRead(_vd1));
    digitalWrite(_vd2, !digitalRead(_vd2));
    digitalWrite(_vd3, !digitalRead(_vd3));
    f_rst = ResetLoongPressed();
    if (f_rst) {
      Reset();
    }
  }
}

bool sendMessage(String option) {
  WiFiClient client;
  bool sended = false;
  if (!client.connect(ServerIP, Port.Dados.doisBytes)) {
    Serial.println("connection failed");
  } else {
    String msg = Ident;
    msg += ";";
    msg += option;
    client.print(msg);
    sended = true;
  }
  client.stop();
  return sended;
}

bool ResetLoongPressed() {
  if (digitalRead(_rst) == HIGH) {
    t_rst = millis();
  }
  if (millis() - t_rst > 8000) {
    t_rst = millis();
    return true;
  }
  return false;
}


void Reset() {
  Serial.println("Resetando!");
  EEPROM.begin(_EEPROMLenght);
  EEPROM.write(_IsConfigured, 0);
  EEPROM.end();
  f_rst = false;
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1) {
    wdt_reset();
  }
}

void loop() {


  //  Se o painel ainda não está configurado
  if (IsConfigured != 1) {
    server.handleClient(); //Handling of incoming requests
  }

  if (IsConfigured == 1) {
    waitWileIsNotConnect();
    if (ResetLoongPressed()) {
      Reset();
    }
    if (digitalRead(_vd1) == HIGH and millis() - t_vd1 > 3000) {
      digitalWrite(_vd1, LOW);
    }
    if (digitalRead(_vd2) == HIGH and millis() - t_vd2 > 3000) {
      digitalWrite(_vd2, LOW);
    }
    if (digitalRead(_vd3) == HIGH and millis() - t_vd3 > 3000) {
      digitalWrite(_vd3, LOW);
    }
    if (f_bt1) {
      String message = "Tenho fome";
      Serial.println(message);
      f_bt1 = false;
      if (sendMessage(message)) {
        digitalWrite(_vd1, HIGH);
        t_vd1 = millis();
      }
    }
    if (f_bt2) {
      String message = "Tenho sede";
      Serial.println(message);
      Serial.print(ServerIP);
      Serial.print(":");
      Serial.println(Port.Dados.doisBytes);
      f_bt2 = false;
      if (sendMessage(message)) {
        digitalWrite(_vd2, HIGH);
        t_vd2 = millis();
      }
    }
    if (f_bt3) {
      String message = "Não me sinto bem";
      Serial.println(message);
      f_bt3 = false;
      if (sendMessage(message)) {
        digitalWrite(_vd3, HIGH);
        t_vd3 = millis();
      }
    }
  }
}
void interruptBt1() {
  if (millis() - t_bt1 > 500) {
    f_bt1 = true;
    t_bt1 = millis();
  }
}
void interruptBt2() {
  if (millis() - t_bt2 > 500) {
    f_bt2 = true;
    t_bt2 = millis();
  }
}
void interruptBt3() {
  if (millis() - t_bt3 > 500) {
    f_bt3 = true;
    t_bt3 = millis();
  }
}

void handleMain() {
  if (server.hasArg("SIID") == false) { //Check if body received
    server.send(200, "text/html", siteMain());
  }
  if (server.hasArg("SIID") == true) {
    server.send(200, "text/html", siteResposta());
    delay(50);

    escreveEEPROM(_SIID    , toChar(server.arg("SIID")));
    escreveEEPROM(_Pass    , toChar(server.arg("Pass")));
    escreveEEPROM(_ServerIP, toChar(server.arg("ServerIP")));
    escreveEEPROM(_Ident   , toChar(server.arg("Ident")));
    Port.toEEPROM(server.arg("Port"));

    EEPROM.begin(_EEPROMLenght);
    EEPROM.write(_IsConfigured, 1);
    EEPROM.end();

    Serial.println("Leitura da EEPROM");
    Serial.print("SIID: ");
    printChar(lerEEPROM(_SIID));
    Serial.print("\nPass: ");
    printChar(lerEEPROM(_Pass));
    Serial.print("\nServerIP: ");
    printChar(lerEEPROM(_ServerIP));
    Serial.print("\nPort: ");
    Serial.print(Port.Dados.doisBytes);
    Serial.print("\nIdent: ");
    printChar(lerEEPROM(_Ident));
    Serial.println();

    WiFi.forceSleepBegin();
    wdt_reset();
    ESP.restart();
    while (1) {
      wdt_reset();
    }
  }
}
