import {useEffect, useState} from 'react';
import {motion} from "framer-motion"
import {TitleBar} from '../components/TitleBar';
import {FooterLegend} from '../components/Footer';
import {invoke, path} from '@tauri-apps/api';
import {FormatBytes} from '../util/Bytes';
import CustomCheckbox from '../components/CheckBox';
import {Tooltip} from 'react-tooltip';
import {Core} from '../util/ForeignFunctions';

enum eUninstallType {
	MILLENNIUM,
	CORE,
	PYTHON,
	THEMES,
	PLUGINS
}

interface ComponentSizeProps {
	type: eUninstallType;
	size: number;
	path: string;
	checked?: boolean;
}

interface UninstallingComponentProps { 
	success: boolean;
	errorMessage?: string;
	inProgress: boolean;
	type: eUninstallType;
	path: string;
	excluded: boolean;
}

interface UninstallerStateProps {
	hasFinished: boolean;
	inProgress: boolean;
}

function UninstallerSelectComponents() {

	const [steamPath, setSteamPath] = useState<any>();
	const [propMap, setPropMap] = useState<ComponentSizeProps[]>([]);
	const [uninstallerState, setUninstalling] = useState<UninstallerStateProps>({ hasFinished: false, inProgress: false });
	const [uninstallList, setUninstallList] = useState<UninstallingComponentProps[]>([]);

	const StartUninstall = async () => { 
		setUninstalling({ hasFinished: false, inProgress: true });

		let bufferUninstallList: UninstallingComponentProps[] = propMap.map((component) => ({ 
			success: false, inProgress: true,
			type: component.type, path: component.path,
			errorMessage: String(),excluded: !component.checked
		})
		).filter((component) => !component.excluded);
	
		setUninstallList(bufferUninstallList);
		await Core.KillSteam();

		for (const component of bufferUninstallList) {
			await new Promise(resolve => setTimeout(resolve, 2500)); // add a delay for ux
	
			// const result = await invoke('delete_recursive', { path: await path.resolve(steamPath, component.path)})
			// 	.then((_: any) => ({ success: true }))
			// 	.catch((error: any) => ({ success: false, message: error}));

			const result = { success: true }

			console.log('Uninstall result:', await path.resolve(steamPath, component.path), result);
	
			bufferUninstallList = bufferUninstallList.map((target) => {
				if (target.path === component.path) {
					target.success = result.success;
					target.errorMessage = (result as any).message;
					target.inProgress = false;
				}
				return target;
			});
			setUninstallList(bufferUninstallList);
		}
		setUninstalling({ hasFinished: true, inProgress: false });
	}

	const GetTreeSize = (relativePath: string) => {
		return new Promise((resolve) => {
			invoke('calc_dir_size', { path: relativePath, fromSteamDir: true }).then((size: any) => resolve(size)).catch((_: any) => resolve(0))
		})
	}

	const FetchComponents = () => {
		return new Promise((resolve) => {
			invoke('get_steam_path').then(async (steamPath: any) => {
				setSteamPath(steamPath)

				let artifactPaths = [
					{ path: "user32.dll", id: eUninstallType.MILLENNIUM },
					{ path: "millennium.dll", id: eUninstallType.MILLENNIUM },
					{ path: "ext/data/cache", id: eUninstallType.PYTHON },
					{ path: "ext/data/assets", id: eUninstallType.CORE },
					{ path: "plugins", id: eUninstallType.PLUGINS },
					{ path: "steamui/skins", id: eUninstallType.THEMES },
				]
	
				resolve(artifactPaths)
			});
		})
	}

	const HandleComponentChange = (type: eUninstallType) => {	
		setPropMap(propMap?.map((component) => {
			component.type === type && (component.checked = !component.checked)
			return component;
		}))
	}

	const GetSizeFromType = (type: eUninstallType) => {
		const totalSize = propMap?.filter((component) => component.type === type)?.reduce((total, component) => total + component.size, 0);
		return FormatBytes(totalSize);
	}

	const RenderComponentIsUninstalling = ({name, type}: {name: string, type: eUninstallType}) => {

		const targetComponent = uninstallList?.filter((component) => component.type === type);
		const hasFinished   = targetComponent?.every((component) => !component.inProgress);
		const hasFailed     = targetComponent?.some((component) => !component.success);
		const failedMessage = targetComponent?.map((component) => component.errorMessage);
		const isExcluded	= targetComponent?.some((component) => component.excluded);

		const RenderIsExcluded = () => {
			return (<>
				<svg className='uninstallFinishedCheck' xmlns="http://www.w3.org/2000/svg" xmlnsXlink="http://www.w3.org/1999/xlink" viewBox="0,0,256,256" width="144px" height="144px">
					<g fill="#404145" fillRule="nonzero" stroke="none" strokeWidth={1} strokeLinecap="butt" strokeLinejoin="miter" strokeMiterlimit={10} strokeDashoffset={0} fontFamily="none" fontWeight="none" fontSize="none" textAnchor="none" style={{mixBlendMode: 'normal'}}>
						<g transform="scale(5.33333,5.33333)">
							<path d="M24,4c-11.046,0 -20,8.954 -20,20c0,11.046 8.954,20 20,20c11.046,0 20,-8.954 20,-20c0,-11.046 -8.954,-20 -20,-20zM31.561,29.439c0.586,0.586 0.586,1.535 0,2.121c-0.293,0.294 -0.677,0.44 -1.061,0.44c-0.384,0 -0.768,-0.146 -1.061,-0.439l-5.439,-5.44l-5.439,5.439c-0.293,0.294 -0.677,0.44 -1.061,0.44c-0.384,0 -0.768,-0.146 -1.061,-0.439c-0.586,-0.586 -0.586,-1.535 0,-2.121l5.44,-5.44l-5.439,-5.439c-0.586,-0.586 -0.586,-1.535 0,-2.121c0.586,-0.586 1.535,-0.586 2.121,0l5.439,5.439l5.439,-5.439c0.586,-0.586 1.535,-0.586 2.121,0c0.586,0.586 0.586,1.535 0,2.121l-5.439,5.439z" />
						</g>
					</g>
				</svg>
				<Tooltip anchorSelect={`#component__${eUninstallType[type]}`} place="right-end">
					Component was excluded.
				</Tooltip>

				<p className='isUninstallingProp'>{name}: {GetSizeFromType(type)}</p>
			</>)
		}

		if (targetComponent.length === 0 || isExcluded) {
			return RenderIsExcluded();
		}

		const RenderFinished = () => {

			const FinishFail = () => {
				return (<>
					<svg className='uninstallFinishedCheck' xmlns="http://www.w3.org/2000/svg" xmlnsXlink="http://www.w3.org/1999/xlink" viewBox="0,0,256,256" width="144px" height="144px">
						<g fill="#d90000" fillRule="nonzero" stroke="none" strokeWidth={1} strokeLinecap="butt" strokeLinejoin="miter" strokeMiterlimit={10} strokeDashoffset={0} fontFamily="none" fontWeight="none" fontSize="none" textAnchor="none" style={{mixBlendMode: 'normal'}}>
							<g transform="scale(5.33333,5.33333)">
								<path d="M24,4c-11.046,0 -20,8.954 -20,20c0,11.046 8.954,20 20,20c11.046,0 20,-8.954 20,-20c0,-11.046 -8.954,-20 -20,-20zM31.561,29.439c0.586,0.586 0.586,1.535 0,2.121c-0.293,0.294 -0.677,0.44 -1.061,0.44c-0.384,0 -0.768,-0.146 -1.061,-0.439l-5.439,-5.44l-5.439,5.439c-0.293,0.294 -0.677,0.44 -1.061,0.44c-0.384,0 -0.768,-0.146 -1.061,-0.439c-0.586,-0.586 -0.586,-1.535 0,-2.121l5.44,-5.44l-5.439,-5.439c-0.586,-0.586 -0.586,-1.535 0,-2.121c0.586,-0.586 1.535,-0.586 2.121,0l5.439,5.439l5.439,-5.439c0.586,-0.586 1.535,-0.586 2.121,0c0.586,0.586 0.586,1.535 0,2.121l-5.439,5.439z" />
							</g>
						</g>
					</svg>
					<Tooltip anchorSelect={`#component__${eUninstallType[type]}`} place="right-end">
						{failedMessage?.map((error, index) => (<div key={index}>{error}</div>))}
					</Tooltip>
				</>)
			}

			const FinishSuccess = () => (
				<svg className='uninstallFinishedCheck' xmlns="http://www.w3.org/2000/svg" xmlnsXlink="http://www.w3.org/1999/xlink" viewBox="0,0,256,256" width="144px" height="144px">
					<g fill="#17a401" fillRule="nonzero" stroke="none" strokeWidth={1} strokeLinecap="butt" strokeLinejoin="miter" strokeMiterlimit={10} strokeDashoffset={0} fontFamily="none" fontWeight="none" fontSize="none" textAnchor="none" style={{mixBlendMode: 'normal'}}>
						<g transform="scale(5.33333,5.33333)">
							<path d="M42.96094,8.98047c-0.5196,0.01548 -1.01276,0.23264 -1.375,0.60547l-24.58594,24.58594l-10.58594,-10.58594c-0.50163,-0.52247 -1.24653,-0.73294 -1.94741,-0.55022c-0.70088,0.18271 -1.24822,0.73006 -1.43094,1.43094c-0.18271,0.70088 0.02775,1.44578 0.55022,1.94741l12,12c0.78106,0.78074 2.04706,0.78074 2.82812,0l26,-26c0.59152,-0.57498 0.76938,-1.45413 0.44787,-2.21383c-0.32151,-0.75969 -1.07643,-1.24408 -1.90099,-1.21977z" />
						</g>
					</g>
				</svg>
			);

			return hasFailed ? <FinishFail/> : <FinishSuccess/>	
		}

		const RenderUninstallingSpinner = () => (
			<svg className="uninstalling-spinner spinner">
				<circle>
					<animateTransform attributeName="transform" type="rotate" values="-90;810" keyTimes="0;1" dur="2s" repeatCount="indefinite"></animateTransform>
					<animate attributeName="stroke-dashoffset" values="0%;0%;-157.080%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
					<animate attributeName="stroke-dasharray" values="0% 314.159%;157.080% 157.080%;0% 314.159%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
				</circle>
			</svg>
		)

		return (<>
			{ hasFinished ? <RenderFinished/> : <RenderUninstallingSpinner/> }
			<p className='isUninstallingProp'>{name}: {GetSizeFromType(type)}</p>
		</>)
	}

	const RenderComponentSize = ({name, type}: {name: string, type: eUninstallType}) => {
		const localPaths = propMap?.filter((component) => component.type === type)?.map((component) => component.path);
		const isChecked  = propMap?.filter((component) => component.type === type)?.every((component) => component.checked);

		return (
			<div className='check' id={`component__${eUninstallType[type]}`}>
			{
				uninstallerState.inProgress || uninstallerState.hasFinished ? 
				<RenderComponentIsUninstalling name={name} type={type}/> :
				<>
					<CustomCheckbox isChecked={isChecked} onChange={_ => HandleComponentChange(type)}> <p>{name}: {GetSizeFromType(type)}</p> </CustomCheckbox>
					<Tooltip anchorSelect={`#component__${eUninstallType[type]}`} place="right-end"> {localPaths?.map((path) => (<div key={path}>{`${steamPath.replace(/\\/g, "/")}/${path}`}</div>))} </Tooltip>
				</>
			}
			</div>
		)
	}

	const CalculateMillenniumSize = async () => {
		const targetComponents: any = await FetchComponents()
		Promise.all(targetComponents.map(async (component: any) => ({ 
			type: component.id, 
			size: await GetTreeSize(component.path),
			path: component.path,
			checked: true // default to checked
		})))

		.then((sizes: ComponentSizeProps[]) => {
			setPropMap(sizes.sort((a, b) => a.type - b.type));
		})
	}

	const CalculateSelectedSize = () => propMap?.reduce((total, component) => component.checked ? total + component.size : total, 0)

	useEffect(() => { CalculateMillenniumSize() }, [])

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
							<RenderComponentSize name={"Millennium"} type={eUninstallType.MILLENNIUM}/>
							<RenderComponentSize name={"Custom Steam Components"} type={eUninstallType.CORE}/>
							<RenderComponentSize name={"Dependencies"} type={eUninstallType.PYTHON}/>
							<RenderComponentSize name={"Themes"} type={eUninstallType.THEMES}/>
							<RenderComponentSize name={"Plugins"} type={eUninstallType.PLUGINS}/>
						</div>
							
                        <div className='hor-seperator'></div>

                        <div><b>Uninstalling from: </b> {steamPath}</div>
                        <div style={{marginTop: "4px"}}><b>Reclaimed Disk Space: </b> 
							{FormatBytes(CalculateSelectedSize())}
						</div>

                    </motion.div>
                </div>
            </div>
			<FooterLegend> 
				{
					uninstallerState.hasFinished ? 
					<button id="install" className={`btn next`} onClick={Core.ExitEnvironment}>Exit</button>
					:
					<button id="install" className={`btn next`} onClick={_ => !uninstallerState.inProgress && StartUninstall()}>{
						uninstallerState.inProgress ? 
						<svg className="uninstalling-spinner spinner">
							<circle>
								<animateTransform attributeName="transform" type="rotate" values="-90;810" keyTimes="0;1" dur="2s" repeatCount="indefinite"></animateTransform>
								<animate attributeName="stroke-dashoffset" values="0%;0%;-157.080%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
								<animate attributeName="stroke-dasharray" values="0% 314.159%;157.080% 157.080%;0% 314.159%" calcMode="spline" keySplines="0.61, 1, 0.88, 1; 0.12, 0, 0.39, 0" keyTimes="0;0.5;1" dur="2s" repeatCount="indefinite"></animate>
							</circle>
						</svg>
						: 'Uninstall'	
					}</button>
				}
			</FooterLegend>
        </div>
		</>
	)
}
  
export default UninstallerSelectComponents 
