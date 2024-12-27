import { Platform, platform } from '@tauri-apps/api/os';

declare global {
    interface Window {
        GITHUB_RELEASE_INFO: any;
    }
}

const API_URL = 'https://api.github.com/repos/SteamClientHomebrew/Millennium/releases';

const GitHub = {

    GetReleaseInfo: async () => {

        if (!window?.GITHUB_RELEASE_INFO) {
            const response = await fetch(API_URL)  
            window.GITHUB_RELEASE_INFO = await response.json();
        }

        return window.GITHUB_RELEASE_INFO;
    },

    GetLatestRelease: async () => {

        const platformType: Platform = await platform();
        const release = (await GitHub.GetReleaseInfo()).filter((release: any) => !release.prerelease)[0];

        if (platformType === "win32") {
            return release.assets.filter((asset: any) => asset.name === `millennium-${release.tag_name}-windows-x86_64.zip`);
        }
        else if (platformType === "linux") {
            return release.assets.filter((asset: any) => asset.name === `millennium-${release.tag_name}-linux-x86_64.tar.gz`);
        }
    },

    GetReleases: async () => { 
        return await GitHub.GetReleaseInfo();
    }
}

export { GitHub };