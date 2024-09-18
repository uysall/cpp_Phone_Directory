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
    std::vector<User> userList;

    User() = default;

    User(std::string name, std::string surname, std::string email, std::string phoneNumber)
        : name(std::move(name)), surname(std::move(surname)), email(std::move(email)),
          phoneNumber(std::move(phoneNumber)) {}

    void addUser(pqxx::connection &conn) const {
        pqxx::nontransaction nt(conn);
        nt.exec_params(
            "INSERT INTO direction.direction_table(name, surname, email, phonenumber) VALUES ($1, $2, $3, $4);",
            name, surname, email, phoneNumber);
    }

    void removeUser(pqxx::connection &conn) const {
        pqxx::nontransaction nt(conn);
        nt.exec_params("DELETE FROM direction.direction_table WHERE phonenumber = $1;", phoneNumber);
    }

    std::vector<User> listUsers(pqxx::connection &conn) {
        pqxx::nontransaction nt(conn);
        const pqxx::result res = nt.exec("SELECT name, surname, email, phonenumber FROM direction.direction_table;");
        std::vector<User> users;
        for (const auto &row : res) {
            users.emplace_back(
                row["name"].as<std::string>(),
                row["surname"].as<std::string>(),
                row["email"].as<std::string>(),
                row["phonenumber"].as<std::string>()
            );
        }
        return users;
    }
};
#endif
