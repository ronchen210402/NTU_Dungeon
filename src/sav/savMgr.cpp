/*************************************************************************************************************
 * FileName     [ save.cpp ]
 * PackageName  [ save ]
 * Synopsis     [ Define commands for save files manager  ]
 * Author       [ You-Jung (Ron) Chen, Mu-Te (Joshua) Lau, & Pu-Jia Hsieh ]
 *************************************************************************************************************/
#include "savMgr.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

#include <iomanip>

#include <unordered_map>

SavMgr* savMgr = 0;
SavMgr* attrMgr = 0;

// using namespace std;

bool aKeyExists (const std::string& key, const json& j) {
    if (j.find(key) == j.end()) {
        std::cerr << "[Error] Failed to find key \"" << key << "\" in the file. " << std::endl
        << "Please check the attributes and/or savefiles. " << std::endl;
        return false;
    }
    return true;
}

bool keysExist (const std::vector<std::string>& keys, const json& j) {
    for (auto key: keys) {
        if (!aKeyExists(key, j)) return false;
    }
    return true;
}



bool SavMgr::errorSavCmd(SAV_CMD_EXEC_STATUS msg, const std::string& tok)
{
    switch (msg) {
        case SAV_CANNOT_OPEN:
            std::cerr << "Cannot open file \"" << tok << "\"!" << std::endl;
            break;
        case SAV_CANNOT_CREATE:
            std::cerr << "Cannot create file \"" << tok << "\"!" << std::endl;
            break;
        case SAV_CORRUPTED_FILE:
            std::cerr << "File \"" << tok
                 << "\" is either corrupted or not a json file. " << std::endl;
            break;
        case SAV_QUERY_FAIL:
            std::cerr << "Fail to query \"" << tok << "\"!" << std::endl;
            break;
        default:
            std::cerr << "Unknown error!" << std::endl;
    }
    return false;
}

bool SavMgr::loadFile(const std::string& path)
{
    // COMPLETED
    std::ifstream fin(path);
    if (!fin) {
        return errorSavCmd(SAV_CANNOT_OPEN, path);
    }

    std::cout << "Reading in \"" << path << "\"..." << std::endl;
    try {
        _file = json::parse(fin);
    }
    catch (json::parse_error& e) {
        return errorSavCmd(SAV_CANNOT_OPEN, path);
    }
    catch (...) {
        return errorSavCmd(SAV_TOT, path);
    }
    return true;
}

bool SavMgr::saveFile(const std::string& path, const std::string& objectName)
{
    // TODO: save the document as a json file

    std::ofstream fout(path);
    if (!fout) {
        return errorSavCmd(SAV_CANNOT_CREATE, path);
    }

    json j;
    if (!objectName.size()) {
        j = _file;
    }
    else {
        if (!resolve(objectName, j)) {
            return errorSavCmd(SAV_QUERY_FAIL, objectName);
        }
    }

    std::cout << "Saving ";
    if (objectName.size()) std::cout << "object \"" << objectName << "\" ";
    std::cout << "to " << path << "... " << std::endl;

    fout << std::setw(4) << j;
    return true;
}

bool SavMgr::insertByValue(const std::string& key, const std::string& value)
{
    // TODO
    return false;
}

bool SavMgr::insertByFile(const std::string& key, const std::string& filePath) {
    return false;
}

bool SavMgr::print(const std::string& path)
{
    // TODO
    // print out the whole json file
    if (!path.size()) {
        std::cout << std::setw(4) << _file << std::endl;
        return true;
    }
    // resolves to the file layer by layer
    json j;
    if (!resolve(path, j)) return errorSavCmd(SAV_QUERY_FAIL, path);

    std::cout << std::setw(4) << j << std::endl;

    return true;
}

bool SavMgr::getType(const std::string& elemPath) { return false; }

// helper functions
bool SavMgr::resolve(std::string key, json& j)
{
    char* jsonPath = (char*)key.c_str();
    char* pch = strtok(jsonPath, "/");
    j = _file;
    try {
        while (pch != NULL && j != nullptr) {
            j = j[pch];
            pch = strtok(NULL, "/");
        }
    }
    catch (json::type_error& e) {
        return false;
    }

    if (j.is_null()) return false;

    return true;
}

bool SavMgr::createPath(std::string key, json& j) {
    j = _file;
    char* jsonPath = (char*)key.c_str();
    char* pch = strtok(jsonPath, "/");

    while (pch != NULL) {
        if (j[pch] == nullptr) {
            j[pch] = json::object({});
        }
        j = j[pch];
        pch = strtok(NULL, "/");
    }

    
    return false;
}
