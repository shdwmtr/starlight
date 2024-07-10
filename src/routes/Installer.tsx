import { useState, useEffect } from 'react';
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import { Core, InstallerMessageProps, eInstallMessageType } from '../util/ForeignFunctions';
import { FooterLegend } from '../components/Footer';

interface InstallerFinishedProps {
    installFailed: boolean;
    strFailedMessage?: string;
}

const InstallCoreFinished: React.FC<InstallerFinishedProps> = ({ installFailed, strFailedMessage }) => {

    const InstallSuccess = () => {
        return (
            <div className="container" id="install-success">
                <div className='header'>You're all set! Thanks for using Millennium 💖</div>
                <div className='description'>If you're new here, see further instructions when Steam® starts.</div>
            </div>
        )
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

    return (
        <>
            <TitleBar bCanGoBack={true} bIsBusy={isInstalling}/>
            <div className="content-box" id='installer-pane'>
            {
                isInstalling ? 
                <InstallerInProgress status={strProgressMessage}/> 
                : 
                <>
                    <InstallCoreFinished installFailed={hasFailed.fail} strFailedMessage={hasFailed.message}/>
                    <FooterLegend>
                        <button id="install" className="btn next" onClick={Core.ExitEnvironment}>Finish</button>
                    </FooterLegend>
                </>
            }       
            </div>
        </>
    )
  }
  
  export default InstallerPanel 