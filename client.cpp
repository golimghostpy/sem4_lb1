#include <iostream>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERV_IP "217.71.129.139"
#define PORT 5319

using namespace std;

void displayMenu() {
    cout << "1. Вход" << endl;
    cout << "2. Регистрация обычного пользователя" << endl;
    cout << "3. Регистрация владельца отеля" << endl;
    cout << "4. Выход" << endl;
}

void displayUserMenu(const string& role) {
    if (role == "обычный пользователь") {
        cout << "1. Просмотреть все отели" << endl;
        cout << "2. Выйти" << endl;
    }
    else if (role == "владелец отеля") {
        cout << "1. Просмотреть все отели" << endl;
        cout << "2. Создать новую заявку на отель" << endl;
        cout << "3. Выйти" << endl;
    }
    else if (role == "администратор") {
        cout << "1. Просмотреть заявки" << endl;
        cout << "2. Одобрить заявку" << endl;
        cout << "3. Выйти" << endl;
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Ошибка создания сокета" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr) <= 0) {
        cerr << "Неверный адрес" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Ошибка подключения" << endl;
        return -1;
    }

    char buffer[1024];
    int connectResp = read(sock, buffer, 1024);
    cout << "Сообщение сервера: " << buffer << endl;

    string currentRole = "";
    int currentUserId = -1;

    while (true) {
        if (currentRole.empty()) {
            displayMenu();
            cout << "Введите выбор: ";
            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 4) break;

            if (choice == 1) {
                string username, password;
                cout << "Имя пользователя: ";
                getline(cin, username);
                cout << "Пароль: ";
                getline(cin, password);

                string request = "LOGIN " + username + " " + password;
                send(sock, request.c_str(), request.size(), 0);

                vector<char> response(1024);
                ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                if (byteResp > 0) {
                    response.data()[byteResp] = '\0';
                    currentRole = string(response.data());
                    if (currentRole == "ошибка") {
                        currentRole = "";
                        cout << "Ошибка входа" << endl;
                    } else {
                        cout << "Вы вошли как " << currentRole << endl;
                    }
                }
            }
            else if (choice == 2 || choice == 3) {
                string username, password;
                cout << "Имя пользователя: ";
                getline(cin, username);
                cout << "Пароль: ";
                getline(cin, password);

                string role = (choice == 2) ? "обычный пользователь" : "владелец отеля";
                string request = "REGISTER " + username + " " + password + " " + role;
                send(sock, request.c_str(), request.size(), 0);

                vector<char> response(1024);
                ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                if (byteResp > 0) {
                    response.data()[byteResp] = '\0';
                    cout << string(response.data()) << endl;
                }
            }
        }
        else {
            displayUserMenu(currentRole);
            cout << "Введите выбор: ";
            int choice;
            cin >> choice;
            cin.ignore();

            if ((currentRole == "обычный пользователь" && choice == 2) ||
                (currentRole == "владелец отеля" && choice == 3) ||
                (currentRole == "администратор" && choice == 3)) {
                currentRole = "";
                cout << "Вы вышли из системы" << endl;
                continue;
            }

            if (currentRole == "обычный пользователь") {
                if (choice == 1) {
                    string request = "GET_HOTELS";
                    send(sock, request.c_str(), request.size(), 0);

                    vector<char> response(1024);
                    ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                    if (byteResp > 0) {
                        response.data()[byteResp] = '\0';
                        cout << string(response.data()) << endl;
                    }
                }
            }
            else if (currentRole == "владелец отеля") {
                if (choice == 1) {
                    string request = "GET_HOTELS";
                    send(sock, request.c_str(), request.size(), 0);

                    vector<char> response(1024);
                    ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                    if (byteResp > 0) {
                        response.data()[byteResp] = '\0';
                        cout << string(response.data()) << endl;
                    }
                }
                else if (choice == 2) {
                    string title, city;
                    unsigned int price;
                    cout << "Название отеля: ";
                    getline(cin, title);
                    cout << "Город: ";
                    getline(cin, city);
                    cout << "Цена за ночь: ";
                    cin >> price;
                    cin.ignore();

                    string request = "CREATE_APPLICATION " + to_string(currentUserId) + " " + title + " " + city + " " + to_string(price);
                    send(sock, request.c_str(), request.size(), 0);

                    vector<char> response(1024);
                    ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                    if (byteResp > 0) {
                        response.data()[byteResp] = '\0';
                        cout << string(response.data()) << endl;
                    }
                }
            }
            else if (currentRole == "администратор") {
                if (choice == 1) {
                    string request = "GET_APPLICATIONS";
                    send(sock, request.c_str(), request.size(), 0);

                    vector<char> response(1024);
                    ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                    if (byteResp > 0) {
                        response.data()[byteResp] = '\0';
                        cout << string(response.data()) << endl;
                    }
                }
                else if (choice == 2) {
                    unsigned int appId;
                    cout << "ID заявки для одобрения: ";
                    cin >> appId;
                    cin.ignore();

                    string request = "CREATE_HOTEL " + to_string(appId) + " НазваниеОтеля Город 100";
                    send(sock, request.c_str(), request.size(), 0);

                    vector<char> response(1024);
                    ssize_t byteResp = recv(sock, response.data(), response.size() - 1, 0);
                    if (byteResp > 0) {
                        response.data()[byteResp] = '\0';
                        cout << string(response.data()) << endl;
                    }
                }
            }
        }
    }

    close(sock);
    cout << "Отключено от сервера" << endl;
    return 0;
}