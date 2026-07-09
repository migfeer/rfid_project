// Hash variables
#define HASH_SIZE 8
#define ARRAY_SIZE 4

//Protoypes:
int hashing(byte key1[4]);

//Users node structure
typedef struct user
{
  byte UID[4];
  char username[10];
  char access_level[10];
  struct user *next;
} user;

user users[5];
user *hashtable[HASH_SIZE];


void setup() 
{
  Serial.begin(9600);
  //byte key1[4]= {0xC1, 0x2F, 0xD6, 0x0E};

  //Register users:
  // TODO: Crear variables temporales para UID's
  memcpy(users[0].UID, {0xC1, 0x2F, 0xD6, 0x2E}, 4);
  memcpy(users[0].UID, {0xAF, 0x4A, 0xD2, 0xA1}, 4);
  memcpy(users[0].UID, {0x34, 0x6E, 0x6A, 0xB3}, 4);
  memcpy(users[0].UID, {0x45, 0x1F, 0x5D, 0x3A}, 4);

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
    user *u = malloc(sizeof(user));

    memcpy((*u).UID, users[i].UID, 4);
    strcpy((*u).username, users[i].username);
    u->next = NULL;

    //TODO: Add prepeding and avoid memory leak
    

  }

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