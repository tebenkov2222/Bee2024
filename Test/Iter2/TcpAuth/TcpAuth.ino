#include <AESLib.h>

AESLib aesLib;

const char* in = "WbB29L80-8CWA7";
const char * incoming_1 = "vkidC/kUru3O3Uo0ehbX23AYOYHlnstvAY8IQ1Gc/XM=";
const char * incoming = "WbbHq4NbNMQZLBvtDaVNBcJ1cy0izDaetxUkb8XZbjU=";

void setup() {
  Serial.begin(9600);

  initAes();

  print_key_iv();

  Serial.println();

  String en = encrypt((char *) in);

  Serial.print("in: ");
  Serial.println((char*)in);

  Serial.print("en: ");
  Serial.println(en);

  testIncoming(paddingMode::ZeroLength);
  testIncoming(paddingMode::CMS);
  testIncoming(paddingMode::Bit);
  testIncoming(paddingMode::Null);
  testIncoming(paddingMode::Space);
  testIncoming(paddingMode::Random);
  testIncoming(paddingMode::Array);
/*
  String result = decoded;
  result += ":";
  result += imai;

  Serial.print("result: ");
  Serial.println(result);

  String resultCoded = encrypt(result);

  Serial.print("resultCoded: ");
  Serial.println(resultCoded);*/
}

void testIncoming(paddingMode pm){
  aesLib.set_paddingmode(pm);

  Serial.println("---");

  String decoded = decrypt((char *)incoming);

  Serial.print("incoming: ");
  Serial.println(incoming);

  Serial.print("decoded: ");
  Serial.println(decoded);
}

void loop() {            
}
