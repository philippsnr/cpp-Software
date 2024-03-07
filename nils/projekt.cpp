#include <iostream>
#include <fstream>
#include <filesystem>
#include <jsoncpp/json/json.h>

using namespace std;
namespace fs = std::filesystem;

class Entry
{
public:
    string type;
    Entry(string _type) : type(_type) {}

    virtual string getBatchCommand()
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
        return "";
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
        return "";
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
        return "";
    }
};

int JsonReader()
{
    // Variablen für Werte aus der JSON-Datei
    string outputfile;
    bool hideshell;
    string application;

    // Vector für Einträge aus entries
    vector<EXE> entries_exe;
    vector<ENV> entries_env;
    vector<PATH> entries_path;

    // JSON-Datei öffnen
    ifstream file("sample.json");
    Json::Reader reader;
    Json::Value root;

    // JSON parsen
    if (!reader.parse(file, root))
    {
        cerr << "Error while parsing the JSON-file" << endl;
        return 1;
    }

    // Werte aus JSON lesen
    outputfile = root["outputfile"].asString();
    hideshell = root["hideshell"].asBool();
    application = root["application"].asString();

    // Einträge aus JSON lesen
    const Json::Value entries_json = root["entries"];
    if (entries_json.isArray())
    {
        for (const Json::Value entry_json : entries_json)
        {
            string type = entry_json["type"].asString();
            if (type == "EXE")
            {
                string command = entry_json["command"].asString();
                entries_exe.push_back(EXE(command));
            }
            else if (type == "ENV")
            {
                string key = entry_json["key"].asString();
                string value = entry_json["value"].asString();
            }
            else if (type == "PATH")
            {
                string path = entry_json["path"].asString();
                entries_path.push_back(PATH(path));
            }
            else
            {
                cerr << "Unknown type: " << type << endl;
                return 1;
            }
        }
    }
    else
    {
        cerr << "" << endl;
        return 1;
    }

    // Ausgabe der gespeicherten Informationen
    cout << "outputfile: " << outputfile << endl;
    cout << "hideshell: " << hideshell << endl;
    cout << "application: " << application << endl;
    cout << "entries:" << endl;
    for (const auto exe : entries_exe)
    {
        cout << "  type: EXE, command: " << exe.command << endl;
    }
    for (const auto env : entries_env)
    {
        cout << "  type: EXE, command: " << env.key << "value: " << env.value << endl;
    }
    for (const auto path : entries_path)
    {
        cout << "  type: PATH, path: " << path.path << endl;
    }  
}

int main()
{
    JsonReader();
    return 0;
}
