char* readEEPROM(int endereco) {
  int i;

  // Inicia a EEPROM com o tamanho definido
  EEPROM.begin(_EEPROMLenght);

  // Obtem o tamanho do vetor do primeiro endereço
  int tamanho = EEPROM.read(endereco);

  //
  char* retorno = new char[tamanho + 1];

  // Ler bytes apartir de endereço[1] até endereço[tamanho]
  for (i = 0; i < tamanho; i++) {
    retorno[i] = (char)EEPROM.read(1 + i + endereco);
  }

  // Nesse ponto, i = tamanho+1, é adicionado o identificador de fim de palavra
  retorno[i] = '\0';

  // Finaliza a EEPROM
  EEPROM.end();

  return retorno;
}
char* toChar(String str) {
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return cstr;
}

void printChar(char* msg) {
  for (int i = 0; msg[i] != '\0'; i++) {
    Serial.write(msg[i]);
  }
}

void escreveEEPROM(int endereco, char* mensagem) {
  int tamanho;

  // Inicia a EEPROM com o tamanho definido
  EEPROM.begin(_EEPROMLenght);

  // Lendo o tamanho da mensagem
  for (tamanho = 0; mensagem[tamanho] != '\0'; tamanho++);

  // Salvando o tamanho da mensagem no endereço[0]
  EEPROM.write(endereco, tamanho);
  EEPROM.commit();

  // Gravando a mensagem a partir de endereço[1]
  for (int i = 0; i < tamanho; i++) {
    EEPROM.write(endereco + i + 1, mensagem[i]);
    EEPROM.commit();
  }
  // Finaliza a EEPROM
  EEPROM.end();
}
