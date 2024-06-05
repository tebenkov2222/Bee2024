byte aes_key[] = {48, 45, 56, 67, 67, 87, 98, 66, 50, 57, 76, 56, 67, 87, 65, 55};

byte aes_iv[N_BLOCK] = {87, 98, 66, 50, 57, 76, 56, 48, 45, 56, 67, 87, 65, 55, 67, 67};

void print_key_iv() {
  Serial.print("AES IV: ");
  for (unsigned int i = 0; i < sizeof(aes_iv); i++) {
    Serial.print(aes_iv[i], DEC);
    if ((i + 1) < sizeof(aes_iv)) {
      Serial.print(",");
    }
  }

  Serial.println("");
}

void resetiv(){
  byte input[N_BLOCK] = {87, 98, 66, 50, 57, 76, 56, 48, 45, 56, 67, 87, 65, 55, 67, 67};

  for (int i = 0; i < N_BLOCK; i++) {
    aes_iv[i] = input[i];
  }
}

void initAes(){
  aesLib.set_paddingmode(paddingMode::CMS);
}

String encrypt(char * msg) {
  resetiv();
  int msgLen = strlen(msg);
  char encrypted[3 * msgLen] = {0};
  aesLib.encrypt64((const byte*)msg, msgLen, encrypted, aes_key, sizeof(aes_key), aes_iv);
  return String(encrypted);
}

String decrypt(char * msg) {
  resetiv();
  int msgLen = strlen(msg);
  char decrypted[msgLen] = {0};
  aesLib.decrypt64(msg, msgLen, (byte*)decrypted, aes_key, sizeof(aes_key), aes_iv);
  String res = String(decrypted).substring(0, msgLen/3);
  return res;
}