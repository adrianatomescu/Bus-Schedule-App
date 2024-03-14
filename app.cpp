#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cctype>

using namespace std;

// Clasa pentru gestionarea exceptiilor specifice
class CustomException : public exception
{

private:
    string errorMsg;

public:
    explicit CustomException(const string &message) : errorMsg(message) {}
    const char *what() const noexcept override
    {
        return errorMsg.c_str();
    }
};

class Operator
{
private:
    string opName, opPassword;

public:
    Operator(const string &opName, const string &opPassword) : opName(opName), opPassword(opPassword) {}

    const string &getUsername() const
    {
        return opName;
    }

    const string &getPassword() const
    {
        return opPassword;
    }
};

// Clasa care gestioneaza actiunile legate de operator (login si managementul cursei de autocar)
class OperatorHandler
{
public:
    // Metoda pentru login-ul operatorului
    bool login(const string &inputName, const string &inputPassword)
    {
        try
        {

            // Identificarea operatorului in CSV
            ifstream file("operators.csv");
            string line;

            while (getline(file, line))
            {
                stringstream ss(line);
                string storedName, storedPassword;

                getline(ss, storedName, ',');
                getline(ss, storedPassword);
                // Eliminam spatiile din numele si parola stocate
                storedName.erase(remove_if(storedName.begin(), storedName.end(), ::isspace), storedName.end());
                storedPassword.erase(remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());

                if (storedName == inputName && storedPassword == inputPassword)
                {
                    file.close();
                    return true;
                }
            }
            file.close();
            throw CustomException("Login nereusit. Nume sau parola gresita.");
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie: " << e.what() << endl;
            return false;
        }
    }

    // Metoda pentru adaugarea unei noi curse de autocar
    void addBusTrip(const string &departure, const string &arrival, const string &date, const string &startTime, const string &endTime)
    {
        try
        {
            ifstream file("bus_schedule.csv"); // Deschidem fisierul pentru citire

            string line;
            bool alreadyExists = false;

            while (getline(file, line))
            {
                stringstream ss(line);
                string storedDeparture, storedArrival, storedDate, storedStartTime, storedEndTime;

                getline(ss, storedDeparture, ',');
                getline(ss, storedArrival, ',');
                getline(ss, storedDate, ',');
                getline(ss, storedStartTime, ',');
                getline(ss, storedEndTime);

                // Verificam daca exista o cursa cu aceleasi detalii
                if (storedDeparture == departure && storedArrival == arrival && storedDate == date &&
                    storedStartTime == startTime && storedEndTime == endTime)
                {
                    alreadyExists = true;
                    break;
                }
            }

            file.close();

            if (!alreadyExists)
            {

                // Daca cursa nu exista, o adaugam in fisier
                ofstream outputFile("bus_schedule.csv", ios::app);
                outputFile << departure << "," << arrival << "," << date << "," << startTime << "," << endTime << "\n";
                outputFile.close();
                cout << "Calatorie adaugata cu succes!" << endl;
            }
            else
            {
                cout << "Aceasta cursa exista deja!" << endl;
            }
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie la adaugarea cursei: " << e.what() << endl;
        }
    }

    // Metoda pentru stergerea unei curse de autocar
    void deleteBusTrip(const string &departure, const string &arrival, const string &date, const string &startTime, const string &endTime)
    {
        try
        {
            ifstream inputFile("bus_schedule.csv");
            ofstream outputFile("temp_schedule.csv"); // Fisier temporar pentru a scrie cursele filtrate

            string line;

            while (getline(inputFile, line))
            {
                stringstream ss(line);
                string storedDeparture, storedArrival, storedDate, storedStartTime, storedEndTime;

                getline(ss, storedDeparture, ',');
                getline(ss, storedArrival, ',');
                getline(ss, storedDate, ',');
                getline(ss, storedStartTime, ',');
                getline(ss, storedEndTime);

                // Verificam daca cursa din fisier corespunde cu cursa specificata si o eliminam din fisier
                if (!(storedDeparture == departure && storedArrival == arrival &&
                      storedDate == date && storedStartTime == startTime && storedEndTime == endTime))
                {
                    outputFile << storedDeparture << "," << storedArrival << "," << storedDate << ","
                               << storedStartTime << "," << storedEndTime << "\n";
                }
            }

            inputFile.close();
            outputFile.close();

            // Inlocuim fisierul original cu continutul fisierului temporar care contine doar cursele care nu au fost sterse
            remove("bus_schedule.csv");
            rename("temp_schedule.csv", "bus_schedule.csv");

            cout << "Cursa a fost stearsa cu succes din fisierul original!" << endl;
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie la stergerea cursei: " << e.what() << endl;
        }
    }
};

