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
        User userManager;
        crow::SimpleApp app;

        CROW_ROUTE(app, "/add_user")
                .methods(crow::HTTPMethod::POST)
                ([&conn](const crow::request &req) {
                    std::cerr << "Request body: " << req.body << std::endl;

                    auto json = crow::json::load(req.body);
                    if (!json || !json.has("name") || !json.has("surname") || !json.has("email") || !json.has(
                            "phoneNumber")) {
                        return crow::response{400, "Invalid parameters"};
                    }

                    std::string name = json["name"].s();
                    std::string surname = json["surname"].s();
                    std::string email = json["email"].s();
                    std::string phoneNumber = json["phoneNumber"].s();

                    User user{name, surname, email, phoneNumber};

                    try {
                        user.addUser(conn);
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
                        User user{"", "", "", phoneNumber};

                        try {
                            user.removeUser(conn);
                            return crow::response{200, "User successfully removed"};
                        } catch (const std::exception &e) {
                            return crow::response{500, "Database error: " + std::string(e.what())};
                        }
                    }
                    return crow::response{400, "Invalid parameters"};
                });

        CROW_ROUTE(app, "/list_users")
                .methods(crow::HTTPMethod::GET)
                ([&conn]() {
                    User user;
                    auto users = user.listUsers(conn);

                    crow::json::wvalue users_json;
                    users_json["users"] = crow::json::wvalue::list();

                    for (const auto &u: users) {
                        crow::json::wvalue user_json;
                        user_json["name"] = u.name;
                        user_json["surname"] = u.surname;
                        user_json["email"] = u.email;
                        user_json["phoneNumber"] = u.phoneNumber;
                    }

                    return crow::response{users_json};
                });

        app.port(18081).multithreaded().run();
    } catch (const std::exception &e) {
        std::cerr << RED << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}
