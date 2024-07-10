import { useState, useEffect } from 'react';
import { NavigateFunction, useNavigate } from 'react-router-dom';
import { Tooltip } from 'react-tooltip'
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import TimestampConverter from '../components/TimeStamp';
import { invoke } from '@tauri-apps/api';
import { open } from '@tauri-apps/api/dialog';
import { Core, eInstallType } from '../components/ForeignFunctions';
import { FormatBytes } from '../components/Bytes';
import { GitHub } from '../logic/GitHub';

const RenderEvaluatedFiles = ({ assets }: { assets: any }) => { 

    return (
        <ul>
            {/* Render all asset size */}
            {assets.map((asset: any) => { return (<li>{asset?.name} ({FormatBytes(asset.size)})</li>) })}
            {/* Calculate total download size from all assets */}
            <li>Total download size: ({ FormatBytes(assets.reduce((totalSize: number, asset: any) => totalSize + asset.size, 0)) }) </li>
        </ul>
    )
}

const RenderDownloadCount = ({ details }: { details: any }) => {

    const CalculateCount = () => {
        return details?.reduce((totalDownloads: any, release: any) => {
            return totalDownloads + release?.assets?.reduce((assetCount: any, asset: any) => {
                return assetCount + (asset.download_count || 0);
            }, 0)
        }, 0)
    }

    return (<>{` ${CalculateCount()} `}</>)
}

