#include <SPI.h>
#include <MFRC522.h>

// Hash variables
#define HASH_SIZE 8
#define ARRAY_SIZE 4

#define RST_PIN  9			// pin de reset
#define SS_PIN  10			// pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);	// 
byte LecturaUID[4]; 				// UID read

//Protoypes:
unsigned int hashing(byte key[ARRAY_SIZE]);
bool search(byte lectura[ARRAY_SIZE]);

//Users node structure
typedef struct userInfo
{
  byte UID[ARRAY_SIZE];
  String username;
  char access_level[10];
  struct userInfo *next;
} userInfo;

userInfo users[4];
userInfo *hashtable[HASH_SIZE];


void setup() 
{
  Serial.begin(9600);
  SPI.begin();				// inicializa bus SPI
  mfrc522.PCD_Init();			// inicializa modulo lector

  //Register users:
  // TODO: Crear variables temporales para UID's
  const byte key1[ARRAY_SIZE] = {0xC1, 0x2F, 0xD6, 0x2E};
  const byte key2[ARRAY_SIZE] = {0xAF, 0x4A, 0xD2, 0xA1};
  const byte key3[ARRAY_SIZE] = {0x79, 0xC7, 0x00, 0x7F}; // correct user
  const byte key4[ARRAY_SIZE] = {0x79, 0xC7, 0x00, 0x6F};

  memcpy(users[0].UID, key1, 4);
  memcpy(users[1].UID, key2, 4);
  memcpy(users[2].UID, key3, 4);
  memcpy(users[3].UID, key4, 4);

  users[0].username = "Pedro";
  users[1].username = "Jesus";
  users[2].username = "Karina";
  users[3].username = "Sr. Stark";

  int size = sizeof(users) / sizeof(users[0]);
  for (int i = 0; i < size; i++)
  {
    int hash;
    hash = hashing(users[i].UID);

    //Create a node
    userInfo *u = (userInfo *)malloc(sizeof(userInfo)); //Type casting - malloc retorna un generic pointer
    if (u == NULL){
      Serial.println("Can't assing memory!");
      free(u);
      return;
    }

    memcpy((*u).UID, users[i].UID, 4);
    (*u).username = users[i].username;
    u->next = NULL;

    if (hashtable[hash] == NULL){
        hashtable[hash] = u;
    } else {
        u->next = hashtable[hash];
        hashtable[hash] = u;
    }
  }

}

void loop() {
  
  if ( ! mfrc522.PICC_IsNewCardPresent())		// si no hay una tarjeta presente
    return;						// retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 		// si no puede obtener datos de la tarjeta
    return;						// retorna al loop esperando por otra tarjeta

  Serial.print("UID:");				// muestra texto UID:
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {	
    // bucle recorre de a un byte por vez el UID
    if (mfrc522.uid.uidByte[i] < 0x10)
    {		// si el byte leido es menor a 0x10
      Serial.print(" 0");				// imprime espacio en blanco y numero cero
    }
    else{						// sino
      Serial.print(" ");				// imprime un espacio en blanco
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX);   	// imprime el byte del UID leido en hexadecimal
    LecturaUID[i]=mfrc522.uid.uidByte[i];   	// almacena en array el byte del UID leido      
  }

  Serial.print("\t");   			// imprime un espacio de tabulacion 
  

  bool search_state = search(LecturaUID);
  if (search_state)
  {    
      Serial.println("Acceso concedido");
      Serial.print("Bienvenido: ");

      //Bucket index
      //Serial.print("Found on: ");
      //Serial.println(hash);
  }

  mfrc522.PICC_HaltA();  		// detiene comunicacion con tarjeta
  Serial.println("Swipe car");



  /*
  Free all the memory
  for (int j = 0; j < HASH_SIZE; j++)
  {
    freeMemory(hashtable[j]);
  }
  hashtable[i] = NULL
  */
}


// Hashing function 
unsigned int hashing(byte key[ARRAY_SIZE])
{
  int acumulado = 0;
  unsigned int hashcode = 0;
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    acumulado += key[i] * (i+1);
  }

  hashcode = acumulado % HASH_SIZE;
  return hashcode;
}


//Search function
bool search(byte lectura[ARRAY_SIZE])
{
  // Hashing 
  int hash = hashing(lectura);
  userInfo *ptr;
  ptr = hashtable[hash];
  bool flag = false;

  // Search in hash table
  while (ptr != NULL)
  {
    int size_UID = sizeof(ptr->UID);
    if (ARRAY_SIZE == size_UID)
    {
      if (memcmp(lectura, ptr->UID, sizeof(ptr->UID)) == 0) //compare bytes
      {
        flag = true;
        return flag;
      }
    }
    else {
      Serial.println("Not the same size");
      return flag;
    }
    ptr = ptr->next;
  }
  return flag; //false
}

void countUsers() 
{
  userInfo *p;
  int users;

  for (int i = 0; i < HASH_SIZE; i++){
    users = 0;
    p = hashtable[i];
    while (p != NULL){
      users++;
      p= p->next;
    }
    Serial.print("Bucket: ");
    Serial.print(i);
    Serial.print("\t"); 
    Serial.print("Users:");
    Serial.println(users);
  }
  return;
}
