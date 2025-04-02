#pragma once
#include <string>
#include <windows.h>
#include <iostream> 

inline std::string BytesToReadableFormat(float bytes)
{
    const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    int suffixIndex = 0;

    while (bytes >= 1024 && suffixIndex < 4)
    {
        bytes /= 1024;
        suffixIndex++;
    }

    char buffer[32];
    sprintf(buffer, "%.2f %s", bytes, suffixes[suffixIndex]);

    return std::string(buffer);
}

enum ColorScheme
{
    Dark,
    Light
};

static ColorScheme GetWindowsColorScheme()
{
    HKEY key;
    DWORD value;
    DWORD size = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &key) != ERROR_SUCCESS)
    {
        return Dark;
    }

    if (RegQueryValueEx(key, "SystemUsesLightTheme", nullptr, nullptr, (LPBYTE)&value, &size) != ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return Dark;
    }

    RegCloseKey(key);
    return value == 1 ? Light : Dark;
}

static std::string SanitizeDirectoryName(const std::string& path) 
{
    std::wstring widePath(path.begin(), path.end());
    
    size_t startPos = 0;
    if (widePath.length() >= 2 && widePath[1] == L':') 
    {
        widePath[0] = std::towupper(widePath[0]);
        startPos = 2;
    }
    
    std::wstring result = (startPos == 2) ? widePath.substr(0, 2) : L"";
    
    size_t pos = startPos;
    while (pos < widePath.length()) 
    {
        size_t delimPos = widePath.find_first_of(L"/\\", pos);
        if (delimPos == std::wstring::npos) 
        {
            delimPos = widePath.length();
        }
        
        std::wstring component = widePath.substr(pos, delimPos - pos);
        if (!component.empty()) 
        {
            if (!result.empty() && result.back() != L'\\') 
            {
                result += L'\\';
            }

            WIN32_FIND_DATAW findData;
            std::wstring searchPath = result.empty() ? component : result + component;
            HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
            
            if (hFind != INVALID_HANDLE_VALUE) 
            {
                result += findData.cFileName;
                FindClose(hFind);
            } 
            else 
            {
                result += component;
            }
        }
        pos = delimPos + 1;
    }
    
    std::string correctPath(result.begin(), result.end());
    return correctPath;
}

static std::string GetSteamPath()
{
    HKEY key;
    char value[260]; 
    DWORD size = sizeof(value);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &key) != ERROR_SUCCESS)
    {
        return {};
    }

    if (RegQueryValueExA(key, "SteamPath", nullptr, nullptr, (LPBYTE)value, &size) != ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return {};
    }

    RegCloseKey(key);
    return SanitizeDirectoryName(std::string(value, size - 1));  // Remove null terminator
}

static const void OpenUrl(const char* url)
{
    #ifdef _WIN32
    ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
    #elif __linux__
    system(fmt::format("xdg-open {}", url).c_str());
    #endif
}