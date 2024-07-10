import { useState, useEffect } from 'react';
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import { Core, InstallerMessageProps, eInstallMessageType, eInstallType } from '../components/ForeignFunctions';
import { FooterLegend } from '../components/Footer';
import { invoke } from '@tauri-apps/api';

interface InstallerFinishedProps {
    installFailed: boolean;
    strFailedMessage?: string;
}

const InstallCoreFinished: React.FC<InstallerFinishedProps> = ({ installFailed, strFailedMessage }) => {

    const InstallSuccess = () => {
        Core.ExitEnvironment()
        return (<></>)
    }

    const InstallFailed = () => {
        return (
            <div className="container" id="install-success">
                <div className='header'>❌ &nbsp; Failed to install Millennium</div>
                <div className='description'>{strFailedMessage}</div>
            </div> 
        )
    }

    return installFailed ? <InstallFailed /> : <InstallSuccess />
}

const InstallerInProgress = ({ status }: { status: string }) => { 
    return (
        <motion.div transition={{ delay: .5, }} initial={{ y: "-100vh" }} animate={{ y: 0 }} className='installer-box'>
            <svg className="spinner">
                <circle>
                    <animateTransform attributeName="transform" type="rotate" values="-90;810" keyTimes="0;1" dur="2s" repeatCount="indefinite"></animateTransform>
                    <animate attributeName="stroke-dashoffset" values="0%;0%;-157.080%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
                    <animate attributeName="stroke-dasharray" values="0% 314.159%;157.080% 157.080%;0% 314.159%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
                </circle>
            </svg>
            <div className='installer-details'>
                <div className='installer-header'>Installing Millennium.</div>
                <div className='installer-text'>{status}</div>
            </div>
        </motion.div>
    )
}

function InstallerPanel() {
    
    const [strProgressMessage, setStatus] = useState("Starting Installer...");
    const [isInstalling, setInstallStatus] = useState(true);
    const [hasFailed, setFailed] = useState<any>({});

    Core.RegisterForInstallerMessages((message: InstallerMessageProps) => {

        switch (message.identifier) {
            case eInstallMessageType.FINISH_SUCCESS: {
                setFailed({ fail: false })
                setInstallStatus(false)
                break;
            }
            case eInstallMessageType.FINISH_FAILED: {
                setFailed({ fail: true, message: message.message })
                setInstallStatus(false)
                break;
            }
            case eInstallMessageType.IN_PROGRESS: {
                setStatus(message.message)
                break;
            }
        }
    });

    useEffect(() => {
        console.log('Starting Installer...')
        invoke('get_steam_path').then((steamPath: any) => {
            Core.StartInstaller(eInstallType.INSTALL, steamPath)
        })
    }, [])

    return (
        <>
            <div data-tauri-drag-region className="titlebar">
                <div className="closebtn-container" onClick={Core.ExitEnvironment}>
                    <img className="closebtn" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAACXBIWXMAAAsTAAALEwEAmpwYAAABJUlEQVR4nO2Y2wqCUBBF7Qe7UfRQ315QJEWXHzBYIRQFWuk5Z8aRZr31smev9OhUljmO4zhSAHPgDOyBseLcCXAArsAiNGQAnHhRBIe1m7t4zHpyKbuEhh3fgsQlqJYvOcYETmsCRSSoL19+nsYGz2qCb8Aq8VkramYsUw0Qk0C6vKQEWuUlJNTLp5TorHwKic7Lx0iYKR8iYa58Gwmz5ZtImC/fYBVQWUUkr4Ttb76FhP3yJR/u+crB7lt5+xL0+RDz5VGp8XsiiibPebMStHhJmZMg4A1rRoKI9aBzCRLsNp1JkHAxU5dAYKtES0JyJUZaQmOfR0oCGBn4a3EUE7rTKP9DYpuFAuRa5b9I5FnkLbQF1lGXMmzu5jF7qDXXcRznD7kDEjZgdJknwowAAAAASUVORK5CYII="/>
                </div>
            </div>
            <div className="content-box auto-installer" id='installer-pane'>
            { isInstalling ?  <InstallerInProgress status={strProgressMessage}/> : <InstallCoreFinished installFailed={hasFailed.fail} strFailedMessage={hasFailed.message}/> }       
            </div>
        </>
    )
  }
  
  export default InstallerPanel 