# Address Book System

A console-based **Address Book System** built in **C++** using **Object-Oriented Programming** concepts.  
Developed as a B.Tech 2nd Year OOP Project.

---

## Project Overview

| Field       | Details                          |
|-------------|----------------------------------|
| Subject     | C++ Object-Oriented Programming  |
| Type        | Console-Based Application        |
| Language    | C++                              |
| Storage     | Text File (`contacts.txt`)       |

---

## OOP Concepts Used

| Concept              | Where Applied                                      |
|----------------------|----------------------------------------------------|
| **Struct**           | `struct Contact` — holds one contact's data fields |
| **Class**            | `class AddressBook` — manages the contact list     |
| **Constructor**      | Loads contacts from file automatically on startup  |
| **Member Functions** | `addContact()`, `searchContact()`, `deleteContact()`, etc. |

---

## Project Structure

```
Address-book-system/
├── main.cpp        ← All code in one file
├── contacts.txt    ← Permanent storage (pipe-separated)
└── README.md
```

---

## How to Build & Run

### Linux / Mac

```bash
cd Address-book-system
g++ main.cpp -o addressbook
./addressbook
```

### Windows (MinGW)

```cmd
g++ main.cpp -o addressbook.exe
addressbook.exe
```

---

## Features

| Option | Feature          | Description                                        |
|--------|------------------|----------------------------------------------------|
| 1      | Add Contact      | Add a new contact (rejects duplicate phone numbers)|
| 2      | Display All      | Show all saved contacts with serial numbers        |
| 3      | Search Contact   | Search by name — partial and case-insensitive      |
| 4      | Update Contact   | Edit any field of an existing contact by phone     |
| 5      | Delete Contact   | Remove a contact by phone number (with confirmation)|
| 6      | Exit             | Save all contacts to file and quit                 |

---

## How It Works

- On **startup**: constructor reads `contacts.txt` and loads all contacts into a `vector<Contact>` in RAM
- During **runtime**: all operations work on the in-memory vector (fast)
- On **exit**: all contacts are written back to `contacts.txt` (permanent storage)

### File Format (`contacts.txt`)

Each line stores one contact, fields separated by `|`:

```
Name|Phone|Email|Address|Category
Akshay|9876543210|akshay@gmail.com|Kakinada|College
Ravi|9123456789|ravi@gmail.com|Vizag|Friends
```

---

## Contact Fields

Each contact stores:
- **Name**
- **Phone Number** *(must be unique — duplicates are rejected)*
- **Email Address**
- **Address / Location**
- **Category** *(Family / Friends / College / Work / Other)*

---

## Requirements

- A C++ compiler (g++ recommended)
- On Windows: [MinGW](https://www.mingw-w64.org/)
- On Linux/Mac: g++ is usually pre-installed

---

## Authors

**Akshay** — B.Tech 2nd Year OOP Project  
**SriDivya** — B.Tech 2nd Year OOP Project  
**Thanoj** — B.Tech 2nd Year OOP Project