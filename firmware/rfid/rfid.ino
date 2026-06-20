// Hash variables
#define HASH_SIZE 8
#define ARRAY_SIZE 4
//Protoypes:

int hashing(byte key1[4]);

void setup() {
   
  Serial.begin(9600);
  byte key1[4]= {0xC1, 0x2F, 0xD6, 0x0E};
  Serial.println(hashing(key1));
}

void loop() {
  
}

// Hashing function 
int hashing(byte key1[4])
{
  int acumulado = 0;
  unsigned int hashcode = 0;
  for (int i = 0; i <  ARRAY_SIZE; i++)
  {
    acumulado += key1[i] * (i+1);
  }

  hashcode = acumulado % HASH_SIZE;
  return hashcode;
}