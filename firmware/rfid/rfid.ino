#include <SPI.h>
#include <MFRC522.h>

// Hash variables
#define HASH_SIZE 8
#define ARRAY_SIZE 4
#include <stdio.h>  
#include <stdlib.h>

#define RST_PIN  9			// constante para referenciar pin de reset
#define SS_PIN  10			// constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);	// crea objeto mfrc522 enviando pines de slave select y reset
byte LecturaUID[4]; 				// crea array para almacenar el UID leido

//Protoypes:
unsigned int hashing(byte key[ARRAY_SIZE]);
void search(byte lectura[ARRAY_SIZE]);

//Users node structure
typedef struct userInfo
{
  byte UID[ARRAY_SIZE];
  char username[10];
  char access_level[10];
  struct userInfo *next;
} userInfo;

userInfo users[4];
userInfo *hashtable[HASH_SIZE];


//Variables
bool acceso;

void setup() 
{
  Serial.begin(9600);
  SPI.begin();				// inicializa bus SPI
  mfrc522.PCD_Init();			// inicializa modulo lector

  //Register users:
  // TODO: Crear variables temporales para UID's
  byte key1[ARRAY_SIZE] = {0xC1, 0x2F, 0xD6, 0x2E};
  byte key2[ARRAY_SIZE] = {0xAF, 0x4A, 0xD2, 0xA1};
  byte key3[ARRAY_SIZE] = {0x79, 0xC7, 0x00, 0x7F}; // correct user
  byte key4[ARRAY_SIZE] = {0x79, 0xC7, 0x00, 0x6F};

  memcpy(users[0].UID, key1, 4);
  memcpy(users[1].UID, key2, 4);
  memcpy(users[2].UID, key3, 4);
  memcpy(users[3].UID, key4, 4);

  strcpy(users[0].username, "Pedro");
  strcpy(users[1].username, "Jesus");
  strcpy(users[2].username, "Karina");
  strcpy(users[3].username, "Sr. Stark");

  int size = sizeof(users) / sizeof(users[0]);
  for (int i = 0; i < size; i++)
  {
    int hash;
    hash = hashing(users[i].UID);
    //Serial.print("User: ");
    //Serial.print(i+1);
    //Serial.print("  | Hash: ");
    //Serial.println(hash);
    //Create a node
    userInfo *u = (userInfo *)malloc(sizeof(userInfo)); //Type casting - malloc retorna un generic pointer
    if (u == NULL){
      Serial.println("Can't assing memory!"); 
      return;
    }

    memcpy((*u).UID, users[i].UID, 4);
    strcpy((*u).username, users[i].username);
    u->next = NULL;

    //TODO: Add prepeding and avoid memory leak
    if (hashtable[hash] == NULL){
        hashtable[hash] = u;
        Serial.print("New user slot: "); 
        Serial.println(hash); 
    } else {
        u->next = hashtable[hash];
        hashtable[hash] = u;
        Serial.print("New user slot: "); 
        Serial.println(hash); 
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
  search(LecturaUID);

  mfrc522.PICC_HaltA();  		// detiene comunicacion con tarjeta
  Serial.println("Swipe car");
  /*
    acceso = search(UID);
    if (acceso){
      Serial.println("")
    }
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
void search(byte lectura[ARRAY_SIZE])
{
  // Hashing 
  int hash = hashing(lectura);
  userInfo *ptr;
  ptr = hashtable[hash];

  // Search in hash table
  while (ptr != NULL)
  {
    for (int i = 0; i < mfrc522.uid.size; i++)
    {
      if(lectura[i] != ptr->UID[i])
      {
        break;
      }
      Serial.println("Acceso concedido");
      Serial.print("Bienvenido: ");
      Serial.println(ptr->username);
      return;
    }
    ptr = ptr->next;
  }
  Serial.println("Acceso denegado");
  return;
}