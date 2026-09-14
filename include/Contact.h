#ifndef CONTACT_H
#define CONTACT_H

#include <string>

// ============================================================
// Contact Class
// Represents a single contact with all personal details.
// Demonstrates: Class, Object, Abstraction, Constructor,
//               Member Functions (Getters & Setters)
// ============================================================
class Contact {
private:
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    std::string category;

public:
    // Default constructor
    Contact();

    // Parameterized constructor
    Contact(const std::string& name,
            const std::string& phone,
            const std::string& email,
            const std::string& address,
            const std::string& category);

    // ---- Getters ----
    std::string getName()     const;
    std::string getPhone()    const;
    std::string getEmail()    const;
    std::string getAddress()  const;
    std::string getCategory() const;

    // ---- Setters ----
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setAddress(const std::string& address);
    void setCategory(const std::string& category);

    // ---- Display Methods ----
    // Prints detailed single-contact view
    void display() const;

    // Prints one row in a table (used by AddressBook)
    void displayRow(int serial) const;
};

#endif // CONTACT_H
