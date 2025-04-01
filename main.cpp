#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#define _BSD_SOURCE 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>

using namespace std;

#define PORT 6391

mutex mainMuter;
atomic<int> cntThreads(1);

enum class Request{
    SELECT,
    INSERT,
    DELETE,
    END,
    UNKNOWN
};

vector<string> split(const string &to_split, char delimiter) {
    vector<string> splited;
    size_t start = 0;
    size_t end = to_split.find(delimiter);

    while (end != string::npos) {
        splited.push_back(to_split.substr(start, end - start));
        start = end + 1;
        end = to_split.find(delimiter, start);
    }

    splited.push_back(to_split.substr(start));
    return splited;
}

class DataAccess {
public:
    struct HotelOwner;
    struct Hotel;

    struct Application {
        unsigned int id;
        unsigned int sender_id;
        string title;
        string city;
        unsigned int price_per_night;
    };

    struct Hotel {
        unsigned int id;
        unsigned int owner;
        string title;
        string city;
        unsigned int price_per_night;
    };

    struct User {
        unsigned int id;
        string username;
        string password;
        string role;
    };

    struct RegularUser : User {
        vector<Hotel> bookmarks;
    };

    struct HotelOwner : User {
        vector<Hotel> my_hotels;
    };

    struct Admin : User {
        string access_level;
    };

    void addApplication(const Application& app) {
        applications.push_back(app);
    }

    void removeApplication(const unsigned int& app_id) {
        auto it = remove_if(applications.begin(), applications.end(), [app_id](const Application& app) {
            return app.id == app_id;
        });

        if (it != applications.end()) {
            applications.erase(it, applications.end());
        }
    }

    vector<Application> getApplications() const {
        return applications;
    }

    void addHotel(const Hotel& new_hotel) {
        hotels.push_back(new_hotel);
        HotelOwner curr_owner;
        for (auto& owner: hotel_owners) {
            if (owner.id == new_hotel.owner) {
                curr_owner = owner;
            }
        }
        curr_owner.my_hotels.push_back(new_hotel);
    }

    vector<Hotel> getHotels() const {
        return hotels;
    }

    void addRegularUser(const RegularUser& new_reg_user) {
        regular_users.push_back(new_reg_user);
    }

    vector<RegularUser> getRegularUsers() const {
        return regular_users;
    }

    void addHotelOwner(const HotelOwner& new_hot_own) {
        hotel_owners.push_back(new_hot_own);
    }

    vector<HotelOwner> getHotelOwners() const {
        return hotel_owners;
    }

    void addAdmin(const Admin& new_admin) {
        admins.push_back(new_admin);
    }

    vector<Admin> getAdmins() const {
        return admins;
    }

private:
    vector<Application> applications;
    vector<Hotel> hotels;
    vector<RegularUser> regular_users;
    vector<HotelOwner> hotel_owners = {{1, "asd", "asd", "владелец отеля"}};
    vector<Admin> admins = {{1, "admin", "admin", "администратор"}};
};

class BusinessLogic {
public:
    BusinessLogic(DataAccess& dataAccess) : dataAccess(dataAccess) {}

    DataAccess::Application loadApplication(const unsigned int& load_id) {
        for (auto& app : dataAccess.getApplications()) {
            if (app.id == load_id) {
                return app;
            }
        }
        return {};
    }

    void createApplication(const unsigned int& sender_id, const string& title, const string& city, const unsigned int& price) {
        DataAccess::Application app = {
            static_cast<unsigned int>(dataAccess.getApplications().size()) + 1,
            sender_id,
            title,
            city,
            price
        };
        dataAccess.addApplication(app);
    }

    void removeApplication(const unsigned int& app_id) {
        dataAccess.removeApplication(app_id);
    }

    void createHotel(const unsigned int& owner, const string& title, const string& city, const unsigned int& price) {
        DataAccess::Hotel hotel = {
            static_cast<unsigned int>(dataAccess.getHotels().size()) + 1,
            owner,
            title,
            city,
            price
        };
        dataAccess.addHotel(hotel);
    }

    void createUser(const string& username, const string& password, const string& status) {
        if (status == "обычный пользователь") {
            DataAccess::RegularUser reg_user = {
                static_cast<unsigned int>(dataAccess.getRegularUsers().size()) + 1,
                username,
                password,
                status
            };
            dataAccess.addRegularUser(reg_user);
        } else if (status == "владелец отеля") {
            DataAccess::HotelOwner hotel_owner = {
                static_cast<unsigned int>(dataAccess.getHotelOwners().size()) + 1,
                username,
                password,
                status
            };
            dataAccess.addHotelOwner(hotel_owner);
        }
    }

    string displayAllHotels() const {
        string result;
        if (dataAccess.getHotels().size() > 0) {
            for (const auto& hotel : dataAccess.getHotels()) {
                result += "ID: " + to_string(hotel.id) + ", Название: " + hotel.title +
                     ", Город: " + hotel.city + ", Цена за ночь: " + to_string(hotel.price_per_night) + "\n";
            }
        } else {
            result = "Каталог отелей пуст\n";
        }
        return result;
    }

