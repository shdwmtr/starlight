import React from "react";
import ReactDOM from "react-dom/client";
import SelectPanel from './routes/Controller';

import Installer from './routes/Installer';
import InstallPanel from './routes/InstallPanel';

import Uninstaller from './routes/Uninstaller';
import UninstallPanel from './routes/UninstallPanel';

import AutoInstaller from './routes/AutoUpdater';

import { useEffect, useState } from 'react';
import { createHashRouter, RouterProvider } from 'react-router-dom';

import './styles/styles.css';
import { invoke } from "@tauri-apps/api/tauri";
import UninstallerSelectComponents from "./routes/SelectUninstallComponents";

function App() {
  const router = createHashRouter([
    { path: "/",                     element: <SelectPanel/>                 },
    { path: "/install",              element: <InstallPanel/>                },
    { path: "/run-install",          element: <Installer/>                   },
    { path: "/uninstall",            element: <UninstallPanel/>              },
    { path: "/run-uninstall",        element: <Uninstaller/>                 },
    { path: "/uninstall-components", element: <UninstallerSelectComponents/> },
  ]);

  // const auto_router = createHashRouter([
  //   {
  //     path: "/",
  //     element: <AutoInstaller/>
  //   }
  // ])

  // const [auto_installer, setAutoInstaller] = useState({
  //   initial: false,
  //   auto_installer: true,
  // });

  useEffect(() => {
    // window.ipcRenderer.invoke('auto-installer').then((response: any) => {
    //   console.log('Response from main process:', response);
    //   setAutoInstaller({
    //     initial: true,
    //     auto_installer: response,
    //   })
    // });
  }, [])

  return (
    <>
        <RouterProvider router={router}></RouterProvider>
    </>
  );
}

export default App;

ReactDOM.createRoot(document.getElementById("root") as HTMLElement).render(
  <App />
  // <React.StrictMode>
  //   <App />
  // </React.StrictMode>,
);
