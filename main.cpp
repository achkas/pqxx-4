#include<iostream>
#include <string>
#include <pqxx/pqxx>
#include <windows.h>
#pragma execution_character_set("utf-8")

class my_tables 
{
private:
    int id;
    std::string name;
    std::string surname;
    std::string email;
    std::string telefon;
    

public:
    my_tables(int m_id, std::string m_name, std::string m_surname, std::string m_email, std::string m_telefon) :
        id{ m_id }, name{ m_name }, surname{ m_surname}, email{m_email}, telefon{m_telefon} {
        /*std::cout << "главныи конструктор";*/};

   

    void CREATE_TABLE_Client(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("CREATE_TABLE_Client");        
        t.commit();
    }

    void CREATE_TABLE_Telefon(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("CREATE_TABLE_Telefon");
        t.commit();
    }
    
    void Client_INSERT(pqxx::connection& conect)
    {      
        pqxx::transaction t(conect); 
        t.exec_prepared("Client_INSERT");      
        t.commit();
    }

    void Telefon_INSERT(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("Telefon_INSERT");
        t.commit();
    }

    void Client_UPDATE(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("Client_UPDATE");
        t.commit();
    }

    void Telefon_DELETE(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("Telefon_DELETE");
        t.commit();
    }

    void Client_DELETE(pqxx::connection& conect)
    {
        pqxx::transaction t(conect);
        t.exec_prepared("Client_DELETE");
        t.commit();
    } 
    
    void Customer_search(pqxx::connection& conect, std::string any)
    {

        pqxx::transaction t(conect);
        auto result = t.query<std::string, std::string>(any);
        for (auto& row : result)
        {
            std::string name = std::get<0>(row);
            std::string surname = std::get<1>(row);
            std::cout << name << " " << surname << std::endl;

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

        //pqxx::transaction r(conect);        
        
        my_tables man(1, " ", " ", " "," "); 

       conect.prepare("CREATE_TABLE_Client", "CREATE TABLE IF NOT EXISTS Client (id SERIAL PRIMARY KEY NOT NULL, name VARCHAR(60) NOT NULL, surname VARCHAR(60) NOT NULL, email VARCHAR(30) NOT NULL)");
       man.CREATE_TABLE_Client(conect);

       conect.prepare("CREATE_TABLE_Telefon", "CREATE TABLE IF NOT EXISTS Telefon (id SERIAL PRIMARY KEY NOT NULL , client_id  INTEGER REFERENCES Client(id), telefon VARCHAR(30))");
       man.CREATE_TABLE_Telefon(conect);
       
        conect.prepare("Client_INSERT", "INSERT INTO Client (name,surname,email) VALUES('Виктор','Володин', 'VDder@')"); 
        man.Client_INSERT(conect);        

        
        conect.prepare("Telefon_INSERT", "INSERT INTO Telefon (telefon) VALUES('+79873450929')");
        man.Telefon_INSERT(conect);

           
       conect.prepare("Client_UPDATE", "UPDATE Client SET name='Владимир' WHERE id = 1");
        man.Client_UPDATE(conect);

      
          conect.prepare("Telefon_DELETE", "DELETE FROM Telefon  WHERE id = 1");
        man.Telefon_DELETE(conect);
           
        conect.prepare("Client_DELETE", "DELETE FROM Client  WHERE id = 1");
        man.Client_DELETE(conect);          

        std::string mail = "VDder@";
        std::string Email_search = "SELECT name, surname  FROM Client WHERE email = '"+ conect.esc(mail) +"'";
        man.Customer_search(conect, Email_search);
       
    }

    catch (pqxx::sql_error e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}