const RenderFooterLegend = ({ history, steamPath }: { history: NavigateFunction, steamPath: string }) => {

    const NavigateToInstaller = () => { 
        Core.StartInstaller(eInstallType.INSTALL, steamPath)
        history("/run-install");
    }

    return (
        <motion.div initial={{ y: "100%" }} animate={{ y: 0 }} style={{ position: "absolute", bottom: 0, left: 0, right: 0 }} transition={{ delay: 0.2 }} className="footer-content">        
            <p className="tooltip">
                <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAACXBIWXMAAAsTAAALEwEAmpwYAAACjklEQVR4nO1ZyW4TQRAdARduLCe2E/AHbDd/AFI89SpdZZL/YD04h+BciQDxD2xCJArfAcoNLiwHUGKuOFyCHrFZZEUznmnPDMJPKsmS5Zn3uqq7qp+TZIYZZiiNbrd7QMSuiPptwJ4JbBOwrwL7zhh+3hT1p4DdUu1c5m+SuiGyeFrUVgT+Ceq7k4V9FPVeCOFU5cTTND0O+COB70xOfCwGgD8MIRyrhLzMWwfq2xGI/x2wLdVgUyPearUOcdWjE9cxIQ/4rqjkQwiHAVubOnndC1F/yXfGIn8Qas+rIo/fmViLkolKykb3y4Tdj7FhayGPkQjpzBciz2ONJ0PdAqC+zWO7htKxdTYpNjvANiotJWGHLdmk+IzR89ptP1MyC4O5uc7J/ALUVsrXblQBu4DdzX1sFpttxl64QRF75O1V6QWBv881AHKqLE1+SpGGcClTAOB36iaK/bKgfiNbQB1dV/OGPckuIV5GYqwW/G2a+lkGP0cRAH+TQ4D3I6W79+uZ6r04Amwrj4CdSAKW/hCwFKmMBv+FgH5jBSBXCUXaxFMR4NmbmLZIgwU8zs4AvZ3GCrDrmQJoOjVVgGrnQqYADkxQ+9A0AZJ3mIvVeKILUFvORf7nC3mLomPWHAGDiS40BO2+xowSsNWkmP9Z/FLPAS6EcA64dh5q74oLsC9XFxaOJkVAr7Js7ZaNVB1JGdCrrIu8qN1LoliLkbpzLdYiQaOVhmuF5F9EM3dH4GrQYKqA/CqznkwL9Cp5MsQnb59Lb9i84LE2zMag9EaFfeOqt9vtI0nVYHekY1ZkduJsI2rLIosnkrrBIYumE+A3aX3w4sGbHa+nw+gL7DW/o7cjYhcb8TfrDDMk/z5+AHLrqx1z8KkFAAAAAElFTkSuQmCC"/>
                Millennium is not affiliated with Steam®, Valve, or any of their partners
            </p>
            <div className="flex-box">
                <div className='info-container'>
                    <a id="DiscordLink" target='_blank' rel="noreferrer noopener" href="https://steambrew.app/discord" className="social-icon discord">
                        <svg className="social-icon-inner" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 23 23" width="26" height="26">
                            <path fill="currentColor" fillRule="evenodd" d="M20.222 0c1.406 0 2.54 1.137 2.607 2.475V24l-2.677-2.273l-1.47-1.338l-1.604-1.398l.67 2.205H3.71c-1.402 0-2.54-1.065-2.54-2.476V2.48C1.17 1.142 2.31.003 3.715.003h16.5L20.222 0zm-6.118 5.683h-.03l-.202.2c2.073.6 3.076 1.537 3.076 1.537c-1.336-.668-2.54-1.002-3.744-1.137c-.87-.135-1.74-.064-2.475 0h-.2c-.47 0-1.47.2-2.81.735c-.467.203-.735.336-.735.336s1.002-1.002 3.21-1.537l-.135-.135s-1.672-.064-3.477 1.27c0 0-1.805 3.144-1.805 7.02c0 0 1 1.74 3.743 1.806c0 0 .4-.533.805-1.002c-1.54-.468-2.14-1.404-2.14-1.404s.134.066.335.2h.06c.03 0 .044.015.06.03v.006c.016.016.03.03.06.03c.33.136.66.27.93.4a8.18 8.18 0 0 0 1.8.536c.93.135 1.996.2 3.21 0c.6-.135 1.2-.267 1.8-.535c.39-.2.87-.4 1.397-.737c0 0-.6.936-2.205 1.404c.33.466.795 1 .795 1c2.744-.06 3.81-1.8 3.87-1.726c0-3.87-1.815-7.02-1.815-7.02c-1.635-1.214-3.165-1.26-3.435-1.26l.056-.02zm.168 4.413c.703 0 1.27.6 1.27 1.335c0 .74-.57 1.34-1.27 1.34c-.7 0-1.27-.6-1.27-1.334c.002-.74.573-1.338 1.27-1.338zm-4.543 0c.7 0 1.266.6 1.266 1.335c0 .74-.57 1.34-1.27 1.34c-.7 0-1.27-.6-1.27-1.334c0-.74.57-1.338 1.27-1.338z"></path>
                        </svg>
                    </a>
                    <Tooltip anchorSelect="#DiscordLink" place='top'>Join Discord</Tooltip>
                    <a id="GithubLink" target='_blank' rel="noreferrer noopener" href="https://github.com/SteamClientHomebrew/Millennium" className="social-icon github">
                        <svg className="social-icon-inner" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" width="26" height="26">
                            <path fill="currentColor" fillRule="evenodd" d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68 0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.013 8.013 0 0016 8c0-4.42-3.58-8-8-8z"></path>
                        </svg>
                    </a>
                    <Tooltip anchorSelect="#GithubLink" place='top'>View Source Code</Tooltip>
                </div>
                <button id="install" className="btn next" onClick={NavigateToInstaller}>Install</button>
            </div>
        </motion.div>   
    )
}

const IsLoadingComponent = () => {
    return (
        <div className="content-box" id='installer-pane'>      
            <div className='installer-box'>
            <svg className="spinner">
                <circle>
                    <animateTransform attributeName="transform" type="rotate" values="-90;810" keyTimes="0;1" dur="2s" repeatCount="indefinite"></animateTransform>
                    <animate attributeName="stroke-dashoffset" values="0%;0%;-157.080%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
                    <animate attributeName="stroke-dasharray" values="0% 314.159%;157.080% 157.080%;0% 314.159%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
                </circle>
            </svg>
            <div className='installer-details'>
                <div className='installer-header'>One Moment...</div>
            </div>
            </div> 
        </div>
    )

}

