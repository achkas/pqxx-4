#include<iostream>
#include <string>
#include <pqxx/pqxx>
#include <windows.h>
#pragma execution_character_set("utf-8")

struct Client
{
    std::string _name;
    std::string _surname;
    std::string _email;
    std::string _telefon;
    std::vector<std::string> _phones;
};

class my_tables 
{
private:    

    void CREATE_TABLE_Phones(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec("CREATE TABLE IF NOT EXISTS Phones(id SERIAL PRIMARY KEY NOT NULL, client_id  INTEGER REFERENCES Clients(id), telefon VARCHAR(90))");
        t.commit();
    }

public:        

    void CREATE_TABLE(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec("CREATE TABLE IF NOT EXISTS Clients(id SERIAL PRIMARY KEY NOT NULL, name VARCHAR(60) NOT NULL, surname VARCHAR(60) NOT NULL, email VARCHAR(30) NOT NULL)");        
        t.commit();
        CREATE_TABLE_Phones(conect);
        std::cout << "Таблицы созданы !" << std::endl;
    }    

    void addClient1(pqxx::connection& conect, const std::string& name, const std::string& surname, const std::string& email, const std::string& phone) //Method for adding a new client
    {       
            pqxx::transaction t(conect);            
            pqxx::result res = t.exec_params("INSERT INTO Clients (name, surname, email) VALUES ($1, $2, $3) RETURNING id", name, surname, email);
            if (!res.empty())
            {
                int client_id = res[0][0].as<int>();
                t.exec_params("INSERT INTO Phones (client_id, telefon) VALUES ($1, $2);", client_id, phone);
                t.commit();
                std::cout << "Клиент добавлен ! " << std::endl;
            }
            else
            {
                t.abort();
                std::cout << "Не удалось добавить клиента ! " << std::endl;
            }
            
    }    

    void addClient2(pqxx::connection& conect) 
    {
        Client client;       
        std::cout << "Введите имя ";        
        std::getline(std::cin, client._name);
        
        std::cout << "Введите фамилию ";
        std::getline(std::cin, client._surname);
        
        std::cout << "Введите почту ";
        std::getline(std::cin, client._email);       
    
        try
        {
            pqxx::transaction t(conect);
            pqxx::result res = t.exec_params("INSERT INTO clients (name, surname, email) VALUES ($1, $2, $3) RETURNING id", client._name, client._surname, client._email);
            
            if (!res.empty()) 
            {
                int client_id = res[0][0].as<int>();
                std::string nphone = "-";
                std::string phone;
                std::cout << "Введите номер телефона, если телефона нет введите - ";
                std::getline(std::cin, phone);
                if (phone == nphone)
                {
                    t.exec_params("INSERT INTO Phones (client_id, telefon) VALUES ($1, $2);", client_id, nphone);
                    t.commit();
                    std::cout << "Клиент добавлен ! " << std::endl;
                }                

                else 
                {
                    client._phones.push_back(phone);
                    bool lots_phone_numbers = false;
                    do
                    {
                        std::cout << std::endl << "Введите дополнительный номер телефона, если нет нажмите enter ";
                        std::getline(std::cin, phone);

                        if (phone != "") client._phones.push_back(phone);
                        else lots_phone_numbers = true;
                    }
                    while (!lots_phone_numbers);

                    t.exec_params("INSERT INTO Phones (client_id, telefon) VALUES ($1, $2);", client_id, client._phones);

                    t.commit();
                    std::cout << "Клиент добавлен ! " << std::endl;
                }
            }
            else 
            {
                t.abort();
                std::cout << "Не удалось добавить клиента ! " << std::endl;
            }
            
        }
        catch (const std::exception& ex)
        {            
            std::cout << ex.what() << std::endl;
        }
    }

    void addPhone(pqxx::connection& conect,const std::string& name, const std::string& telefon) 
    {
        pqxx::transaction t(conect);
        pqxx::result res = t.exec_params("SELECT id, surname FROM Clients WHERE name = $1", name);
        if (!res.empty())
        {
            int client_id = res[0][0].as<int>();            
            t.exec_params("INSERT INTO Phones (client_id, telefon) VALUES ($1, $2)", client_id, telefon);

            std::string surname = res[0][1].as<std::string>();
            t.commit();
            std::cout << "Номер телефона добавлен ! " << client_id << " " << name << " " << surname << std::endl;
        }
        else
        {
            t.abort();
            std::cout << "Клиент не найден ! " << name << std::endl;
        }
    }

