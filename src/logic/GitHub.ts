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
        const releases = await GitHub.GetReleaseInfo();
        return releases.filter((release: any) => !release.prerelease)[0];
    },

    GetReleases: async () => { 
        return await GitHub.GetReleaseInfo();
    }
}

export { GitHub };