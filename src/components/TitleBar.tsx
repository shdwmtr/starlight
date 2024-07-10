import packageJson from '../../package.json';
import { Core } from '../util/ForeignFunctions';

export interface TitleBarProps {
    bCanGoBack: boolean;
    bIsBusy: boolean;
}

interface TitleBarBackButtonProps {
    onClick: (event: React.MouseEvent<HTMLDivElement>) => void;
    bIsBusy: boolean;
}

const TitleBarBackButton: React.FC<TitleBarBackButtonProps> = ({ onClick, bIsBusy}) => {
    return (
        <div className={`back-btn-container ${bIsBusy ? 'busy' : ''}`} onClick={onClick}>
            <img className="back-btn" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAnUlEQVR4nO3YsQrCQBBF0Um+XbGRgBZa6TeKQSyvhCCidml2JtzzAct7M1ssGyFJkmZADxyBO7CJguEvfNyicPjJPioAOuDMt6lMH9lh+EacfCt45xvByTeCk2+I+VWZwRPYLikwkse4pMCB4hvogNPPYdcSr8w3S2SB1ykJ3EQSuIkkcBO5f+aGqIT/Eo+ohrnEMIUHdq3zSJLW7wVESEJtOi1BnwAAAABJRU5ErkJggg=="/>
        </div>
    )
}

const TitleBar: React.FC<TitleBarProps> = ({ bCanGoBack, bIsBusy }) => {

    const ExitInstaller = () => { 
        Core.ExitEnvironment();
    }

    const HistoryBack = (event: any) => {
        if (event.target.classList.contains('busy')) 
            return;

        window.history.back();
    }

    return (
        <div data-tauri-drag-region className="titlebar">
            { bCanGoBack && <TitleBarBackButton bIsBusy={bIsBusy} onClick={HistoryBack}/> }

            <img className='logo' src="logo.png" alt=""/>  
            <div className="name">Millennium Installer</div>
            <div className="version">v{packageJson.version}</div>

            <div className="closebtn-container" onClick={ExitInstaller}>
                <img className="closebtn" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAACXBIWXMAAAsTAAALEwEAmpwYAAABJUlEQVR4nO2Y2wqCUBBF7Qe7UfRQ315QJEWXHzBYIRQFWuk5Z8aRZr31smev9OhUljmO4zhSAHPgDOyBseLcCXAArsAiNGQAnHhRBIe1m7t4zHpyKbuEhh3fgsQlqJYvOcYETmsCRSSoL19+nsYGz2qCb8Aq8VkramYsUw0Qk0C6vKQEWuUlJNTLp5TorHwKic7Lx0iYKR8iYa58Gwmz5ZtImC/fYBVQWUUkr4Ttb76FhP3yJR/u+crB7lt5+xL0+RDz5VGp8XsiiibPebMStHhJmZMg4A1rRoKI9aBzCRLsNp1JkHAxU5dAYKtES0JyJUZaQmOfR0oCGBn4a3EUE7rTKP9DYpuFAuRa5b9I5FnkLbQF1lGXMmzu5jF7qDXXcRznD7kDEjZgdJknwowAAAAASUVORK5CYII="/>
            </div>
        </div>
    )
}

export { TitleBar }