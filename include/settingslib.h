#ifndef SETTINGSLIB_H
#define SETTINGSLIB_H

#include <string>
#include <map>
#include <vector>

typedef std::map<std::string, std::string> configs;

class SettingsLib
{
private:
    std::string appName;
    std::string configFolderPath;
    std::map<std::string, configs> settings;
    static SettingsLib *me;

    static void parseSettings(int optionalSignal = 0);
    void getApplicationName();
public:
    SettingsLib();
    SettingsLib(std::string path);
    SettingsLib(std::string applicationName, std::string path);
    ~SettingsLib() = default;

    std::string getValue(const std::string& section, const std::string& key);
    std::string getMandatoryValue(const std::string& section, const std::string& key);
    std::vector<std::string> getSections();
};

#endif // SETTINGSLIB_H
