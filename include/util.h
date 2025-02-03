#pragma once
#include <string>
#include <windows.h>

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