#ifndef SETTINGSLIB_H
#define SETTINGSLIB_H

#include <string>
#include <map>

typedef std::map<std::string, std::string> configs;

class SettingsLib
{
private:
    std::string appName;
    std::string configFolderPath;
    std::map<std::string, configs> settings;

    void parseSettings();
    void getApplicationName();
public:
    SettingsLib();
    SettingsLib(std::string path);
    SettingsLib(std::string applicationName, std::string path);
    ~SettingsLib() = default;

    std::string getValue(const std::string& section, const std::string& key);
};

#endif // SETTINGSLIB_H
