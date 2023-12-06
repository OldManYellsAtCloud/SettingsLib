#include "settingslib.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <format>
#include <regex>

#define  SELF_NAME_FILE  "/proc/self/comm"
#define  COMMENT_CHARACTER  ';'
#define  SECTION_REGEX  "^\\[(.+)\\]$"

void SettingsLib::getApplicationName(){
    std::ifstream(SELF_NAME_FILE) >> appName;
    if (appName.empty()){
        std::cerr << "Could not determine application name!" << std::endl;
        exit(1);
    }
}

bool isComment(const std::string& line){
    return line.starts_with(COMMENT_CHARACTER);
}

bool isEmpty(const std::string& line){
    for (const auto& c: line){
        if (!std::isblank(c))
            return false;
    }
    return true;
}

bool isSectionHeader(const std::string& line){
    std::regex sectionRegex {SECTION_REGEX};
    return std::regex_search(line, sectionRegex);
}

std::string getSectionName(const std::string& line){
    std::cout << "Section line: " << line << std::endl;
    std::regex sectionRegex {SECTION_REGEX};
    std::smatch matches;
    std::regex_search(line, matches, sectionRegex);
    if (matches.ready()) {
        std::cout << "Returining section: " << matches[1];
        return matches[1];
    }
    return "";
}

bool isConfig(const std::string& line){
    size_t equalSignIndex = line.find_first_of('=');
    return equalSignIndex != std::string::npos &&
           equalSignIndex != 0 &&
           equalSignIndex != (line.length() - 1);
}

void addConfig(const std::string& line, const std::string& sect, std::map<std::string, configs>& s)
{
    if (isEmpty(sect)){
        std::cerr << "Section header is empty!" << std::endl;
    }

    std::string key, val;
    size_t equalSignIndex = line.find_first_of('=');
    key = line.substr(0, equalSignIndex);
    val = line.substr(equalSignIndex);

    std::cout << "Adding config to section " << sect <<
        "key: " << key << ", val: " << val << std::endl;

    s[sect][key] = val;
}

std::map<std::string, configs> parseSettingsFile(std::string path){
    std::cout << "Parsing config file " << path << std::endl;
    std::ifstream configFile {path};
    std::string line;

    std::map<std::string, configs> ret;
    std::string section_name;

    while (configFile >> line){
        std::cout << "line: " << line << std::endl;
        if (isComment(line) || isEmpty(line)){
            std::cout << "it is empty" << std::endl;
            continue;
        }

        if (isSectionHeader(line)){
            std::cout << "it is section header" << std::endl;
            section_name = getSectionName(line);
            std::cout << "section name: " << section_name;
            continue;
        }

        if (isConfig(line)){
            std::cout << "it is config" << std::endl;
            addConfig(line, section_name, ret);
        }
    }

    configFile.close();
    return ret;
}

void SettingsLib::parseSettings(){
    std::string path = std::format("{}/{}.cfg", configFolderPath, appName);
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("{} config does not exist!", path) << std::endl;
        return;
    }

    settings = parseSettingsFile(path);
}

SettingsLib::SettingsLib()
{
    getApplicationName();
    parseSettings();
}

SettingsLib::SettingsLib(std::string path): configFolderPath {path}
{
    getApplicationName();
    parseSettings();
}

SettingsLib::SettingsLib(std::string applicationName, std::string path) : configFolderPath{path}, appName{applicationName}
{
    parseSettings();
}

std::string SettingsLib::getValue(const std::string &section, const std::string &key)
{
    auto sectionIt = settings.find(section);
    if (sectionIt == settings.end()) {
        std::cerr << "Section " << section << " does not exist." << std::endl;
        return "";
    }

    auto configIt = sectionIt->second.find(key);
    if (configIt == sectionIt->second.end()) {
        std::cerr << "Key " << key << " does not exist." << std::endl;
        return "";
    }

    return settings[section][key];
}
