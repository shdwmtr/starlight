use std::fs;
use std::io;
use std::path::Path;

fn delete_with_force<P: AsRef<Path>>(path: P) -> io::Result<()> {
    let path = path.as_ref();

    if !path.exists() {
        return Err(io::Error::new(io::ErrorKind::NotFound, "Component wasn't found on disk"));
    }

    if path.is_file() {
        fs::remove_file(path)?;
    } else if path.is_dir() {
        fs::remove_dir_all(path)?;
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