    string displayApplications() const {
        string result;
        if (dataAccess.getApplications().size() > 0) {
            for (const auto& app : dataAccess.getApplications()) {
                result += "ID: " + to_string(app.id) + ", Отправитель: " + to_string(app.sender_id) +
                     ", Название: " + app.title + ", Город: " + app.city +
                     ", Цена за ночь: " + to_string(app.price_per_night) + "\n";
            }
        } else {
            result = "Активных заявок нет\n";
        }
        return result;
    }

    string authorizeUser(const string& username, const string& password) {
        for (auto& usr : dataAccess.getRegularUsers()) {
            if (username == usr.username && password == usr.password) {
                return "обычный пользователь";
            }
        }

        for (auto& usr : dataAccess.getHotelOwners()) {
            if (username == usr.username && password == usr.password) {
                return "владелец отеля";
            }
        }

        for (auto& usr : dataAccess.getAdmins()) {
            if (username == usr.username && password == usr.password) {
                return "администратор";
            }
        }

        return "ошибка";
    }

    string registerUser(const string& username, const string& password, const string& status) {
        for (auto& usr : dataAccess.getRegularUsers()) {
            if (username == usr.username) {
                return "Пользователь с таким именем уже существует";
            }
        }

        for (auto& usr : dataAccess.getHotelOwners()) {
            if (username == usr.username) {
                return "Пользователь с таким именем уже существует";
            }
        }

        createUser(username, password, status);
        return "Пользователь успешно зарегистрирован";
    }

private:
    DataAccess& dataAccess;
};

Request get_com(const string& command) {
    if (command == "SELECT") return Request::SELECT;
    if (command == "INSERT") return Request::INSERT;
    if (command == "DELETE") return Request::DELETE;
    if (command == "LOGIN") return Request::SELECT;
    if (command == "REGISTER") return Request::INSERT;
    return Request::UNKNOWN;
}

string process_request(const string& request, BusinessLogic &bis_log) {
    vector<string> splited = split(request, ' ');
    Request choice = get_com(splited[0]);

    if (splited[0] == "LOGIN") {
        if (splited.size() < 3) return "Неверная команда входа";
        return bis_log.authorizeUser(splited[1], splited[2]);
    }
    if (splited[0] == "REGISTER") {
        if (splited.size() < 4) return "Неверная команда регистрации";
        return bis_log.registerUser(splited[1], splited[2], splited[3] + " " + splited[4]);
    }
    if (splited[0] == "GET_HOTELS") {
        return bis_log.displayAllHotels();
    }
    if (splited[0] == "GET_APPLICATIONS") {
        return bis_log.displayApplications();
    }
    if (splited[0] == "CREATE_APPLICATION") {
        if (splited.size() < 5) return "Неверные параметры создания заявки";
        try {
            bis_log.createApplication(stoul(splited[1]), splited[2], splited[3], stoul(splited[4]));
            return "Заявка успешно создана";
        } catch (...) {
            return "Неверные параметры";
        }
    }
    if (splited[0] == "CREATE_HOTEL") {
        if (splited.size() < 5) return "Неверные параметры создания отеля";
        try {
            bis_log.createHotel(stoul(splited[1]), splited[2], splited[3], stoul(splited[4]));
            return "Отель успешно создан";
        } catch (...) {
            return "Неверные параметры";
        }
    }

    return "Неизвестная команда";
}

void serve_client(int clientSocket, const char* clientIP, BusinessLogic &bis_log) {
    ++cntThreads;

    while (true) {
        vector<char> client(1024);
        memset(client.data(), 0, client.size());

        ssize_t bytesRead = recv(clientSocket, client.data(), client.size() - 1, 0);
        if (bytesRead <= 0) {
            cout << "Клиент [" << clientIP << "] отключился" << endl;
            break;
        }

        {
            lock_guard<mutex> lock(mainMuter);
            client.data()[bytesRead] = '\0';
            cout << "Получен запрос: " << client.data() << endl;
        }

        string answer = process_request(client.data(), bis_log);
        send(clientSocket, answer.c_str(), answer.size(), 0);
    }

    close(clientSocket);
    --cntThreads;
}

void start_server() {
    int serverSocket;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Ошибка создания сокета" << endl;
        return;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        cerr << "Ошибка установки параметров сокета" << endl;
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Ошибка привязки сокета" << endl;
        return;
    }

    if (listen(serverSocket, 1) < 0) {
        cerr << "Ошибка прослушивания сокета" << endl;
        return;
    }

    cout << "Сервер запущен" << endl;

    DataAccess dataAccess;
    BusinessLogic businessLogic(dataAccess);

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);
        if (clientSocket < 0) {
            cout << "Ошибка подключения клиента" << endl;
            continue;
        }

        if (cntThreads <= 50) {
            char* clientIP = inet_ntoa(clientAddress.sin_addr);
            cout << "Клиент[" << clientIP << "] подключился" << endl;
            thread(serve_client, clientSocket, clientIP, ref(businessLogic)).detach();

            string answer = "Успешное подключение к серверу";
            send(clientSocket, answer.c_str(), answer.size(), 0);
        }
        else {
            string answer = "Сервер перегружен, попробуйте позже";
            send(clientSocket, answer.c_str(), answer.size(), 0);
            close(clientSocket);
        }
    }

    close(serverSocket);
}

int main() {
    start_server();
    return 0;
}