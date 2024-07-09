import { invoke } from '@tauri-apps/api/tauri';
import { exit } from '@tauri-apps/api/process';
import { exists } from '@tauri-apps/api/fs';
import { IInstaller } from '../logic/Installer';
import { StartUninstallerCore } from '../logic/Uninstaller';

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
        exit().catch((error) => {
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
                StartUninstallerCore();
                break
            }
        }
    },
}

export { Core }