// Clasa pentru gestionarea utilizatorilor
class UserHandler
{
public:
    // Verificam daca parola este considerata slaba
    bool isWeakPassword(const string &password)
    {
        return password.length() < 8; // consideram o ca o parola puternica are cel putin 8 caractere
    }

    // Adaugam un nou utilizator in fisierul CSV
    void addNewUserToCSV(const string &newUsername, const string &newPassword)
    {
        ofstream file("users.csv", ios::app);
        file << newUsername << "," << newPassword << "\n";
        file.close();
    }

    // Verificam daca un timp dat se afla intr-un anumit interval de timp
    bool isTimeWithinInterval(const string &time, const string &startTime, const string &endTime)
    {
        return time >= startTime && time <= endTime;
    }
};

// Clasa pentru manipularea utilizatorilor si a calatoriilor
class User
{
private:
    string username, password;

public:
    // Metoda pentru setarea numelui si parolei utilizatorului
    User(const string &username, const string &password) : username(username), password(password) {}

    const string &getUsername() const
    {
        return username;
    }

    const string &getPassword() const
    {
        return password;
    }

    // Creeaza cont pentru un utilizator nou
    void createAccount(const string &newUsername, const string &newPassword)
    {
        UserHandler handler;

        try
        {
            string passwordInput = newPassword;

            // Cerem o parola puternica pana este introdusa una valida
            while (handler.isWeakPassword(passwordInput))
            {
                cout << "Introduceti o parola mai puternica de cel putin 8 caractere: ";
                cin >> passwordInput;
            }

            ifstream file("users.csv");
            string line;

            while (getline(file, line))
            {
                stringstream ss(line);
                string storedUser, storedPassword;

                getline(ss, storedUser, ',');
                getline(ss, storedPassword);

                if (storedUser == newUsername)
                {
                    file.close();
                    throw CustomException("Utilizatorul exista deja. Autentificati-va sau alegeti alt username");
                }
            }
            file.close();

            handler.addNewUserToCSV(newUsername, passwordInput);
            cout << "Adaugare reusita! Autentificati-va:" << endl;
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie: " << e.what() << endl;
        }
    }

    // Autentifica un utilizator existent
    bool authenticate(const string &inputUsername, const string &inputPassword)
    {
        UserHandler handler;

        try
        {
            ifstream file("users.csv");
            string line;

            while (getline(file, line))
            {
                stringstream ss(line);
                string storedUser, storedPassword;

                getline(ss, storedUser, ',');
                getline(ss, storedPassword);
                // Eliminam spatiile din numele de utilizator ai parola stocata
                storedUser.erase(remove_if(storedUser.begin(), storedUser.end(), ::isspace), storedUser.end());
                storedPassword.erase(remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());

                if (storedUser == inputUsername && storedPassword == inputPassword)
                {
                    file.close();
                    return true;
                }
            }
            file.close();
            throw CustomException("Autentificare esuata. Username sau parola gresita.");
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie: " << e.what() << endl;
            return false;
        }
    }

    // Cauta autocare conform unor scriterii specificate
    void searchBus(const string &departure, const string &arrival, const string &date, const string &startTime, const string &endTime)
    {
        UserHandler handler;

        try
        {
            ifstream file("bus_schedule.csv");
            string line;
            int busCounter = 0;

            while (getline(file, line))
            {
                stringstream ss(line);
                string storedDeparture, storedArrival, storedDate, storedTime;

                getline(ss, storedDeparture, ',');
                getline(ss, storedArrival, ',');
                getline(ss, storedDate, ',');
                getline(ss, storedTime);

                if (storedDeparture == departure && storedArrival == arrival && storedDate == date && handler.isTimeWithinInterval(storedTime, startTime, endTime))
                {
                    busCounter++;
                    cout << "Calatoria " << busCounter << ":" << endl;
                    cout << "Plecare: " << storedDeparture << endl;
                    cout << "Sosire: " << storedArrival << endl;
                    cout << "Data: " << storedDate << endl;
                    cout << "Ora: " << storedTime << endl;
                    cout << endl;
                }
            }

            if (busCounter == 0)
            {
                cout << "Nu s-au gasit autocare care sa corespunda criteriilor de cautare" << endl;
            }
            else
            {
                cout << "Au fost gasite " << busCounter << " autocare care corespund criteriilor de cautare" << endl;
            }

            file.close();
        }
        catch (const exception &e)
        {
            cerr << "A aparut o exceptie: " << e.what() << endl;
        }
    }

