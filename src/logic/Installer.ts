import { invoke } from '@tauri-apps/api';
import { removeFile } from "@tauri-apps/api/fs";
import { Core, eInstallMessageType, eInstallType, EmitInstallerMessage } from '../util/ForeignFunctions';
import { GitHub } from './GitHub';

export class IInstaller {

    steamPath: string;

    constructor(steamPath: string) {
        console.log('Steam path:', steamPath);
        this.steamPath = steamPath;
    }

    async LogProgress(message: string) {
        EmitInstallerMessage(eInstallType.INSTALL, message, eInstallMessageType.IN_PROGRESS);
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

    async DeleteArchive(filePath: string) {
        try {
            await removeFile(filePath);
            console.log(`File deleted successfully: ${filePath}`);
            return true;
        } catch (error) {
            console.error(`Failed to delete file: ${filePath}`, error);
            return false;
        }
    }

    async ExtractDownloadedFiles(releaseName: string) {
        try {
            await invoke("extract_zip", { zipPath: [this.steamPath, releaseName].join("/"), destDir: this.steamPath });
            return true;
        } 
        catch (error) {
            console.error("Failed to extract zip file:", error);
            return false;
        }
    }

    async StartInstaller() {

        this.LogProgress(`Closing Steam...`)
        await Core.KillSteam()

        try { 
            this.LogProgress(`Fetching releases from GitHub.`)

            const latestRelease = (await GitHub.GetLatestRelease())[0];
            const downloadSuccess = await this.DownloadFile(latestRelease.browser_download_url, this.steamPath, latestRelease.name) 

            if (!downloadSuccess) {
                EmitInstallerMessage(eInstallType.INSTALL, `Failed to download: ${latestRelease.name}`, eInstallMessageType.FINISH_FAILED);
                return;
            }
            
            const extractSuccess = await this.ExtractDownloadedFiles(latestRelease.name);

            if (!extractSuccess) {
                EmitInstallerMessage(eInstallType.INSTALL, `Failed to extract: ${latestRelease.name}`, eInstallMessageType.FINISH_FAILED);
                return;
            }

            const deleteSuccess = await this.DeleteArchive([this.steamPath, latestRelease.name].join("/"));

            if (!deleteSuccess) {
                EmitInstallerMessage(eInstallType.INSTALL, `Failed to delete: ${latestRelease.name}`, eInstallMessageType.FINISH_FAILED);
                return;
            }

            EmitInstallerMessage(eInstallType.INSTALL, 'All downloads completed successfully.', eInstallMessageType.FINISH_SUCCESS);
        } 
        catch (error) {
            console.error('Error fetching releases:', error);
        }
    }
}