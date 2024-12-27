// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::fs;
use std::fs::File;
use std::io::{self, Write};
use std::path::{Path, PathBuf};

use reqwest::Client;
use tauri::Manager;

use util::fs::delete_recursive;
use util::steam::close_steam_process;
use util::steam::get_steam_path;
use zip::ZipArchive;

mod util {
    pub mod steam;
    pub mod fs;
}

#[tauri::command]
fn extract_zip(zip_path: String, dest_dir: String) -> Result<(), String> {
    let path = Path::new(&zip_path);
    let file = File::open(&path).map_err(|e| format!("Failed to open zip file: {}", e))?;
    let mut archive = ZipArchive::new(file).map_err(|e| format!("Failed to read zip file: {}", e))?;

    for i in 0..archive.len() {
        let mut file = archive.by_index(i).map_err(|e| format!("Failed to access file in archive: {}", e))?;
        let out_path = Path::new(&dest_dir).join(file.mangled_name());

        if file.name().ends_with('/') {
            std::fs::create_dir_all(&out_path).map_err(|e| format!("Failed to create directory: {}", e))?;
        } else {
            if let Some(parent) = out_path.parent() {
                if !parent.exists() {
                    std::fs::create_dir_all(&parent).map_err(|e| format!("Failed to create parent directories: {}", e))?;
                }
            }
            let mut outfile = File::create(&out_path).map_err(|e| format!("Failed to create output file: {}", e))?;
            std::io::copy(&mut file, &mut outfile).map_err(|e| format!("Failed to copy file contents: {}", e))?;
        }
    }
    Ok(())
}

async fn download_file(url: &str, path: &str) -> Result<(), Box<dyn std::error::Error>> {

    let client = Client::new();

    let response = client.get(url).send().await?;
    let content = response.bytes().await?;

    let parent_dir = Path::new(path).parent().unwrap();
    fs::create_dir_all(parent_dir)?;

    let mut file = File::create(path)?;
    file.write_all(&content)?;

    Ok(())
}

#[tauri::command]
async fn download_file_to_path(url: String, path: String) -> Result<bool, bool> {
    match download_file(&url, &path).await {
        Ok(_) => Ok(true),
        Err(_) => Err(false),
    }
}

fn get_folder_size<P: AsRef<Path>>(path: P) -> io::Result<u64> {
    let mut size = 0;
    for entry in fs::read_dir(path)? {
        let entry = entry?;
        let metadata = entry.metadata()?;
        if metadata.is_dir() {
            size += get_folder_size(entry.path())?;
        } else {
            size += metadata.len();
        }
    }
    Ok(size)
}

#[tauri::command]
fn calc_dir_size(path: String, from_steam_dir: bool) -> Result<u64, String> {
    let folder_path: PathBuf;

    if from_steam_dir {
        let raw_steam_path = get_steam_path().unwrap();
        let steam_path = Path::new(&raw_steam_path);
        let relative_dir = Path::new(&path);
        folder_path = steam_path.join(relative_dir);
    } else {
        folder_path = PathBuf::from(path);
    }

    let metadata = match fs::metadata(&folder_path) {
        Ok(metadata) => metadata,
        Err(_) => return Err("Failed to read file/directory metadata".to_string()),
    };

    if metadata.is_file() {
        Ok(metadata.len())
    } 
    else if metadata.is_dir() {
        match get_folder_size(&folder_path) {
            Ok(size) => Ok(size),
            Err(_) => Err("Failed to calculate folder size".to_string()),
        }
    } else {
        Err("Path does not point to a valid file or directory".to_string())
    }
}

fn center_window(window: tauri::Window) {
    let monitor = window.primary_monitor().unwrap().unwrap();
    let monitor_size = monitor.size();
    let window_size = window.outer_size().unwrap();

    let x = (monitor_size.width - window_size.width) / 2;
    let y = (monitor_size.height - window_size.height) / 2;

    window.set_position(tauri::LogicalPosition { x: x as f64, y: y as f64 }).unwrap();
}

#[tauri::command]
fn is_auto_installer() -> bool {
    let args: Vec<String> = std::env::args().collect();
    args.contains(&String::from("-auto"))
}

fn main() {
    tauri::Builder::default().invoke_handler(tauri::generate_handler![
            // backend functions
            download_file_to_path, 
            get_steam_path, 
            calc_dir_size, 
            delete_recursive,
            close_steam_process,
            is_auto_installer,
            extract_zip

        ]).setup(|app| {
        let window = app.get_window("main").unwrap();
        let mut width = 665.0;
        let mut height = 460.0;

        if is_auto_installer() {
            width = 425.0;
            height = 175.0;

            match close_steam_process() {
                Ok(_) => println!("Closed steam.exe"),
                Err(e) => println!("Failed to close steam.exe: {}", e),
            }
        }

        #[cfg(target_os = "windows")]
        {
            #[cfg(debug_assertions)]
            {
                window.open_devtools();
                window.close_devtools();
            }
            use window_shadows::set_shadow;
            set_shadow(&window, true).unwrap();
        }

        window.set_size(tauri::Size::Logical(tauri::LogicalSize { width, height })).unwrap();
        center_window(window.clone());

        Ok(())
    }).run(tauri::generate_context!()).expect("error while running tauri application");
}