interface InstallerViewModelProps {
    HistoryRouter: NavigateFunction;
    details: any;
    steamPath: string;
}

const ReleaseInfoHasMounted: React.FC<InstallerViewModelProps> = ({ HistoryRouter, details, steamPath }) => {

    const releaseInfo = details[0];
    const [steamPathBuffer, setSteamPath] = useState(steamPath);

    const BrowseSteamDirectory = async () => {
        const selectedPath = await open({
            multiple: false,
            directory: true
        });
        
        if (selectedPath) {
            const isValidPath = await Core.IsValidSteamPath(selectedPath as string);

            if (!isValidPath) {
                alert("Invalid Steam Path, please select a viable directory.")
                return;
            }
            setSteamPath(selectedPath as string);
        } 
    }

    return (
        <div className="content-box" id='installer-pane'>
            <div className="container uninstall-container" >
                <div className='options'>
                    <motion.div className='header' initial={{ y: "-100%" }} animate={{ y: 0 }}>
                        Install Millennium v{releaseInfo?.tag_name} ✨
                    </motion.div>
                    <motion.div initial={{ opacity: 0, scale: 0.5 }} animate={{ opacity: 1, scale: 1 }} transition={{ duration: 0.4, delay: 0.8, ease: [0, 0.71, 0.2, 1.01] }} className='description'>
                        
                        Released: <TimestampConverter timestamp={releaseInfo?.published_at} /> 
                        {/* • <RenderDownloadCount details={details}/> Downloads<br /> */}

                        <div className='hor-seperator'></div>

                        <b>Steam Path:</b>
                        <br/>

                        <div className='steampath-entry'>
                            <input className='steampath-textentry' value={steamPathBuffer}></input>
                            <button className='steampath-browse' onClick={BrowseSteamDirectory}>...</button>
                        </div>
                        <Tooltip anchorSelect=".steampath-browse" place="bottom-end">Browse Files</Tooltip>

                        {/* <ReactMarkdown rehypePlugins={[rehypeRaw]} children={releaseInfo?.body}/> */}
                        <br/><br/>

                        Evaluated files for install
                        <RenderEvaluatedFiles assets={releaseInfo?.assets}/>
                    </motion.div>
                </div>
            </div>
            <RenderFooterLegend steamPath={steamPathBuffer} history={HistoryRouter}/>
        </div>
    )
}

function InstallerViewModel() {

    const HistoryRouter = useNavigate();
    const [details, setDetails] = useState<{ details: any; steamPath: any; }>(null as any);

    const FetchLatestReleaseInfo = () => {
        return new Promise(async (resolve) => {
            resolve(await GitHub.GetReleaseInfo())
        })
    }

    const FetchSteamPath = () => {
        return new Promise((resolve: (value: string) => void) => {
            invoke('get_steam_path').then((path: any) => {

                const steamPath: string = path as string;
                resolve(steamPath)
            })
        })
    }

    const FetchComponents = () => {
        Promise.all([FetchLatestReleaseInfo(), FetchSteamPath()]).then(([releaseInfo, steamPath]) => {
            setDetails({ details: releaseInfo, steamPath: steamPath })
        })
    }

    // Fetch latest release info on component mount
    useEffect(() => FetchComponents(), [])

    return (<>
        <TitleBar bCanGoBack={true} bIsBusy={false}/>
        {   
            details === null ? 
            <IsLoadingComponent/> : <ReleaseInfoHasMounted HistoryRouter={HistoryRouter} details={details?.details} steamPath={details?.steamPath}/>
        }    
    </>)
  }
  
  export default InstallerViewModel 