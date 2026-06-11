# Project: RFID Access System


## Background

When you walk into an office and swipe your card through a reader, the system decides in less than a millisecond whether you have access or not. It doesn't loop through a list of employees one by one. It doesn't do a binary search in a tree. It does something more direct: it takes your card's ID, applies a mathematical function, and jumps straight to the memory position where your record should be. If you're there, the door opens. If not, it doesn't.

That mathematical function is called a **hash function**. And the structure where the records live is called a **hash table**.

A hash table is an array where each position is called a **bucket**. The hash function converts a key — in this case the RFID card's UID — into a numeric index that points directly to the correct bucket. Insertion, search, and deletion are O(1) on average. Not O(log n), not O(n). O(1). Direct.

But there's a problem. Two different UIDs can produce the same index. That's called a **collision**. And collisions are inevitable — they're not a design flaw, they're a mathematical consequence. The question isn't how to avoid them, but how to handle them.

The most common solution is called **chaining**: each bucket doesn't store a single record, it stores a linked list. If two UIDs collide in the same bucket, both records coexist in that list. The search first jumps to the correct bucket via hash, then walks the short list until it finds the exact UID.

That's exactly what you'll build. A real access control system, with a hash table, chaining, and physical hardware. When you swipe a card, the system hashes it, jumps to the bucket, walks the chain, and in microseconds shows on the LCD whether access was granted or denied.

You're not building a security project. You're building a hash table and watching it work in real time with hardware.

---

## Learning Goals

By the end you will be able to:

- Implement a hash function from scratch and understand its properties
- Build a hash table with chaining using linked lists
- Explain why collisions are inevitable (Pigeonhole Principle)
- Understand the difference between O(1) average and O(n) worst case
- Insert, search, and delete records in a hash table with pointers
- Connect an abstract data structure to a physical access system

---

## Hardware Required

- Arduino Uno or Nano
- RFID RC522 module x1
- RFID cards or tags x3 (minimum)
- 16x2 LCD with I2C module x1
- Green LED x1 (access granted)
- Red LED x1 (access denied)
- Passive buzzer x1
- 220Ω resistors x2
- Breadboard + jumper wires

---

## Concepts Involved

| Concept | Where it appears |
|---|---|
| Hash function | Converts card UID into bucket index |
| Hash table | Main array of buckets |
| Chaining | Linked list in each bucket to handle collisions |
| Collision | Two different UIDs produce the same index |
| O(1) insertion | Registering a new card in the table |
| O(1) average search | Verifying access when swiping a card |
| Linked list | Internal structure of each bucket |

---

## Specification

**The system must:**

1. Have a hash table of exactly **8 buckets**, each one is the head of a linked list
2. Pre-register at least **5 users** with their UIDs at startup in `setup()`
3. When a card is presented, read its UID, hash it, jump to the correct bucket, and search the chain
4. If the UID is in the table: turn on the green LED, show the username and "ACCESS OK" on the LCD for 2 seconds
5. If the UID is not in the table: turn on the red LED, show "ACCESS DENIED" for 2 seconds
6. The buzzer emits a short high tone for access granted and a long low tone for denied
7. After each read, the LCD returns to the waiting state: "Swipe card"
8. Print to serial on each read: the UID read, the bucket index, the chain size in that bucket, and the result
9. The system must work correctly even when two UIDs collide in the same bucket
10. On startup, print to serial the complete state of the table: how many users are in each bucket

**The system must not:**

- Use linear search over all users without hashing first
- Use a single global linked list for all users
- Use external data structure libraries

---

## Implementation Details

**The node:**
Each node stores the card UID as a string, the username, the access level, and a pointer to the next node in the chain.

**The hash function:**
You receive the UID as a byte array. Your function must produce a number between 0 and 7 (the bucket index). The simplest function: sum all bytes of the UID and apply modulo 8. This is your starting point. Reflect on its weaknesses.

**The table:**
An array of 8 node pointers. At startup all point to NULL. Each `insert` adds a node to the front of the corresponding chain (prepending).

**Search:**
Hash the received UID → bucket index → walk the linked list in that bucket comparing UID byte by byte → found or not found.

**Intentional collision:**
The project requires that at least two of your 5 users collide in the same bucket. Verify this when designing your test UIDs or when choosing your hash function. If there's no collision, you're not testing chaining.

**Table size:**
8 buckets is intentionally small. With 5 users and 8 buckets, collisions are likely. In production you'd use 256 or more. Here you keep it small to watch collisions happen.

---

## Example Output (Serial Monitor)

```
=== RFID ACCESS SYSTEM STARTED ===
Hash table size: 8 buckets

[TABLE STATE]
Bucket 0: [] (empty)
Bucket 1: [Carlos] (1 node)
Bucket 2: [Maria -> Pedro] (2 nodes) ← COLLISION
Bucket 3: [] (empty)
Bucket 4: [Ana] (1 node)
Bucket 5: [] (empty)
Bucket 6: [Luis] (1 node)
Bucket 7: [] (empty)
Total users: 5 | Collisions: 1

---

[READ] UID: A3 4F 2B 1C
[HASH] A3+4F+2B+1C = 229 | 229 % 8 = 5... bucket not found
[SEARCH] Bucket 5: empty
[RESULT] ACCESS DENIED

[READ] UID: 12 3E 8A 0D
[HASH] 12+3E+8A+0D = 171 | 171 % 8 = 3... wait recalculating
[HASH] Bucket 2 | Chain size: 2
[SEARCH] Node 1: Maria? No. Node 2: Pedro? Yes.
[RESULT] ACCESS GRANTED - Pedro (Level: Admin)

[READ] UID: 45 1A 9C 3F
[HASH] Bucket 2 | Chain size: 2
[SEARCH] Node 1: Maria? Yes.
[RESULT] ACCESS GRANTED - Maria (Level: User)
```

---

## The Hard Question

When you finish, answer this before closing the project:

Your hash function is `sum_of_bytes % 8`. If someone knows your function, they can forge cards whose UIDs always collide in the same bucket. With enough collisions, the search in that bucket stops being O(1) and becomes O(n). This is called a **Hash DoS attack** and is a real vulnerability in production systems.

How would you solve it?

---

## How to think about it

Before writing a single line of code, draw your 8-bucket hash table on paper. Write 5 made-up UIDs. Apply your hash function by hand and draw which bucket each one falls into with arrows. Identify which ones collide and draw the chain. If you can't draw it, you can't program it.