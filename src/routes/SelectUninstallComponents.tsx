import { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import { FooterLegend } from '../components/Footer';
import { invoke } from '@tauri-apps/api';
import { FormatBytes } from '../components/Bytes';
import CustomCheckbox from '../components/CheckBox';
import { Tooltip } from 'react-tooltip';

function UninstallerSelectComponents() {
    
	const StartUninstall = async () => { 


	}

	const [totalSize, setTotalSize] = useState<any>();
	const [pythonSize, setPythonSize] = useState<any>();
	const [coreSize, setCoreSize] = useState<any>();
	const [millenniumSize, setMillenniumSize] = useState<any>();
	const [steamPath, setSteamPath] = useState<any>();
	const [themesSize, setThemesSize] = useState<any>();
	const [pluginsSize, setPluginSize] = useState<any>();

	useEffect(() => {
		invoke('get_steam_path').then((path: any) => setSteamPath(path));

		Promise.all([
			invoke('calc_dir_size', { path: "user32.dll", fromSteamDir: true }), 
			invoke('calc_dir_size', { path: "python311.dll", fromSteamDir: true }),
			invoke('calc_dir_size', { path: "ext", fromSteamDir: true }),
			invoke('calc_dir_size', { path: "steamui/skins", fromSteamDir: true }),

			invoke('calc_dir_size', { path: "ext/data/cache", fromSteamDir: true }),
			invoke('calc_dir_size', { path: "ext/data/assets", fromSteamDir: true }),
			invoke('calc_dir_size', { path: "plugins", fromSteamDir: true })
		])
		.then(([user32, python311, extSize, skinsSize, pythonBin, coreSize, pluginsSize]: any) => { 
			setMillenniumSize(user32 + python311)
			setTotalSize(extSize + user32 + python311 + skinsSize + pluginsSize)

			setThemesSize(skinsSize)
			setPythonSize(pythonBin)
			setCoreSize(coreSize)
			setPluginSize(pluginsSize)
		})
	}, [])

	enum eUninstallType {
		CORE,
		PYTHON,
		MILLENNIUM,
		THEMES,
		PLUGINS
	}

	const uninstallTypes = new Map<eUninstallType, any>([
		[eUninstallType.CORE, coreSize],
		[eUninstallType.PYTHON, pythonSize],
		[eUninstallType.MILLENNIUM, millenniumSize],
		[eUninstallType.THEMES, themesSize],
		[eUninstallType.PLUGINS, pluginsSize]
	]);


	const [uninstallSize, setUninstallSize] = useState<Array<eUninstallType>>(
		Object.keys(eUninstallType)
    		.filter(key => isNaN(Number(key)))
    		.map(key => eUninstallType[key as keyof typeof eUninstallType]
	));

	const HandleComponentChange = (type: eUninstallType) => {
		
		if (uninstallSize?.includes(type)) {
			setUninstallSize(uninstallSize.filter((item) => item != type))
		}
		else {
			setUninstallSize([...uninstallSize ?? [], type]) 
		}
	}

	return (
		<>
		<TitleBar bCanGoBack={true} bIsBusy={false}/>

		<div className="content-box" id='installer-pane'>
            <div className="container uninstall-container" >
                <div className='options'>
                    <motion.div className='header' initial={{ y: "-100%" }} animate={{ y: 0 }}>
                        Uninstalling Millennium ✨
                    </motion.div>
                    <motion.div initial={{ opacity: 0, scale: 0.5 }} animate={{ opacity: 1, scale: 1 }} transition={{ duration: 0.4, delay: 0.8, ease: [0, 0.71, 0.2, 1.01] }} className='description'>
                        
                        <b>Select what to uninstall from the components below</b>

						<div className='select-uninstall-components'>

							<div className='check' id='coreComponents'>
								<CustomCheckbox isChecked={true} onChange={_ => HandleComponentChange(eUninstallType.CORE)}>
									<p>Core Components: {FormatBytes(coreSize)}</p>
								</CustomCheckbox>
							</div>
							<Tooltip anchorSelect="#coreComponents" place="right-end">{steamPath}\ext\data\assets</Tooltip>

							<div className='check' id='pythonComp'>
								<CustomCheckbox isChecked={true} onChange={_ => HandleComponentChange(eUninstallType.PYTHON)}>
									<p>Python Dependencies: {FormatBytes(pythonSize)}</p>
								</CustomCheckbox>
							</div>
							<Tooltip anchorSelect="#pythonComp" place="right-end">{steamPath}\ext\data\cache</Tooltip>

							<div className='check' id='millenniumComp'>
								<CustomCheckbox isChecked={true} onChange={_ => HandleComponentChange(eUninstallType.MILLENNIUM)}>
									<p>Millennium: {FormatBytes(millenniumSize)}</p>
								</CustomCheckbox>
							</div>
							<Tooltip anchorSelect="#millenniumComp" place="right-end">{steamPath}\user32.dll,<br/>{steamPath}\python311.dll</Tooltip>

							<div className='check' id='themesComp'>
								<CustomCheckbox isChecked={true} onChange={_ => HandleComponentChange(eUninstallType.THEMES)}>
									<p>Themes: {FormatBytes(themesSize)}</p>
								</CustomCheckbox>
							</div>
							<Tooltip anchorSelect="#themesComp" place="right-end">{steamPath}\steamui\skins</Tooltip>

							<div className='check' id='pluginsComp'>
								<CustomCheckbox isChecked={true} onChange={_ => HandleComponentChange(eUninstallType.PLUGINS)}>
									<p>Plugins: {FormatBytes(pluginsSize)}</p>
								</CustomCheckbox>
							</div>
							<Tooltip anchorSelect="#pluginsComp" place="right-end">{steamPath}\plugins</Tooltip>
						</div>
							

                        <div className='hor-seperator'></div>

                        <b>Uninstalling from: </b> {steamPath}
						<br/>
                        <b>Freeing: </b>

						{FormatBytes(uninstallSize?.reduce((total: any, type) => total + uninstallTypes.get(type), 0))}


                    </motion.div>
                </div>
            </div>
			<FooterLegend> 
				<button id="install" className={`btn next`} onClick={StartUninstall}>Uninstall</button>
			</FooterLegend>
        </div>
		</>
	)
}
  
export default UninstallerSelectComponents 