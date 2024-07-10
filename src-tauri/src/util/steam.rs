use winreg::enums::*;
use winreg::RegKey;
use sysinfo::{System};
use tauri::command;
use std::fs;
use std::path::Path;

#[command]
// todo linux support
pub fn get_steam_path() -> Result<String, String> {
    let hkcu = RegKey::predef(HKEY_CURRENT_USER);

    let steam_key = hkcu.open_subkey("Software\\Valve\\Steam").map_err(|e| {
        format!("Failed to open Steam registry key: {}", e)
    })?;

    let steam_path: String = steam_key.get_value("SteamPath").map_err(|e| {
        format!("Failed to read SteamPath value: {}", e)
    })?;

    let resolved_path = fs::canonicalize(Path::new(&steam_path)).map_err(|e| {
        format!("Failed to canonicalize path: {}", e)
    })?;

    let resolved_path_str = resolved_path.to_string_lossy().to_string();

    let cleaned_path = if resolved_path_str.starts_with(r"\\?\") {
        resolved_path_str[4..].to_string()
    } else {
        resolved_path_str
    };

    Ok(cleaned_path)
}

// todo linux support
#[command]
pub fn close_steam_process() -> Result<bool, String> {
    let mut system = System::new_all();
    system.refresh_all();

    for (pid, process) in system.processes() {
        if process.name() == "steam.exe" {
            println!("Found steam.exe with PID: {}", pid);

            if process.kill() {
                println!("Successfully killed steam.exe");
                return Ok(true);
            } else {
                return Err("Failed to kill steam.exe".into());
            }
        }
    }

    Err("steam.exe process not found".into())
}