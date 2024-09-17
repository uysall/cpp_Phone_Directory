#ifndef USER_HPP
#define USER_HPP
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <crow.h>
#include "colors.hpp"
#include "user.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <utility>

struct User {
    std::string name, surname, email, phoneNumber;

    User(std::string name, std::string surname, std::string email, std::string phoneNumber)
        : name(std::move(name)), surname(std::move(surname)), email(std::move(email)),
          phoneNumber(std::move(phoneNumber)) {
    }

    void addUser(pqxx::connection &conn, const std::string &name, const std::string &surname, const std::string &email,
                 const std::string &phoneNumber) {
        pqxx::nontransaction nt(conn);
        nt.exec_params(
            "INSERT INTO direction.direction_table(name, surname, email, phonenumber) VALUES ($1, $2, $3, $4);",
            name, surname, email, phoneNumber);
    }

    void removeUser(pqxx::connection &conn, const std::string &phoneNumber) {
        pqxx::nontransaction nt(conn);
        nt.exec_params("DELETE FROM direction.direction_table WHERE phonenumber = $1;", phoneNumber);
    }

    std::string listUsers(pqxx::connection &conn) {
        pqxx::nontransaction nt(conn);
        const pqxx::result res = nt.exec("SELECT name, surname, email, phonenumber FROM direction.direction_table;");
        std::string output;
        for (const auto &row: res) {
            output += "Name: " + row["name"].as<std::string>() + ", ";
            output += "Surname: " + row["surname"].as<std::string>() + ", ";
            output += "Email: " + row["email"].as<std::string>() + ", ";
            output += "Phone Number: " + row["phonenumber"].as<std::string>() + "\n";
        }
        return output;
    }
};
#endif
