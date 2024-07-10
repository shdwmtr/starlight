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

use crate::util::steam::__cmd__close_steam_process;

mod util {
    pub mod steam;
    pub mod fs;
}

async fn download_file(url: &str, path: &str) -> Result<(), Box<dyn std::error::Error>> {
    // Create a reqwest Client
    let client = Client::new();

    // Send a GET request to download the file
    let response = client.get(url).send().await?;
    let content = response.bytes().await?;

    // Ensure the parent directory exists
    let parent_dir = Path::new(path).parent().unwrap();
    fs::create_dir_all(parent_dir)?;

    // Write the downloaded content to the specified file path
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
        let raw_steam_path = get_steam_path().unwrap(); // Assuming get_steam_path() returns a Result<String, _>
        let steam_path = Path::new(&raw_steam_path);
        let relative_dir = Path::new(&path);
        folder_path = steam_path.join(relative_dir);
    } else {
        folder_path = PathBuf::from(path);
    }

    // Check if the path is a directory or file
    let metadata = match fs::metadata(&folder_path) {
        Ok(metadata) => metadata,
        Err(_) => return Err("Failed to read file/directory metadata".to_string()),
    };

    if metadata.is_file() {
        // If it's a file, calculate file size
        Ok(metadata.len())
    } else if metadata.is_dir() {
        // If it's a directory, calculate directory size
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
            is_auto_installer
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
