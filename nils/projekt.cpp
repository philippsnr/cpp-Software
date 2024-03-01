#include <iostream>

using namespace std;

class Entry
{
    public:
        Entry(string _type) : type(_type){}

       virtual string getBatchCommand()
       {
            return "";
       }

    protected:
        string type; 
};
class ENV : public Entry
{
    public:
        ENV(string _key, string _value) : Entry("ENV") 
        {
            key = _key;
            value = _value;
        }

       // string getBatchCommand()
       // {
        //    return " ";
       // }

    protected:
        string key;
        string value;
};
class EXE: public Entry
{
    public:
        EXE(string _command) : Entry("EXE")
        {
            command = _command;
        }

        /*string getBatchCommand()
        {
            return " ";
        }*/
    protected:
        string command;
};
class PATH : public Entry
{
    public:
        PATH(string _path) : Entry("PATH")
        {
            path = _path;
        }

        string getBatchCommand()
        {
            return path;
        }

    protected:
        string path;
};


int main()
{
    PATH p = PATH("C:\\sicherung\\tools\\MinGW\\lib");
    cout << p.getBatchCommand();

    return 0;
}
