> [!WARNING]  
> This repository has been archived as we've moved to a simpler installer.

&nbsp;


<div align="center">
<h3><img align="center" height="40" src="https://i.imgur.com/9qYPFSA.png"> &nbsp; &nbsp;Millennium Installer</h3>

A simple standalone program which automates the installation, removal and maintenance of Millennium.

![userlmn_5792a2476abb30c9e3abc0df0f95e6e2](https://github.com/SteamClientHomebrew/Installer/assets/81448108/0554303a-b0a3-491a-b11c-73fb60451d40)

</div>

## Installing

  The installer current only supports Windows. See [this page](https://github.com/SteamClientHomebrew/Millennium/wiki/Getting-Started#automatic) for a more detailed guide.

  [![Static Badge](https://img.shields.io/badge/Download%20Windows-fff?style=for-the-badge&logo=windows&logoColor=white&color=2D5CBF)][windows-link]

  [windows-link]: https://github.com/SteamClientHomebrew/Installer/releases/latest/download/Millennium.Installer-Windows.exe
  [windows-badge]: https://img.shields.io/badge/Windows%20(10+)-3a71c1?logo=Windows&logoColor=white&labelColor=111111&color=3a71c1&style=for-the-badge


&nbsp;

## Building

### Prerequisites
- [git](https://git-scm.com) 
- [pnpm](https://pnpm.io/)
- [rust](https://www.rust-lang.org/) 
- Command line of your choice.
&nbsp;
```ps1
$ git clone https://github.com/SteamClientHomebrew/Installer.git && cd Installer
$ pnpm install
$ pnpm tauri dev

# Building release
# $ pnpm tauri build
```
