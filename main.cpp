#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <crow.h>
#include "colors.hpp"
#include "user.hpp"
#include <pqxx/pqxx>

//response gönderen yere bak
//user objesi yapıcam ve vector ile userlist yapıcam


int main() {
    try {
        pqxx::connection conn("dbname=direction user=Ömer password=Ou131973 host=127.0.0.1 port=5432");

        if (!conn.is_open()) {
            std::cerr << RED << "Can't open database" << RESET << std::endl;
            return 1;
        }

        crow::SimpleApp app;

        CROW_ROUTE(app, "/add_user")
                .methods(crow::HTTPMethod::POST)
                ([&conn](const crow::request &req) {
                    std::cerr << "Request body: " << req.body << std::endl;

                    auto json = crow::json::load(req.body);
                    if (!json || !json.has("name") || !json.has("surname") || !json.has("email") || !json.has(
                            "phoneNumber")) {
                    }

                    auto name = json["name"].s();
                    auto surname = json["surname"].s();
                    auto email = json["email"].s();
                    auto phoneNumber = json["phoneNumber"].s();

                    try {
                        addUser(conn, name, surname, email, phoneNumber);
                        return crow::response{200, "User added successfully"};
                    } catch (const std::exception &e) {
                        return crow::response{500, "Database error: " + std::string(e.what())};
                    }
                });


        CROW_ROUTE(app, "/remove_user")
                .methods(crow::HTTPMethod::POST)
                ([&conn](const crow::request &req) {
                    auto phoneNumber = req.url_params.get("phoneNumber");

                    if (phoneNumber) {
                        removeUser(conn, phoneNumber);
                        return crow::response{200, "User successfully removed"};
                    }
                    return crow::response{400, "invalid parameters"};
                });

        CROW_ROUTE(app, "/list_users")
                .methods(crow::HTTPMethod::GET)
                ([&conn]() {
                    auto users = listUsers(conn);
                    return crow::response{users};
                });

        app.port(18081).multithreaded().run();
    } catch (const std::exception &e) {
        std::cerr << RED << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}
