import ReactDOM       from "react-dom/client";
import SelectPanel    from './routes/Controller';
import Installer      from './routes/Installer';
import InstallPanel   from './routes/InstallPanel';
import UninstallPanel from './routes/UninstallPanel';
import AutoInstaller  from './routes/AutoUpdater';

import { invoke } from "@tauri-apps/api/tauri";
import { useEffect, useState } from 'react';
import { createHashRouter, RouterProvider } from 'react-router-dom';

import UninstallerSelectComponents from "./routes/Uninstaller";
import './styles/styles.css';

function InstallerApp() {
  const manualInstallerRoute = createHashRouter([
    { path: "/",                     element: <SelectPanel/>                 },
    { path: "/install",              element: <InstallPanel/>                },
    { path: "/run-install",          element: <Installer/>                   },
    { path: "/uninstall",            element: <UninstallPanel/>              },
    { path: "/uninstall-components", element: <UninstallerSelectComponents/> },
  ]);

  const autoInstallerRoute = createHashRouter([{ path: "/", element: <AutoInstaller/> }])

  enum eInstallerStatusProps {
    AUTO, MANUAL, UNSET
  }

  const InstallerStatus = new Map<eInstallerStatusProps, any>([
    [eInstallerStatusProps.UNSET, null],
    [eInstallerStatusProps.MANUAL, manualInstallerRoute],
    [eInstallerStatusProps.AUTO, autoInstallerRoute],
  ]);

  const [installerProps, setInstallerProps] = useState(InstallerStatus.get(eInstallerStatusProps.UNSET));

  const GetViewPortInfo = () => {
    invoke('is_auto_installer').then((response: any) => {
      setInstallerProps(response ? eInstallerStatusProps.AUTO : eInstallerStatusProps.MANUAL);
    });
  }

  useEffect(() => GetViewPortInfo(), [])

  return installerProps != InstallerStatus.get(eInstallerStatusProps.UNSET)
    && <RouterProvider router={InstallerStatus.get(installerProps)}></RouterProvider>;
}
export default InstallerApp;

ReactDOM.createRoot(document.getElementById("root") as HTMLElement).render(<InstallerApp/>);
