#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>

#include "settingslib.h"
#include <loglibrary.h>

#define  SELF_NAME_FILE  "/proc/self/comm"
#define  COMMENT_CHARACTER  ';'
#define  SECTION_REGEX  "^\\[(.+)\\]$"

void SettingsLib::getApplicationName(){
    std::ifstream(SELF_NAME_FILE) >> appName;
    if (appName.empty()){
        ERROR("Could not determine application name!");
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
    std::regex sectionRegex {SECTION_REGEX};
    std::smatch matches;
    std::regex_search(line, matches, sectionRegex);
    if (matches.ready()) {
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
        ERROR("Section header is empty!");
    }

    std::string key, val;
    size_t equalSignIndex = line.find_first_of('=');
    key = line.substr(0, equalSignIndex);
    val = line.substr(equalSignIndex + 1);

    s[sect][key] = val;
}

std::map<std::string, configs> parseSettingsFile(std::string path){
    std::ifstream configFile {path};
    std::string line;

    std::map<std::string, configs> ret;
    std::string section_name;

    while (configFile >> line){
        if (isComment(line) || isEmpty(line)){
            DEBUG("Line '{}' is comment or empty", line);
            continue;
        }

        if (isSectionHeader(line)){
            section_name = getSectionName(line);
            DEBUG("Section name is {}", section_name);
            continue;
        }

        if (isConfig(line)){
            DEBUG("Line '{}' is config", line);
            addConfig(line, section_name, ret);
        }
    }

    configFile.close();
    return ret;
}

void SettingsLib::parseSettings(){
    std::string path = std::format("{}/{}.cfg", configFolderPath, appName);
    if (!std::filesystem::exists(path)) {
        ERROR("{} config does not exist!", path);
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
        ERROR("Section '{}' does not exist.", section);
        return "";
    }

    auto configIt = sectionIt->second.find(key);
    if (configIt == sectionIt->second.end()) {
        ERROR("Key '{}' does not exist", key);
        return "";
    }

    return settings[section][key];
}