    void updateClient(pqxx::connection& conect,const std::string& email, const std::string& newName, const std::string& newSurname, const std::string& newEmail) 
    {
        pqxx::transaction t(conect);
        pqxx::result res = t.exec_params("SELECT id FROM Clients WHERE email = $1", email);
        if (!res.empty())
        {
            int client_id = res[0][0].as<int>();
            t.exec_params("UPDATE Clients SET name = $1, surname = $2, email = $3 WHERE id = $4", newName, newSurname, newEmail, client_id);
            t.commit();
            std::cout << "Данные обновлены !: " << client_id << " " << newName << " " << newSurname << " " << newEmail << std::endl;
        }
        else
        {
            t.abort();
            std::cout << "Клиент не найден ! "  << std::endl;
        }
    }

    void deleteTelefon(pqxx::connection& conect,const std::string& email, const std::string& telefon)
    {
        pqxx::transaction t(conect);
        pqxx::result res = t.exec_params("SELECT id, name, surname FROM Clients WHERE email = $1", email);
        if (!res.empty())
        {
            int client_id = res[0][0].as<int>();
            t.exec_params("DELETE FROM Phones WHERE client_id = $1 AND telefon = $2", client_id, telefon);
            t.commit();
            std::string client_name = res[0][1].as<std::string>();
            std::string client_surname = res[0][2].as<std::string>();
            std::cout << "Номер телефона удалён: " << client_id << " " << client_name << " " << client_surname << std::endl;
        }
        else
        {
            t.abort();
            std::cout << "Клиент не найден !" << std::endl;
        }
    }

    void deleteClient(pqxx::connection& conect, const std::string& email) 
    {
        pqxx::transaction t(conect);
        pqxx::result res = t.exec_params("SELECT id, name, surname FROM Clients WHERE email = $1", email);
        if (!res.empty()) {
            int client_id = res[0][0].as<int>();
            std::string client_name = res[0][1].as<std::string>();
            std::string client_surname = res[0][2].as<std::string>();
            t.exec_params("DELETE FROM Phones WHERE client_id = $1", client_id);
            t.exec_params("DELETE FROM Clients WHERE id = $1", client_id);
            t.commit();
            std::cout << "Клиент удалён: " << client_id << " " << client_name << " " << client_surname << std::endl;
        }
        else {
            t.abort();
            std::cout << "Клиент не найден !" << std::endl;
        }
    }

    void Customer_search(pqxx::connection& conect, std::string email)
    {
        
        try
        {
            pqxx::transaction t(conect);
            pqxx::result res = t.exec_params("SELECT id, name, surname FROM Clients WHERE email = $1", email);
            if (!res.empty()) 
            {
                std::string client_id = res[0][0].as<std::string>();
                std::string telefon = t.query_value<std::string>("SELECT telefon  FROM Phones WHERE client_id = '" + conect.esc(client_id) + "'");

                auto result = t.query<std::string, std::string>("SELECT name, surname  FROM Clients WHERE email = '" + conect.esc(email) + "'");
                
                t.commit();

                std::cout << "Клиент: ";

                for (auto& row : result)
                {
                    std::string name = std::get<0>(row);
                    std::string surname = std::get<1>(row);
                    std::cout << name << " " << surname << " "<< email << " ";
                }
                std::cout << telefon << std::endl;
            }
            else
            {
                t.abort();
                std::cout << "Клиент не найден ! " << std::endl;
            }
            
        }
        catch (const std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    } 

   
};



int main() {

    //setlocale(LC_ALL, "ru");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try
    {
        pqxx::connection conect(
            "host =localhost "
            "port = 5432 "
            "dbname = postgres "
            "user = postgres "
            "password=mjgojwx6$FGHc985khKKLO");

        std::cout << "подключение!" << std::endl;      

        my_tables man;
        man.CREATE_TABLE(conect);
        man.addClient1(conect,"AAAAA", "AAA", "A@A","11111111111");
        man.addClient1(conect,"BBBBB", "BBB", "B@B", "22222222222");
        man.addClient1(conect,"CCCCC", "CCC", "C@C", "33333333333");
        man.addClient1(conect,"DDDDD", "DDD", "D@D", "44444444444");
        man.addClient1(conect,"IIIII", "III", "I@I", "55555555555");
        man.addClient1(conect,"FFFFF", "FFF", "F@F", "66666666666");
        man.addPhone(conect, "FFFFF", "56565656565");
        man.updateClient(conect, "C@C", "MMMMM", "MMM", "M@M");
        man.deleteTelefon(conect, "A@A", "11111111111");
        man.deleteClient(conect, "I@I");
        man.Customer_search(conect, "D@D");
        
       
       
    }

    catch (pqxx::sql_error e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}