    // Metoda de rezervare
    string markTripAsReserved(int selectedTripIndex);
};

string User::markTripAsReserved(int selectedTripIndex)
{
    string details;

    try
    {
        ifstream file("bus_schedule.csv");
        string line;
        int currentTripIndex = 0;

        while (getline(file, line))
        {
            if (currentTripIndex == selectedTripIndex - 1)
            {
                //  Am gasit calatoria
                details = line;
                break;
            }
            currentTripIndex++;
        }

        file.close();
    }
    catch (const exception &e)
    {
        cerr << "A aparut o exceptie: " << e.what() << endl;
    }

    return details;
};

int main()
{

    string roleChoice;
    cout << "Alegeti intre Operator sau Utilizator (operator/utilizator): ";
    cin >> roleChoice;

    if (roleChoice == "operator")
    {
        OperatorHandler opHandler;
        string opName, opPassword;

        bool authenticated = false;
        do
        {
            cout << "Introduceti numele: ";
            cin >> opName;
            cout << "Introduceti parola: ";
            cin >> opPassword;

            if (opHandler.login(opName, opPassword))
            {
                authenticated = true;
                break; // Iesire din bucla daca autentificarea este reusita
            }
            else
            {
                cout << "Autentificare esuata. Nume sau parola gresita." << endl;
                string retryChoice;
                cout << "Doriti sa incercati din nou? (da/nu): ";
                cin >> retryChoice;

                if (retryChoice != "da")
                {
                    break; // Iesire din bucla daca utilizatorul nu doreste sa continue încercarile de autentificare
                }
            }
        } while (!authenticated);

        if (authenticated)
        {
            string choice;
            cout << "Autentificare reusita ca operator. Ce actiune doriti sa realizati? (adaugare/stergere): ";
            cin >> choice;

            if (choice == "adaugare")
            {
                string departure, arrival, date, startTime, endTime;

                cout << "Introduceti orasul de plecare: ";
                cin.ignore();
                getline(cin, departure);

                cout << "Introduceti orasul de sosire: ";
                getline(cin, arrival);

                cout << "Introduceti data (exemplu: 09.12.2023): ";
                getline(cin, date);

                cout << "Introduceti ora de plecare: ";
                getline(cin, startTime);

                cout << "Introduceti ora de sosire: ";
                getline(cin, endTime);

                opHandler.addBusTrip(departure, arrival, date, startTime, endTime);
            }
            else if (choice == "stergere")
            {
                string departure, arrival, date, startTime, endTime;

                cout << "Introduceti orasul de plecare pentru stergere: ";
                cin.ignore();
                getline(cin, departure);

                cout << "Introduceti orasul de sosire pentru stergere: ";
                getline(cin, arrival);

                cout << "Introduceti data pentru stergere (exemplu: 09.12.2023): ";
                getline(cin, date);

                cout << "Introduceti ora de plecare pentru stergere: ";
                getline(cin, startTime);

                cout << "Introduceti ora de sosire pentru stergere: ";
                getline(cin, endTime);

                opHandler.deleteBusTrip(departure, arrival, date, startTime, endTime);
            }
            else
            {
                cout << "Optiune invalida." << endl;
            }
        }
        else
        {
            cout << "Autentificare esuata. Nume sau parola gresita." << endl;
        }
    }

    else if (roleChoice == "utilizator")
    {
        string userChoice;
        cout << "Doriti sa va autentificati sau sa creati un cont nou? (autentificare/nou): ";
        cin >> userChoice;

        User user("", "");
        UserHandler userHandler;

        if (userChoice == "nou")
        {
            string username, password;
            cout << "Introduceti numele de utilizator: ";
            cin >> username;

            cout << "Introduceti parola: ";
            cin >> password;

            cin.ignore();

            user.createAccount(username, password);

            // Dupa ce s-a creat contul, cerem utilizatorului sa se autentifice
            bool authenticated = false;

            do
            {
                string username, password;
                cout << "Introduceti numele de utilizator: ";
                cin >> username;

                cout << "Introduceti parola: ";
                cin >> password;

                authenticated = user.authenticate(username, password);

                if (!authenticated)
                {
                    cout << "Autentificare esuata. Username sau parola gresita. Incercati din nou!" << endl;
                    string retryChoice;
                    cout << "Doriti sa incercati din nou? (da/nu): ";
                    cin >> retryChoice;

                    if (retryChoice != "da")
                    {
                        break; // Iesire din bucla do-while daca utilizatorul nu doreste sa continue incercarile de autentificare
                    }
                }
            } while (!authenticated);

            if (authenticated)
            {
                cout << "Autentificare reusita. Introduceti datele calatoriei:" << endl;

                string departure, arrival, date, startTime, endTime;

                cout << "Introduceti orasul de plecare: ";
                cin.ignore();
                getline(cin, departure);

                cout << "Introduceti orasul de sosire: ";
                getline(cin, arrival);

                cout << "Introduceti data (exemplu: 09.12.2023): ";
                getline(cin, date);

                cout << "Introduceti intervalul orar (ora minima de plecare si ora maxima de sosire):" << endl;
                cout << "Ora minima de plecare: ";
                getline(cin, startTime);
                cout << "Ora maxima de sosire: ";
                getline(cin, endTime);

                user.searchBus(departure, arrival, date, startTime, endTime);

                string choice;
                cout << "Doriti sa rezervati o calatorie? (da/nu): ";
                cin >> choice;

                if (choice == "da")
                {
                    int selectedTrip;
                    cout << "Introduceti numarul calatoriei pe care doriti sa o rezervati: ";
                    cin >> selectedTrip;

                    string tripDetails = user.markTripAsReserved(selectedTrip);
                    if (!tripDetails.empty())
                    {
                        cout << "Detaliile calatoriei rezervate sunt: " << tripDetails << endl;
                        cout << "Cursa a fost rezervata cu succes!" << endl;
                    }
                    else
                    {
                        cout << "Nu a fost gasita nicio calatorie cu numarul specificat." << endl;
                    }
                }
                else
                {
                    cout << "Autentificare esuata. Username sau parola gresita." << endl;
                }
            }
        }
        else if (userChoice == "autentificare")
        {
            bool authenticated = false;
            do
            {
                string username, password;
                cout << "Introduceti numele de utilizator: ";
                cin >> username;

                cout << "Introduceti parola: ";
                cin >> password;

                authenticated = user.authenticate(username, password);

                if (!authenticated)
                {
                    cout << "Autentificare esuata. Username sau parola gresita. Incercati din nou!" << endl;
                    string retryChoice;
                    cout << "Doriti sa incercati din nou? (da/nu): ";
                    cin >> retryChoice;

                    if (retryChoice != "da")
                    {
                        break; // Iesire din bucla do-while daca utilizatorul nu doreste sa continue incercarile de autentificare
                    }
                }
            } while (!authenticated);
            if (authenticated)
            {

                cout << "Autentificare reusita. Introduceti datele calatoriei:" << endl;

                string departure, arrival, date, startTime, endTime;

                cout << "Introduceti orasul de plecare: ";
                cin.ignore();
                getline(cin, departure);

                cout << "Introduceti orasul de sosire: ";
                getline(cin, arrival);

                cout << "Introduceti data (exemplu: 09.12.2023): ";
                getline(cin, date);

                cout << "Introduceti intervalul orar (ora minima de plecare si ora maxima de sosire):" << endl;
                cout << "Ora minima de plecare: ";
                getline(cin, startTime);
                cout << "Ora maxima de sosire: ";
                getline(cin, endTime);

                user.searchBus(departure, arrival, date, startTime, endTime);

                string choice;
                cout << "Doriti sa rezervati o calatorie? (da/nu): ";
                cin >> choice;

                if (choice == "da")
                {
                    int selectedTrip;
                    cout << "Introduceti numarul calatoriei pe care doriti sa o rezervati: ";
                    cin >> selectedTrip;

                    string tripDetails = user.markTripAsReserved(selectedTrip);
                    if (!tripDetails.empty())
                    {
                        cout << "Detaliile calatoriei rezervate sunt: " << tripDetails << endl;
                        cout << "Cursa a fost rezervata cu succes!" << endl;
                    }
                    else
                    {
                        cout << "Nu a fost gasita nicio calatorie cu numarul specificat." << endl;
                    }
                }
            }
        }
    }
    else
    {
        cout << "Optiune invalida." << endl;
    }

    return 0;
}
