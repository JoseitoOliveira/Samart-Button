//Projeto eletrônica versão 3.2
// Nessa versão vou salvar cada canal de cor em um único byte

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Endereços na EEPROM de todas as variáveis

#define _IsConfigured     0   // 1 byte Variável que indica se o painél já foi ou não configurado
#define _SIID             1   // Máximo de 32 caracteres ASCII + \0 
#define _Pass            34   // Máximo de 32 caracteres ASCII + \0
#define _ServerIP        67   // Máximo de 15 caracteres ASCII + \0
#define _Port            83   // 2 Bytes
#define _Ident           85   // Máximo de 32 caracteres ASCII + \0
#define _Button1        118   // 3 Bytes + máximo de 32 caracteres ASCII + \0
#define _Button2        154   // 3 Bytes + máximo de 32 caracteres ASCII + \0
#define _Button3        190   // 3 Bytes + máximo de 32 caracteres ASCII + \0
#define _EEPROMLenght   225

// Pinagem da placa
#define _rst  2   // D4
#define _bt1  12  // D6
#define _bt2  3   // RX
#define _bt3  13  // D7
#define _vd3  15  // D8
#define _vd2  16  // D0
#define _vd1  4   // D2
//******************

//Protótipos de função
char* readEEPROM(int);
char* toChar(String);
void printChar(char*);
void escreveEEPROM(int, char*);


union value {
  uint8_t bytes[2];
  uint16_t doisBytes;
};

class Button {
  private:
    int address;
    String Ident;
  public:
    byte red, green, blue;
    String message;

    Button(int address) {
      this->address = address;
      this->Read();
    }

    String jsonMessage() {
      String json = "{"
                    "I:\"" + Ident + "\","
                    "M:\"" + message + "\","
                    "R:" + red + ","
                    "G:" + green + ","
                    "B:" + blue +
                    "}"
                    ;
      return json;
    }

    void Read() {
      Ident = readEEPROM(_Ident);
      message = readEEPROM(address + 3);
      EEPROM.begin(_EEPROMLenght);
      red = EEPROM.read(address + 0);
      green = EEPROM.read(address + 1);
      blue = EEPROM.read(address + 2);
      EEPROM.end();
    }

    void Set(String color, String message) {
      this->message = message;
      escreveEEPROM(address + 3, toChar(message));

      color.remove(0, 1);                       //the character in index 0 is #
      red = (byte) strtol( color.substring(0, 2).c_str(), NULL, 16);
      green = (byte) strtol( color.substring(2, 4).c_str(), NULL, 16);
      blue = (byte) strtol( color.substring(4, 6).c_str(), NULL, 16);

      EEPROM.begin(_EEPROMLenght);

      EEPROM.write(address + 0, red);
      EEPROM.commit();
      EEPROM.write(address + 1, green);
      EEPROM.commit();
      EEPROM.write(address + 2, blue);
      EEPROM.commit();

      EEPROM.end();
    }

    void Print() {
      Serial.print("Message: ");
      Serial.println(message);
      Serial.print("  Red: ");
      Serial.println(red, HEX);
      Serial.print("  Green: ");
      Serial.println(green, HEX);
      Serial.print("  Blue: ");
      Serial.println(blue, HEX);
    }
};


class Porta {
  private:
    int address;
  public:
    value Dados;

    Porta(int address) {
      this->address = address;
      this->Read();
    }

    void Read() {
      EEPROM.begin(_EEPROMLenght);
      Dados.bytes[0] = EEPROM.read(address);
      Dados.bytes[1] = EEPROM.read(address + 1);
      EEPROM.end();
    }

    void Set(String dado) {
      value dadoInt;
      dadoInt.doisBytes = (uint16_t)dado.toInt();
      Dados = dadoInt;
      EEPROM.begin(_EEPROMLenght);
      EEPROM.write(address, dadoInt.bytes[0]);
      EEPROM.write(address + 1, dadoInt.bytes[1]);
      EEPROM.end();
    }
};

// Variáveis globais - Essas variáveis são lidas da EEPROM
uint8_t IsConfigured;
char* SIID;
char* Pass;
char* ServerIP;
char* Ident;
Porta Port(_Port);
Button button1(_Button1);
Button button2(_Button2);
Button button3(_Button3);

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

  SIID = readEEPROM(_SIID);
  Pass = readEEPROM(_Pass);
  ServerIP = readEEPROM(_ServerIP);
  

  //  Se o painel já está configurado
  if (IsConfigured == 1) {

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
      break;
    }
  }
}

bool sendMessage(String msg) {
  WiFiClient client;
  bool sended = false;
  if (client.connect(ServerIP, Port.Dados.doisBytes)) {
    Serial.println(msg);
    client.print(msg);
    sended = true;

  } else {
    Serial.println("connection failed");
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
  setup();
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
      button1.Print();
      f_bt1 = false;
      if (sendMessage(button1.jsonMessage())) {
        digitalWrite(_vd1, HIGH);
        t_vd1 = millis();
      }
    }
    if (f_bt2) {
      button2.Print();
      Serial.print(ServerIP);
      Serial.print(":");
      Serial.println(Port.Dados.doisBytes);
      f_bt2 = false;
      if (sendMessage(button2.jsonMessage())) {
        digitalWrite(_vd2, HIGH);
        t_vd2 = millis();
      }
    }
    if (f_bt3) {
      button3.Print();
      f_bt3 = false;
      if (sendMessage(button3.jsonMessage())) {
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

    Port.Set(server.arg("Port"));

    button1.Set(server.arg("ColorButton1"), server.arg("MessageButton1"));
    button2.Set(server.arg("ColorButton2"), server.arg("MessageButton2"));
    button3.Set(server.arg("ColorButton3"), server.arg("MessageButton3"));

    EEPROM.begin(_EEPROMLenght);
    EEPROM.write(_IsConfigured, 1);
    EEPROM.end();

    Serial.println("Leitura da EEPROM");
    Serial.print("SIID: ");
    printChar(readEEPROM(_SIID));
    Serial.print("\nPass: ");
    printChar(readEEPROM(_Pass));
    Serial.print("\nServerIP: ");
    printChar(readEEPROM(_ServerIP));
    Serial.print("\nPort: ");
    Serial.print(Port.Dados.doisBytes);
    Serial.print("\nIdent: ");
    printChar(readEEPROM(_Ident));
    Serial.println("\nButton 1");
    button1.Print();
    Serial.println("\nButton 2");
    button2.Print();
    Serial.println("\nButton 3");
    button3.Print();

    delay(50);
    Serial.println("Reiniciando!");
    setup();
  }
}
