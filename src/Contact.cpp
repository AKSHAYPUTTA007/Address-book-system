#include "../include/Contact.h"

#include <iostream>
#include <iomanip>
#include <string>

// ============================================================
// Contact Class Implementation
// ============================================================

// ---- Constructors ----

Contact::Contact()
    : name(""), phone(""), email(""), address(""), category("") {}

Contact::Contact(const std::string& n,
                 const std::string& p,
                 const std::string& em,
                 const std::string& addr,
                 const std::string& cat)
    : name(n), phone(p), email(em), address(addr), category(cat) {}

// ---- Getters ----

std::string Contact::getName()     const { return name;     }
std::string Contact::getPhone()    const { return phone;    }
std::string Contact::getEmail()    const { return email;    }
std::string Contact::getAddress()  const { return address;  }
std::string Contact::getCategory() const { return category; }

// ---- Setters ----

void Contact::setName(const std::string& n)      { name     = n;   }
void Contact::setPhone(const std::string& p)     { phone    = p;   }
void Contact::setEmail(const std::string& em)    { email    = em;  }
void Contact::setAddress(const std::string& addr){ address  = addr;}
void Contact::setCategory(const std::string& cat){ category = cat; }

// ---- Display Methods ----

// Prints a detailed single-contact card
void Contact::display() const {
    std::string line(40, '-');
    std::cout << "\n" << line << "\n";
    std::cout << "  Name     : " << name     << "\n";
    std::cout << "  Phone    : " << phone    << "\n";
    std::cout << "  Email    : " << email    << "\n";
    std::cout << "  Address  : " << address  << "\n";
    std::cout << "  Category : " << category << "\n";
    std::cout << line << "\n";
}

// Prints one row in the contacts table
void Contact::displayRow(int serial) const {
    std::cout << "| "
              << std::left
              << std::setw(4)  << serial
              << "| "
              << std::setw(20) << name.substr(0, 19)
              << "| "
              << std::setw(14) << phone.substr(0, 13)
              << "| "
              << std::setw(26) << email.substr(0, 25)
              << "| "
              << std::setw(12) << category.substr(0, 11)
              << "| "
              << std::setw(15) << address.substr(0, 14)
              << "|\n";
}
