use tauri::command;

#[cfg(target_os = "windows")]
#[command]
pub fn get_steam_path() -> Result<String, String> {
    use winreg::enums::*;
    use winreg::RegKey;
    use std::fs;
    use std::path::Path;
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

#[cfg(target_os = "linux")]
#[command]
pub fn get_steam_path() -> Result<String, String> {
    use std::process::Command;
    use tauri::api::path::home_dir;

    let output = Command::new("readlink").arg("-f").arg(home_dir().expect("Failed to get home dir").join(".steam/steam")).output().expect("Failed to get steam path.");
    let clean_path = std::str::from_utf8(&output.stdout).unwrap().replace("\n", "").parse().unwrap();
    if output.status.success() {
        Ok(clean_path)
    } else { return Err("Failed to get steam path.".into()); }
}

#[cfg(target_os = "windows")]
#[command]
pub fn close_steam_process() -> Result<bool, String> {
    use sysinfo::System;
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

#[cfg(target_os = "linux")]
#[command]
pub(crate) fn close_steam_process() -> Result<bool, String> {
    use psutil::process;
    use process::processes;

    match processes() {
        Ok(procs) => {
            for process in procs {
                let pr = process.unwrap();
                if pr.name().unwrap().to_lowercase().contains("steam") {
                    pr.kill().map_err(|e| e.to_string())?;
                    println!("Successfully killed process '{}'", "steam");
                    return Ok(true);
                }
            }
            Err("steam.exe process not found".into())
        }
        Err(e) => Err(e.to_string()),
    }
}