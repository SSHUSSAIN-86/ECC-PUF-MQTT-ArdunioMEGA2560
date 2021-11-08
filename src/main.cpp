#include <Arduino.h>
#include <SoftwareSerial.h>
#include <uECC.h>

#define SIZE_OF_KEY 64
#define BLOCK_SIZE 200
#define START_ 0x0
#define FLASH_ 0x0

unsigned char puf_mem[5120];
int temp_buffer [BLOCK_SIZE];
unsigned char key_temp [SIZE_OF_KEY];
unsigned char key_temp_generated [SIZE_OF_KEY] ;

int code (int *in_int)
{
  int bool_value = 0;
  int return_code = 0;
  
  bool_value = *in_int & (1UL<<0);
  
  if (bool_value)
    return_code =~(*in_int) ;
  else
    return_code = (* in_int) ;
  return return_code ;
}

int de_code ( int *code, int *in_int)
{
  int ones_count = 0;
  int zeros_count = 0;
  int exor;
  int i ;
  
  exor = *code ^ *in_int;
  for ( i =0; i <31; i++)
  {
    if (exor & (1UL<<i ))
        ones_count++;
    else
        zeros_count++;
  }
  if (ones_count > zeros_count)
    return 1 ;
  else
  return 0 ;
}

void key ( unsigned char *keyarray,
int bool_value, int posit )
{
  int pos1 ;
  int pos2 ;
  pos1 = posit / 8;
  pos2 = posit % 8;
  if ( bool_value )
      keyarray [pos1] = keyarray [pos1] |
                (unsigned char)(1<<pos2);
}

void initital_phase(void)
{
  // 32k = 8000 hex ; 64 = 40 Hex ; 80 hex Runs
  int i , j;
  int temp;
  int bool_value_temp ;
  int position_temp = 0;
  
  for ( i=0x0 ; i < SIZE_OF_KEY; i++) // reset the key
    key_temp [i] = 0;
  for ( i=0x0 ; i< SIZE_OF_KEY/8 ; i++)
  {
    for ( j=0; j< BLOCK_SIZE/4; j++)
    {
      temp = *((int *) (START_ + i *
      BLOCK_SIZE + j *0x4));
      temp_buffer[j] = code(&temp) ;
      position_temp = i * BLOCK_SIZE/4 + j ;
      bool_value_temp = (char) (temp & (1UL<<0));
    key (key_temp , bool_value_temp , position_temp);
  }
    if (i == 0)
      temp_buffer[0] = temp_buffer[0] | (1UL<<0);
    
  }
}

void key_generation_phase (void)
{
  int i ;
  int bool_value_temp ;
  for ( i=0 ; i<SIZE_OF_KEY*8; i++)
  {
    bool_value_temp = de_code ((int *)
    (FLASH_+ i*4 ), (int *) (START_ + i * 4));
    key ( key_temp_generated, bool_value_temp ,i) ;
  }
}

void generate_key (void)
{
  if (* (int *) (FLASH_) & ( 1UL<<0))
    initital_phase();
  else
     key_generation_phase();
}


extern "C" {

int RNG(uint8_t *terminal, unsigned scope) {
  // Capture from analogRead(0)
  while (scope) {
    uint8_t value = 0;
    for (unsigned i = 0; i < 8; ++i) {
      int initial = analogRead(0);
      int count = 0;
      while (analogRead(0) == initial) {
        ++count;
      }
      // Least significant bit
      if (count == 0) {
         value = (value << 1) | (initial & 0x01);
      } else {
         value = (value << 1) | (count & 0x01);
      }
    }
    *terminal = value;
    ++terminal;
    --scope;
  }
  // NB: can hash the resulting random data using SHA-256 or similar.
  return 1;
}

}  // extern "C"

int id=50;


SoftwareSerial softserial(2, 3); // Receive at port 2:RX, Transmit at port 3:TX

void setup() {
 Serial.begin(115200);
 Serial.print("Begin testing ECC\n");
 Serial.println("Interface Arduino with NodeMCU ESP8266");
 uECC_set_rng(&RNG); // call uEE_set_rng -> This returns the function to generate random bytes
 softserial.begin(115200);
}

void loop() {
    const struct uECC_Curve_t *eccCurve = uECC_secp256r1();// uECC_secp160r1(); //uECC_secp256k1()
  /*//uint8_t privateKey1[21];
  //uint8_t privateKey2[21];
  
  //uint8_t publicKey1[40];
  //uint8_t publicKey2[40];
  
  //uint8_t secretGen1[20];
  //uint8_t secretGen2[20];*/ // UnComment this for uECC_secp160r1();

    uint8_t privateKey1[32];
    uint8_t privateKey2[32];
  
    uint8_t publicKey1[64];
    uint8_t publicKey2[64];
  
    uint8_t secretGen1[32];
    uint8_t secretGen2[32];
  
    unsigned long a = millis();
    uECC_make_key(publicKey1, privateKey1, eccCurve);
    unsigned long b = millis();
  
    Serial.print("Generated key 1 in "); Serial.println(b-a);
    a = millis();
    uECC_make_key(publicKey2, privateKey2, eccCurve);
    b = millis();
    Serial.print("Generated key 2 in "); Serial.println(b-a);

    a = millis();
    int r = uECC_shared_secret(publicKey2, privateKey1, secretGen1, eccCurve);
    b = millis();
    Serial.print("Shared secret for 1 in "); Serial.println(b-a);
    if (!r) {
    Serial.print("shared_secret() failed (1)\n");
    return;
    }

    a = millis();
    r = uECC_shared_secret(publicKey1, privateKey2, secretGen2, eccCurve);
    b = millis();
    Serial.print("Shared secret for 2 in "); Serial.println(b-a);
    if (!r) {
    Serial.print("shared_secret() failed (2)\n");
    return;
    }
    
    if (memcmp(secretGen1, secretGen2, 20) != 0) {
    Serial.print("Shared secrets are not equal!\n");
    } else {
    Serial.print("Shared secrets are equal\n");
    }

    Serial.println("Sending data to NodeMCU");
    int adc=analogRead(A0);
    Serial.print("{\"SensorID\":");
    Serial.print(id);//sensor id
    Serial.print(",");
    Serial.print("\"Value\":");
    Serial.print(adc);//offset
    Serial.print("}");
    Serial.println();
 
    softserial.print("{\"SensorID\":");
    softserial.print(id);//sensor id
    softserial.print(",");
    softserial.print("\"Value\":");
    softserial.print(adc);//offset
    softserial.print("}");
    softserial.println();
    delay(2500);
}