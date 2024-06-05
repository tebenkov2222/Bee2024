/* Example taking test values from node.js server as input as well... */

const char* plaintext = "Looks like key but it's not me.";
int loopcount = 0;

char cleartext[256] = {0};
char ciphertext[512];

byte aes_key[] = {48, 45, 56, 67, 67, 87, 98, 66, 50, 57, 76, 56, 67, 87, 65, 55};
// AES Encryption Key

// General initialization vector (you must use your own IV's in production for full security!!!)
const byte aes_iv_default[N_BLOCK] = {87, 98, 66, 50, 57, 76, 56, 48, 45, 56, 67, 87, 65, 55, 67, 67};
byte aes_iv[N_BLOCK] = {87, 98, 66, 50, 57, 76, 56, 48, 45, 56, 67, 87, 65, 55, 67, 67};

String server_b64msg = "MMxseakr0Imv41KC1a+zRQ=="; // CBC/Zeropadding; same as aes_iv  but in Base-64 form as received from server

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

String encrypt_impl(char * msg) {
  resetiv();
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen] = {0};
  aesLib.encrypt64((const byte*)msg, msgLen, encrypted, aes_key, sizeof(aes_key), aes_iv);
  return String(encrypted);
}

String decrypt_impl(char * msg) {
  resetiv();
  int msgLen = strlen(msg);
  char decrypted[msgLen] = {0}; // half may be enough
  aesLib.decrypt64(msg, msgLen, (byte*)decrypted, aes_key, sizeof(aes_key), aes_iv);
  return String(decrypted);
}

// Generate IV (once)
void aes_init() {

  Serial.flush();
  
  delay(1000);

  Serial.println("\n=======\n");

  ///
  
  Serial.println("\n1) AES init... paddingMode::ZeroLength");  
  test(paddingMode::ZeroLength);

  Serial.println("\n2) AES init... paddingMode::CMS");  
  test(paddingMode::CMS);

  Serial.println("\n3) AES init... paddingMode::Bit");  
  test(paddingMode::Bit);

  Serial.println("\n4) AES init... paddingMode::Null");  
  test(paddingMode::Null);

  Serial.println("\n5) AES init... paddingMode::Space");  
  test(paddingMode::Space);

  Serial.println("\n6) AES init... paddingMode::Random");  
  test(paddingMode::Random);

  Serial.println("\n7) AES init... paddingMode::Array");  
  test(paddingMode::Array);


  /*aesLib.set_paddingmode(paddingMode::ZeroLength);  

  byte enc_iv_A[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  Serial.println("Encrypting \"Looks like key but it's not me.\" using null-IV with ZeroLength padding");
  String encrypted1 = encrypt_impl((char*)plaintext, enc_iv_A);
  Serial.print("Encrypted(1): "); Serial.println(encrypted1);
  print_key_iv();

  aesLib.set_paddingmode(paddingMode::ZeroLength);  

  byte dec_iv_B[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  Serial.println("Decrypting \"Looks like key but it's not me.\" using null-IV ZeroLength padding");
  String decrypted = decrypt_impl((char*)encrypted1.c_str(), dec_iv_B); // aes_iv fails here, incorrectly decoded...
  Serial.print("Cleartext: ");
  Serial.println(decrypted);
  Serial.println("In first iteration this should work (using untouched dec_iv_B) ^^^");

  ///
  
  Serial.println("\n2) AES init... paddingMode::CMS");
  aesLib.set_paddingmode(paddingMode::CMS);  

  Serial.println("Encrypting \"Looks like key but it's not me.\" using null-IV with CMS padding");
  byte enc_iv_X[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  String encrypted2 = encrypt_impl((char*)plaintext, enc_iv_X );
  Serial.print("Encrypted (2): "); Serial.println(encrypted2);
  print_key_iv();

  aesLib.set_paddingmode(paddingMode::CMS);

  Serial.println("Decrypting \"Looks like key but it's not me.\" using null-IV CMS padding");
  byte enc_iv_Y[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  decrypted = decrypt_impl((char*)encrypted2.c_str(), enc_iv_Y);
  Serial.print("Cleartext: ");
  Serial.println(decrypted);
  
  ///

  Serial.println("\n2) AES init... paddingMode::Null");
  aesLib.set_paddingmode(paddingMode::Null);  

  Serial.println("Encrypting \"Looks like key but it's not me.\" using null-IV with Null padding");
  byte enc_iv_X3[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  String encrypted3 = encrypt_impl((char*)plaintext, enc_iv_X3 );
  Serial.print("Encrypted (3): "); Serial.println(encrypted3);
  print_key_iv();

  aesLib.set_paddingmode(paddingMode::Null);

  Serial.println("Decrypting \"Looks like key but it's not me.\" using null-IV Null padding");
  byte enc_iv_Y3[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  decrypted = decrypt_impl((char*)encrypted3.c_str(), enc_iv_Y3);
  Serial.print("Cleartext: ");
  Serial.println(decrypted);


  Serial.println("\n3) AES init... from Server, paddingMode::CMS");
  aesLib.set_paddingmode(paddingMode::CMS);  

  Serial.print("B64 Ciphertext: "); Serial.println(server_b64msg);  

  String decrypted_string = decrypt_impl((char*)server_b64msg.c_str(), aes_iv);

  Serial.print("Server message decrypted using server IV and CMS, cleartext: ");
  Serial.println(decrypted_string);*/
}

void test(paddingMode pm){
  
  aesLib.set_paddingmode(pm);  


  Serial.println("Encrypting \"Looks like key but it's not me.\" using null-IV with ZeroLength padding");
  String encrypted1 = encrypt_impl((char*)plaintext);
  Serial.print("Encrypted(1): "); Serial.println(encrypted1);
  print_key_iv();

  aesLib.set_paddingmode(pm);  

  Serial.println("Decrypting \"Looks like key but it's not me.\" using null-IV ZeroLength padding");
  String decrypted = decrypt_impl((char*)encrypted1.c_str()); // aes_iv fails here, incorrectly decoded...
  Serial.print("Cleartext: ");
  Serial.println(decrypted);
  Serial.println("In first iteration this should work (using untouched dec_iv_B) ^^^");
}

void setup() {
  Serial.begin(9600);
  aes_init();
}

void loop() {


}
