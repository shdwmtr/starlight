import { writeTextFile, createDir } from '@tauri-apps/api/fs';
import { invoke, path } from '@tauri-apps/api';
import { eInstallMessageType, eInstallType, OnInstallerMessage } from '../components/ForeignFunctions';

export class IInstaller {

    steamPath: string;
    API_URL: string = 'https://api.github.com/repos/SteamClientHomebrew/Millennium/releases';

    constructor(steamPath: string) {
        console.log('Steam path:', steamPath);
        this.steamPath = steamPath;
    }

    async LogProgress(message: string) {
        OnInstallerMessage(eInstallType.INSTALL, message, eInstallMessageType.IN_PROGRESS);
    }

    async DownloadFile(url: string, path: string, fileName: string) {
        return new Promise(async (resolve) => {
            this.LogProgress(`Downloading file ${fileName}`);
            console.log('Downloading file:', fileName);

            try {       
                setTimeout(() => {
                    invoke('download_file_to_path', { url: url, path: `${path}/${fileName}` }).then((response: any) => {
                        console.log('Response from download:', response);
                        resolve(response);
                    })
                }, 3500); // ux delay
            } 
            catch (error) {
                resolve(false);
            }
        })
    }

    async InstallerFinished(downloadLog: Array<any>) {
        const allDownloadsSuccessful = downloadLog.every((log) => log.success);

        if (allDownloadsSuccessful) {
            OnInstallerMessage(eInstallType.INSTALL, 'All downloads completed successfully.', eInstallMessageType.FINISH_SUCCESS);
        } 
        else {
            const failedDownloads = downloadLog.filter((log) => !log.success);
            const failedDownloadNames = failedDownloads.map((log) => log.name);
            OnInstallerMessage(eInstallType.INSTALL, `Failed to download: ${failedDownloadNames.join(', ')}`, eInstallMessageType.FINISH_FAILED);
        }

        const parentPath = await path.resolve(this.steamPath, "ext", "logs");
        const logFile = await path.resolve(parentPath, "fs_log.log");

        const bufferLogData = {
            downloads: downloadLog,
            path: this.steamPath
        }

        await createDir(parentPath, { recursive: true });
        await writeTextFile(logFile, JSON.stringify(bufferLogData, null, 4));
    }

    async GetLatestRelease() {
        const response = await fetch(this.API_URL)
        const releases = await response.json();

        return releases.filter((release: any) => !release.prerelease)[0];
    }

    async StartInstaller() {
        try { 
            this.LogProgress(`Fetching releases from GitHub.`)

            const latestRelease = await this.GetLatestRelease();
            const fileQueryList = latestRelease.assets.map((asset: any) => ({ url: asset.browser_download_url, name: asset.name }));

            const downloadLog = []
            for (const file of fileQueryList) {
                downloadLog.push({ 
                    name: file.name, success: await this.DownloadFile(file.url, this.steamPath, file.name) 
                });
            }
            
            this.InstallerFinished(downloadLog)
        } 
        catch (error) {
            console.error('Error fetching releases:', error);
        }
    }
}