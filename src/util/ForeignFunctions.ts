import { invoke } from '@tauri-apps/api/tauri';
import { exit } from '@tauri-apps/api/process';
import { exists } from '@tauri-apps/api/fs';
import { IInstaller } from '../logic/Installer';

export enum eInstallMessageType {
    IN_PROGRESS,
    FINISH_SUCCESS,
    FINISH_FAILED,
}

export enum eInstallType {
    INSTALL,
    UNINSTALL,
}

export interface InstallerMessageProps {
    message: string;
    identifier: eInstallMessageType;
}

let g_installerEventListeners: Array<[callback: (message: InstallerMessageProps) => void, type: eInstallType]> = [];

declare global {
    interface Window {
        OnInstallerMessage: (type: eInstallType, message: string, identifier: eInstallMessageType) => void;
    }
}

export const OnInstallerMessage = (type: eInstallType, message: string, identifier: eInstallMessageType) => { 
    g_installerEventListeners.forEach((listener) => {
        if (listener[1] == type) {
            listener[0]({ message, identifier });
        }
    });
}

const Core = {

    IsValidSteamPath: async (inputPath: string) => {
        return await exists(`${inputPath}/steam.exe`)
    },

    ExitEnvironment: () => {
        exit(1).catch((error) => {
            console.error('Failed to exit the app:', error);
        });
    },

    RegisterForInstallerMessages: (callback: (message: InstallerMessageProps) => void) => {
        g_installerEventListeners.push([callback, eInstallType.INSTALL]);
    },

    RegisterForUninstallerMessages: (callback: (message: InstallerMessageProps) => void) => {
        g_installerEventListeners.push([callback, eInstallType.UNINSTALL]);
    },

    StartInstaller: (installType: eInstallType, steamPath: string) => {

        switch (installType) {
            case eInstallType.INSTALL: {
                const installer = new IInstaller(steamPath); 
                installer.StartInstaller();
                break;
            }
            case eInstallType.UNINSTALL: {
                // no-oped for now
                break
            }
        }
    },

    KillSteam: async () => {
        return new Promise((resolve) => {
            setTimeout(() => {
                invoke('close_steam_process').then(() => {
                    console.log('Steam killed.');
                    resolve(true);
                })
                .catch(() => resolve(false))
            }, 1500);
        })
    }

}

export { Core }