use std::fs;
use std::io;
use std::path::Path;
use walkdir::WalkDir;

fn delete_with_force<P: AsRef<Path>>(path: P) -> io::Result<()> {
    let path = path.as_ref();
    
    if !path.exists() {
        return Err(io::Error::new(io::ErrorKind::NotFound, "Component wasn't found on disk"));
    }

    if path.is_file() {
        fs::remove_file(path)?;
    } else if path.is_dir() {
        for entry in WalkDir::new(path).min_depth(1).contents_first(true) {
            let entry = entry?;
            let path = entry.path();
            if path.is_file() {
                fs::remove_file(path)?;
            } else if path.is_dir() {
                fs::remove_dir(path)?;
            }
        }
        fs::remove_dir(path)?;
    }
    
    Ok(())
}

#[tauri::command]
pub fn delete_recursive(path: &str) -> Result<bool, String> {
    match delete_with_force(path) {
        Ok(_) => Ok(true),
        Err(err) => Err(format!("{}", err)),
    }
}