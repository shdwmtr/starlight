use reqwest;
use serde::Deserialize;
use tokio;

#[derive(Deserialize)]
struct Release {
    tag_name: String,
    prerelease: bool,
    assets: Vec<Asset>,
}

#[derive(Deserialize)]
struct Asset {
    browser_download_url: String,
}

pub async fn get_latest_release() -> Result<(), Box<dyn std::error::Error>> {
    let url = "https://api.github.com/repos/SteamClientHomebrew/Millennium/releases";
    
    // Send the GET request
    let response = reqwest::get(url)
        .await?
        .json::<Vec<Release>>()
        .await?;

    // Find the latest non-prerelease
    if let Some(release) = response.iter().find(|r| !r.prerelease) {
        println!("Latest release: {}", release.tag_name);
        for asset in &release.assets {
            println!("Download URL: {}", asset.browser_download_url);
        }
    } else {
        println!("No release found");
    }

    Ok(())
}