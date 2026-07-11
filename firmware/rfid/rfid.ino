// Hash variables
#define HASH_SIZE 8
#define ARRAY_SIZE 4
#include <stdio.h>  
#include <stdlib.h>

//Protoypes:
unsigned int hashing(byte key[ARRAY_SIZE]);
bool search(byte UID[ARRAY_SIZE]);

//Users node structure
typedef struct userInfo
{
  byte UID[ARRAY_SIZE];
  char username[10];
  char access_level[10];
  struct userInfo *next;
} userInfo;

userInfo users[5];
userInfo *hashtable[HASH_SIZE];


//Variables
bool acceso;

void setup() 
{
  Serial.begin(9600);
  //byte key1[4]= {0xC1, 0x2F, 0xD6, 0x0E};

  //Register users:
  // TODO: Crear variables temporales para UID's
  byte key1[ARRAY_SIZE] = {0xC1, 0x2F, 0xD6, 0x2E};
  byte key2[ARRAY_SIZE] = {0xAF, 0x4A, 0xD2, 0xA1};
  byte key3[ARRAY_SIZE] = {0x34, 0x6E, 0x6A, 0xB3};
  byte key4[ARRAY_SIZE] = {0x45, 0x1F, 0x5D, 0x3A};

  memcpy(users[0].UID, key1, 4);
  memcpy(users[1].UID, key2, 4);
  memcpy(users[2].UID, key3, 4);
  memcpy(users[3].UID, key4, 4);

  strcpy(users[0].username, "Fer");
  strcpy(users[1].username, "Jesus");
  strcpy(users[2].username, "Karina");
  strcpy(users[3].username, "Pedro");

  int size = sizeof(users) / sizeof(users[0]);

  for (int i = 0; i < size; i++)
  {
    int hash;
    hash = hashing(users[i].UID);

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
    } else {
        u->next = hashtable[hash];
        hashtable[hash] = u;
    }
  }
}

void loop() {
  
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
bool search(byte UID[ARRAY_SIZE])
{
  // Hashing 
  int hash = hashing(UID);
  userInfo *ptr;
  ptr = hashtable[hash];

  // Search in hash table
  while (ptr != NULL)
  {
    if ( (ptr->UID[0] == UID[0]) && (ptr->UID[1] == UID[1]) && (ptr->UID[2] == UID[2]) && (ptr->UID[4] == UID[4]) )
    {
      return true;
    }
    ptr = ptr->next;
  }
  return false;
}