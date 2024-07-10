import { useState, useEffect } from 'react';
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import { Core, InstallerMessageProps, eInstallMessageType, eInstallType } from '../components/ForeignFunctions';
import { FooterLegend } from '../components/Footer';

interface InstallerFinishedProps {
    installSuccess: boolean;
    strFailedMessage?: string;
}

const InstallCoreFinished: React.FC<InstallerFinishedProps> = ({ installSuccess, strFailedMessage }) => {

    const InstallSuccess = () => {
        return (
            <div className="container" id="install-success">
                <div className='header'>Successfully Removed! Thanks for trying Millennium 💖</div>
                <div className='description'>You're themes and presets haven't been touched 😊. We're sorry to see you go.</div>
            </div>
        )
    }

    const InstallFailed = () => {
        return (
            <div className="container" id="install-success">
                <div className='header'>❌ Failed to uninstall Millennium</div>
                <div className='description'>{strFailedMessage}</div>
            </div> 
        )
    }

    return installSuccess ? <InstallSuccess /> : <InstallFailed />
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
                <div className='installer-header'>Uninstalling Millennium.</div>
                <div className='installer-text'>{status}</div>
            </div>
        </motion.div>
    )
}

function UninstallPanel() {
    
    const [strProgressMessage, setStatus] = useState("Starting Uninstaller...");
    const [isInstalling, setInstallStatus] = useState(true);
    const [hasFailed, setFailed] = useState<any>({});

    Core.RegisterForInstallerMessages((message: InstallerMessageProps) => {

        switch (message.identifier) {
            case eInstallMessageType.FINISH_SUCCESS: {
                setInstallStatus(false)
                break;
            }
            case eInstallMessageType.FINISH_FAILED:{
                setFailed({fail: true, message: message.message})
                break;
            }
            case eInstallMessageType.IN_PROGRESS: {
                setStatus(message.message)
                break;
            }
        }
    });

    /** Start the installer when the component mounts */
    //useEffect(() => Core.StartInstaller(eInstallType.UNINSTALL), [])

    return (
        <>
            <TitleBar bCanGoBack={true} bIsBusy={isInstalling}/>
            <div className="content-box" id='installer-pane'>
            {
                isInstalling ? 
                <InstallerInProgress status={strProgressMessage}/> 
                : 
                <>
                    <InstallCoreFinished installSuccess={hasFailed?.fail} strFailedMessage={hasFailed?.message}/>
                    <FooterLegend>
                        <button id="install" className="btn next" onClick={Core.ExitEnvironment}>Finish</button>
                    </FooterLegend>
                </>
            }       
            </div>
        </>
    )
  }
  
  export default UninstallPanel 