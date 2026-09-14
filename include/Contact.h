#ifndef CONTACT_H
#define CONTACT_H

#include <string>

// ============================================================
// Contact Class
// Represents a single contact with all personal details.
// OOP Concepts: Class, Object, Abstraction, Constructor,
//               Member Functions (Getters & Setters)
// ============================================================
class Contact {
private:
    std::string name;
    std::string phone;        // Stored with country code: "+91 9876543210"
    std::string email;
    std::string address;
    std::string category;
    bool        isFavourite;  // true = starred contact [*]
    std::string birthday;     // "DD/MM/YYYY" or "" if not set

public:
    // ---- Constructors ----
    Contact();

    // Full parameterized constructor
    Contact(const std::string& name,
            const std::string& phone,
            const std::string& email,
            const std::string& address,
            const std::string& category,
            bool isFavourite = false,
            const std::string& birthday = "");

    // ---- Getters ----
    std::string getName()      const;
    std::string getPhone()     const;
    std::string getEmail()     const;
    std::string getAddress()   const;
    std::string getCategory()  const;
    bool        getFavourite() const;
    std::string getBirthday()  const;

    // ---- Setters ----
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setAddress(const std::string& address);
    void setCategory(const std::string& category);
    void setFavourite(bool fav);
    void setBirthday(const std::string& birthday);

    // ---- Display ----
    void display()              const; // Detailed card view
    void displayRow(int serial) const; // One row in a table
};

#endif // CONTACT_H
