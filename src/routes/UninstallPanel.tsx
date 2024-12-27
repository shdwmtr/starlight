import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { motion } from "framer-motion"
import { TitleBar } from '../components/TitleBar';
import { FooterLegend } from '../components/Footer';

const SubmitUninstallInformation = async (tags: any) => { 
	return new Promise(async (resolve, reject) => {
		try {
			const webHookUrl = import.meta.env.VITE_UNINSTALL_LOGS_WEBHOOK_URL!;
			const webhookResponse = await fetch(webHookUrl, {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					content: 'user uninstalling reason: ' + tags.join(', ')
				}),
			});
			webhookResponse.ok ? resolve(true) : reject(false);
		} 
		catch (error: any) {
			reject(false);
		}
	})
}

function UninstallerViewModal() {
    
	const HistoryNavigator = useNavigate();
	const [tagCollection, setTags] = useState<any>([]);

	const ToggleTagState = (event: any, tag: string) => {

		const hasToggled = event.target.classList.toggle('selected')
		setTags(hasToggled ? [...tagCollection, tag] : tagCollection.filter((item: any) => item != tag))
	}

	const StartUninstall = async () => { 
		if (!tagCollection.length) {
			return;
		}

		SubmitUninstallInformation(tagCollection).finally(() => {
			HistoryNavigator("/uninstall-components")
		})
	}

	return (
		<>
		<TitleBar bCanGoBack={true} bIsBusy={false}/>

		<div className="content-box" id='installer-pane'>
			<div className="container uninstall-container" >
				<div className='options'>
					<motion.div className='header' initial={{ y: "-100%" }} animate={{ y: 0 }}>
						<div className='header'>We're sorry to see you go 😔</div>
						<div className='description'>Select an option below to help improve Millennium </div>
					</motion.div>

					<motion.div initial={{ opacity: 0, scale: 0.5 }} animate={{ opacity: 1, scale: 1 }} transition={{ duration: 0.4, delay: 0.8, ease: [0, 0.71, 0.2, 1.01] }} className='options'>
						<div className='reason' onClick={e => ToggleTagState(e, "not interested")    }> Not interested anymore. </div>
						<div className='reason' onClick={e => ToggleTagState(e, "buggy")             }> Buggy, doesn't work as intended </div>
						<div className='reason' onClick={e => ToggleTagState(e, "not enough content")}> Not enough community made themes </div>
						<div className='reason' onClick={e => ToggleTagState(e, "other")             }> Other </div>
					</motion.div>
				</div>
			</div>

			<FooterLegend> 
				<button id="install" className={`btn next ${!tagCollection.length && 'disabled'}`} onClick={StartUninstall}>Uninstall</button>
			</FooterLegend>
		</div>
		</>
	)
}
  
export default UninstallerViewModal 