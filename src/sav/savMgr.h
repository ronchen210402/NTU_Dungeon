/*************************************************************************************************************
 * FileName     [ save.cpp ]
 * PackageName  [ save ]
 * Synopsis     [ A manager for reading/writing save files ]
 * Author       [ You-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/
#ifndef SAV_MGR_H
#define SAV_MGR_H

#include "json.hpp"
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

// using namespace std;

bool aKeyExists (const std::string&, const json&);
bool keysExist (const std::vector<std::string>&, const json&);


enum SAV_CMD_EXEC_STATUS {
    SAV_CANNOT_OPEN = 0,
    SAV_CANNOT_CREATE,
    SAV_CORRUPTED_FILE,
    SAV_QUERY_FAIL,
    SAV_TOT
};

class SavMgr {
   public:
    SavMgr() {};
    ~SavMgr() {};

    bool loadFile(const std::string&);
    bool saveFile(const std::string&, const std::string& = "");

    // Wrapper for the two different insert type
    bool insert(const std::string& key, const std::string& value, bool doFile) { 
        return true;
    }
    bool print(const std::string& = "");
    bool getType(const std::string&);

    json& operator* () { return _file; }
    const json& operator* () const { return _file; }
    
    json _file;

   private:
    // data member
    
    bool insertByValue(const std::string&, const std::string&);
    bool insertByFile(const std::string&, const std::string&);

    // member function
    bool init() { return true; }
    // helper functions

    bool resolve(std::string, json& j);
    bool createPath(std::string, json& j);

    bool errorSavCmd(SAV_CMD_EXEC_STATUS, const std::string&);
};

#endif  // SAV_MGR_H