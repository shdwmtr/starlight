// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use serde::de::value;
use window_shadows::set_shadow;

mod util {
    pub mod steam;
}

use util::steam::get_steam_path;

use tauri::Manager;
use std::fs::File;
use std::io::{self, Write};
use std::path::{Path, PathBuf};
use reqwest::Client;
use std::fs;
use std::time::Instant;

async fn download_file(url: &str, path: &str) -> Result<(), Box<dyn std::error::Error>> {
    // Create a reqwest Client
    let client = Client::new();

    // Send a GET request to download the file
    let response = client.get(url).send().await?;
    let content = response.bytes().await?;

    // Ensure the parent directory exists
    let parent_dir = Path::new(path).parent().unwrap();
    std::fs::create_dir_all(parent_dir)?;

    // Write the downloaded content to the specified file path
    let mut file = File::create(path)?;
    file.write_all(&content)?;

    Ok(())
}

#[tauri::command]
async fn download_file_to_path(url: String, path: String) -> Result<bool, bool> {
    match download_file(&url, &path).await {
        Ok(_) => Ok(true),
        Err(err) => Err(false),
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

fn main() {
    let start_time = Instant::now();
    let folder_path = "C:/Program Files (x86)/Steam/ext";
    let total_size = get_folder_size(folder_path);
    let duration = start_time.elapsed();
    println!("Total size: {:#?} bytes", total_size.unwrap());
    println!("Time taken: {:?}", duration);

    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![download_file_to_path, get_steam_path, calc_dir_size])
        .setup(|app| {
            let window = app.get_window("main").unwrap();

            #[cfg(debug_assertions)]
            {
                window.open_devtools();
                window.close_devtools();
            }
            set_shadow(&window, true).unwrap();
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
