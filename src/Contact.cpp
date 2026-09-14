#include "../include/Contact.h"

#include <iostream>
#include <iomanip>
#include <string>

// ============================================================
// Contact Class Implementation
// ============================================================

// ---- Constructors ----

Contact::Contact()
    : name(""), phone(""), email(""), address(""), category(""),
      isFavourite(false), birthday("") {}

Contact::Contact(const std::string& n,
                 const std::string& p,
                 const std::string& em,
                 const std::string& addr,
                 const std::string& cat,
                 bool fav,
                 const std::string& bday)
    : name(n), phone(p), email(em), address(addr), category(cat),
      isFavourite(fav), birthday(bday) {}

// ---- Getters ----

std::string Contact::getName()      const { return name;        }
std::string Contact::getPhone()     const { return phone;       }
std::string Contact::getEmail()     const { return email;       }
std::string Contact::getAddress()   const { return address;     }
std::string Contact::getCategory()  const { return category;    }
bool        Contact::getFavourite() const { return isFavourite; }
std::string Contact::getBirthday()  const { return birthday;    }

// ---- Setters ----

void Contact::setName(const std::string& n)      { name        = n;   }
void Contact::setPhone(const std::string& p)     { phone       = p;   }
void Contact::setEmail(const std::string& em)    { email       = em;  }
void Contact::setAddress(const std::string& addr){ address     = addr;}
void Contact::setCategory(const std::string& cat){ category    = cat; }
void Contact::setFavourite(bool fav)             { isFavourite = fav; }
void Contact::setBirthday(const std::string& b)  { birthday    = b;   }

// ---- Display Methods ----

// Detailed single-contact card
void Contact::display() const {
    std::string line(44, '-');
    std::cout << "\n" << line << "\n";
    std::cout << "  Name      : " << name                              << "\n";
    std::cout << "  Phone     : " << phone                             << "\n";
    std::cout << "  Email     : " << email                             << "\n";
    std::cout << "  Address   : " << address                           << "\n";
    std::cout << "  Category  : " << category                          << "\n";
    std::cout << "  Favourite : " << (isFavourite ? "Yes [*]" : "No") << "\n";
    if (!birthday.empty())
        std::cout << "  Birthday  : " << birthday                      << "\n";
    std::cout << line << "\n";
}

// One row in the contacts table
// Name column prefixed with "[*]" for favourites
void Contact::displayRow(int serial) const {
    std::string displayName = (isFavourite ? "[*] " : "    ") + name;
    std::cout << "| "
              << std::left
              << std::setw(4)  << serial
              << "| "
              << std::setw(20) << displayName.substr(0, 19)
              << "| "
              << std::setw(17) << phone.substr(0, 16)
              << "| "
              << std::setw(26) << email.substr(0, 25)
              << "| "
              << std::setw(12) << category.substr(0, 11)
              << "| "
              << std::setw(15) << address.substr(0, 14)
              << "|\n";
}
