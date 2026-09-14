# Address Book System

A console-based **Address Book System** built in **C++** using **Object-Oriented Programming** concepts.  
Developed as a B.Tech 1st Year OOP project.

---

## Project Overview

| Field       | Details                         |
|-------------|----------------------------------|
| Subject     | C++ Object-Oriented Programming  |
| Type        | Console-Based Application        |
| Language    | C++ (Standard: C++17)            |
| Storage     | Text File (`data/contacts.txt`)  |

---

## OOP Concepts Used

| Concept           | Where Applied                              |
|-------------------|--------------------------------------------|
| **Class**         | `Contact`, `AddressBook`                   |
| **Object**        | `book` in `main.cpp`; each `Contact` item  |
| **Abstraction**   | Private members + public interface only    |
| **Constructor**   | Default & Parameterized in `Contact`       |
| **Member Functions** | Getters, Setters, display(), all features |

---

## Folder Structure

```
Address-book-system/
│
├── include/
│   ├── Contact.h          # Contact class declaration
│   └── AddressBook.h      # AddressBook class declaration
│
├── src/
│   ├── main.cpp           # Entry point — loads, menu loop, saves
│   ├── Contact.cpp        # Contact class implementation
│   └── AddressBook.cpp    # AddressBook class implementation (all features)
│
├── data/
│   └── contacts.txt       # Permanent storage (pipe-separated)
│
├── README.md
└── .gitignore
```

---

## How to Build & Run

### Using g++ directly

```bash
cd Address-book-system

g++ -std=c++17 -Wall \
    src/main.cpp src/Contact.cpp src/AddressBook.cpp \
    -o addressbook

./addressbook
```

> **Important:** Run the binary from the project root (`Address-book-system/`) so it can find `data/contacts.txt`.

---

## Storage Design

```
Program Start
    │
    ▼
Read data/contacts.txt
    │
    ▼
Load into vector<Contact>  (RAM)
    │
    ▼
[All operations work on RAM — Add, Display, Search, Update, Delete]
    │
    ▼
User selects Exit
    │
    ▼
Overwrite data/contacts.txt  (permanent save)
    │
    ▼
Program Ends
```

### File Format

Contacts are stored as **pipe-separated values**:

```
Akshay|9876543210|akshay@gmail.com|Kakinada|College
Ravi|9123456789|ravi@gmail.com|Vizag|Friends
```

---

## Features

| No | Feature            | Description                                            |
|----|--------------------|--------------------------------------------------------|
| 1  | Add Contact        | Add with duplicate phone validation                    |
| 2  | Display Contacts   | All / By Category / By Location / Category+Location    |
| 3  | Search Contacts    | Name (smart %), Phone, Email, Category, Location       |
| 4  | Update Contact     | Edit any field; validates duplicate phones             |
| 5  | Delete Contact     | Search → confirm → remove                             |
| 6  | Import Contacts    | Merge from backup file; skip duplicates & malformed    |
| 7  | Export Backup      | Write all contacts to any file path                    |
| 8  | Statistics         | Total, category breakdown, top locations               |
| 9  | Exit               | Saves everything to `contacts.txt` before quitting     |

---

## Contact Fields

Each contact stores:
- **Name**
- **Phone Number** *(must be unique)*
- **Email Address**
- **Physical Address / Location**
- **Category** — Family / Friends / College / Work / Other

---

## Smart Name Search

When searching by name, the system computes a similarity score between the query and every contact name. Results are ranked and shown with a match percentage, allowing the user to pick one for full details.

---

## Author

**Akshay** — B.Tech 1st Year OOP Project
