#include <ranges>
#include <filesystem>
#include <fstream>
#include <regex>
#include <csignal>

#include "settingslib.h"
#include <loglib/loglib.h>

#define  SELF_NAME_FILE  "/proc/self/comm"
#define  COMMENT_CHARACTER  ';'
#define  SECTION_REGEX  "^\\[(.+)\\]$"

void SettingsLib::getApplicationName(){
    std::ifstream(SELF_NAME_FILE) >> appName;
    if (appName.empty()){
        LOG_ERROR("Could not determine application name!");
        exit(1);
    }
}

std::string trimLeft(std::string s)
{
    if (!s.length())
        return s;

    if (std::isblank(s[0]))
        return trimLeft(s.substr(1));

    return s;
}

std::string trimRight(std::string s)
{
    if (!s.length())
        return s;

    if (std::isblank(s[s.length() - 1]))
        return trimRight(s.substr(0, s.length() - 1));

    return s;
}

std::string trim(std::string s)
{
    return trimLeft(trimRight(s));
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
        LOG_ERROR("Section header is empty!");
    }

    std::string key, val;
    size_t equalSignIndex = line.find_first_of('=');
    key = trim(line.substr(0, equalSignIndex));
    val = trim(line.substr(equalSignIndex + 1));

    s[sect][key] = val;
}

std::map<std::string, configs> parseSettingsFile(std::string path){
    std::ifstream configFile {path};
    std::string line;

    std::map<std::string, configs> ret;
    std::string section_name;

    for(;std::getline(configFile, line);){
        line = trim(line);
        if (isComment(line) || isEmpty(line)){
            LOG_DEBUG_F("Line '{}' is comment or empty", line);
            continue;
        }

        if (isSectionHeader(line)){
            section_name = getSectionName(line);
            LOG_DEBUG_F("Section name is {}", section_name);
            continue;
        }

        if (isConfig(line)){
            LOG_DEBUG_F("Line '{}' is config", line);
            addConfig(line, section_name, ret);
        }
    }

    configFile.close();
    return ret;
}

void SettingsLib::parseSettings(int optionalSignal){
    std::string path = std::format("{}/{}.cfg", me->configFolderPath, me->appName);
    if (!std::filesystem::exists(path)) {
        LOG_ERROR_F("{} config does not exist!", path);
        return;
    }

    me->settings = parseSettingsFile(path);
}

SettingsLib::SettingsLib()
{
    me = this;
    getApplicationName();
    parseSettings();
    std::signal(SIGUSR2, SettingsLib::parseSettings);
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
        LOG_ERROR_F("Section '{}' does not exist.", section);
        return "";
    }

    auto configIt = sectionIt->second.find(key);
    if (configIt == sectionIt->second.end()) {
        LOG_ERROR_F("Key '{}' does not exist", key);
        return "";
    }

    return settings[section][key];
}

std::string SettingsLib::getMandatoryValue(const std::string &section, const std::string &key)
{
    std::string ret = getValue(section, key);
    if (ret.empty()){
        throw std::runtime_error("Missing property in config file");
    }
    return ret;
}

std::vector<std::string> SettingsLib::getSections()
{
    auto keys = std::views::keys(settings);
    std::vector<std::string> ret{keys.begin(), keys.end()};
    return ret;
}
