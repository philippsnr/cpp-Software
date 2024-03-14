// Erstellen des Programm
// mkdir build
// cd build
// cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja ..
// ninja
// Alternativ diesen Ordner mit VSCode oeffnen
// #include <config.h>
// #include <pathtest.h>

#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;

#include <jsoncpp/json/json.h>

class Entry
{
public:
    string type;
    Entry(string _type) : type(_type) {}

    virtual string getBatchCommand()
    {
        return "";
    }
    virtual string getData()
    {
        return "";
    }
};
class ENV : public Entry
{
public:
    string key;
    string value;
    ENV(string _key, string _value) : Entry("ENV")
    {
        key = _key;
        value = _value;
    }
    string getBatchCommand()
    {
        return "set " + key + "=" + value;
    }
    string getData()
    {
        return key + " , value: " + value;
    }
};

class EXE : public Entry
{
public:
    string command;
    EXE(string _command) : Entry("EXE")
    {
        command = _command;
    }

    string getBatchCommand()
    {
        return command;
    }
    string getData()
    {
        return command;
    }
};

class PATH : public Entry
{
public:
    string path;
    PATH(string _path) : Entry("PATH")
    {
        path = _path;
    }
    string getBatchCommand()
    {
        return path;
    }
    string getData()
    {
        return path;
    }
};

int JsonReader()
{
    // Variablen für Werte aus der JSON-Datei
    string outputfile;
    bool hideshell;
    string application_path;
    string application_name;

    // Vector für Einträge aus entries
    vector<Entry *> entries;

    // JSON-Datei öffnen
    ifstream file("sample.json");
    Json::Reader reader;
    Json::Value root;

    // JSON parsen
    if (!reader.parse(file, root))
    {
        cerr << "Error while parsing the JSON-file" << endl;
        return EXIT_FAILURE;
    }
    // Werte aus JSON lesen
    outputfile = root["outputfile"].asString();
    if (outputfile.find(".bat") == 0)
    {
        cerr << "Error with outputfilename. (LINE 2) \n Has to have '.bat'. Use -h for more Information" << '\n';
        return EXIT_FAILURE;
    }
    string hideshell_error_check = root["hideshell"].asString();
    /*if (hideshell_error_check != "false" || hideshell_error_check != "true")
    {
        cerr << "Error with variable hideshell: Wrong Value. (LINE ) \n Use -h for Help" << '\n';
        return EXIT_FAILURE;
    }
    */
    hideshell = root["hideshell"].asBool();
    application_path = root["application"].asString();

    // Name der Applikation rausfinden
    int index = application_path.rfind(92); /* 92 (ASCII) = \ */
    int length = application_path.length();
    application_name = application_path.substr(index + 1, length - index);

    // Einträge aus JSON lesen
    int line = 4;
    const Json::Value entries_json = root["entries"];
    if (entries_json.isArray())
    {
        for (const Json::Value entry_json : entries_json)
        {
            line++;
            string type = entry_json["type"].asString();
            if (type == "EXE")
            {
                string command = entry_json["command"].asString();
                entries.push_back(new EXE(command));
            }
            else if (type == "ENV")
            {
                string key = entry_json["key"].asString();
                string value = entry_json["value"].asString();
                entries.push_back(new ENV(key, value));
            }
            else if (type == "PATH")
            {
                string path = entry_json["path"].asString();
                entries.push_back(new PATH(path));
            }
            else
            {
                cerr << "Unknown type: " << type << ". (LINE " << line << ") \n Use -h for Help" << '\n';
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        cerr << "Error for entries: Not an Array. (LINE 4) \n Use -h for Help" << endl;
        return EXIT_FAILURE;
    }
    /*xists(application_path) == 0)
    {
        cerr << "Error with application path: Doesn't Exist. (LINE " << line + 2 << ") \n Use -h for Help" << '\n';
        return EXIT_FAILURE;
    }
    */

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde

    // MakeBatch();
    ofstream batchFile(outputfile);

    if (!batchFile.is_open())
    {
        cerr << "Fehler beim Öffnen der Datei!" << endl;
        return 1;
    }
    if (hideshell == 0)
    {
        batchFile << "@echo on" << endl;
    }
    else
    {
        batchFile << "@echo off" << endl;
    }

    for (const auto entry : entries)
    {
        if(entry->type == "EXE")
        {
            batchFile << entry->getBatchCommand() << " && ";
        }   
    }
    for (const auto entry : entries)
    {
        if(entry->type == "ENV")
        {
            batchFile << entry->getBatchCommand() << " && ";
        } 
    }
    batchFile << "set path=";
    for (const auto entry : entries)
    {
        if(entry->type == "PATH")
        {
            batchFile << entry->getBatchCommand() << ";";
        }
    }
    batchFile << "%PATH%";


    // Datei schließen
    batchFile.close();
    // Iteriere über den Vektor und lösche die Objekte
    for (auto entry : entries)
    {
        delete entry;
    }
    // Lösche alle Einträge im Vektor
    entries.clear();
    return EXIT_SUCCESS;
}

/*
    // Ausgabe der gespeicherten Informationen
    cout << "outputfile: " << outputfile << endl;
    cout << "hideshell: " << hideshell << endl;
    cout << "application: " << application_name << " path: " << application_path << endl;
    cout << "entries:" << endl;

if (entry->type == "EXE")
        {
            cout << "  type: EXE, command: " << entry->getData() << endl;
        }
        else if (entry->type == "ENV")
        {
            cout << "  type: ENV, key: " << entry->getData() << endl;
        }
        else if (entry->type == "PATH")
        {
            cout << "  type: PATH, path: " << entry->getData() << endl;
        }
        else
        {
            cerr << "Unknown type: " << entry->type << ". (LINE " << line << ") \n Use -h for Help" << '\n';
            return EXIT_FAILURE;
        }
        cout << "BatchCommand: " << entry->getBatchCommand() << endl;
        */

int main()
{
    JsonReader();
    // MakeBatch();
    return EXIT_SUCCESS;
}
