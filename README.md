<img src="https://i.imgur.com/9qYPFSA.png" alt="Alt text" width="40">

## SCH Millennium Installer

A simple standalone program which automates the installation, removal and maintenance of Millennium.

[![GitHub Releases][downloads-badge]][downloads-link] &nbsp; [![Discord][discord-badge]][discord-link]  &nbsp; [![Website][website-badge]][website-link]  &nbsp; [![Docs][docs-badge]][docs-link]

[downloads-badge]: https://img.shields.io/github/downloads/shadowmonster99/millennium-steam-binaries/total?labelColor=0c0d10&color=3a71c1&logo=data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNDgiIGhlaWdodD0iNDgiIHZpZXdCb3g9IjAgMCA0OCA0OCIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEyLjI1IDM4LjVIMzUuNzVDMzYuNzE2NSAzOC41IDM3LjUgMzkuMjgzNSAzNy41IDQwLjI1QzM3LjUgNDEuMTY4MiAzNi43OTI5IDQxLjkyMTIgMzUuODkzNSA0MS45OTQyTDM1Ljc1IDQySDEyLjI1QzExLjI4MzUgNDIgMTAuNSA0MS4yMTY1IDEwLjUgNDAuMjVDMTAuNSAzOS4zMzE4IDExLjIwNzEgMzguNTc4OCAxMi4xMDY1IDM4LjUwNThMMTIuMjUgMzguNUgzNS43NUgxMi4yNVpNMjMuNjA2NSA2LjI1NThMMjMuNzUgNi4yNUMyNC42NjgyIDYuMjUgMjUuNDIxMiA2Ljk1NzExIDI1LjQ5NDIgNy44NTY0N0wyNS41IDhWMjkuMzMzTDMwLjI5MzEgMjQuNTQwN0MzMC45NzY1IDIzLjg1NzMgMzIuMDg0NiAyMy44NTczIDMyLjc2OCAyNC41NDA3QzMzLjQ1MTQgMjUuMjI0MiAzMy40NTE0IDI2LjMzMjIgMzIuNzY4IDI3LjAxNTZMMjQuOTg5OCAzNC43OTM4QzI0LjMwNjQgMzUuNDc3MiAyMy4xOTg0IDM1LjQ3NzIgMjIuNTE1IDM0Ljc5MzhMMTQuNzM2OCAyNy4wMTU2QzE0LjA1MzQgMjYuMzMyMiAxNC4wNTM0IDI1LjIyNDIgMTQuNzM2OCAyNC41NDA3QzE1LjQyMDIgMjMuODU3MyAxNi41MjgyIDIzLjg1NzMgMTcuMjExNyAyNC41NDA3TDIyIDI5LjMyOVY4QzIyIDcuMDgxODMgMjIuNzA3MSA2LjMyODgxIDIzLjYwNjUgNi4yNTU4TDIzLjc1IDYuMjVMMjMuNjA2NSA2LjI1NThaIiBmaWxsPSIjM2E3MWMxIi8+Cjwvc3ZnPgo=
[downloads-link]: #auto-installers

[build-badge]: https://img.shields.io/github/actions/workflow/status/shadowmonster99/millennium-steam-patcher/build.yml?branch=main&logo=Github&logoColor=3a71c1&labelColor=0c0d10&color=3a71c1
[build-link]: https://github.com/shadowmonster99/millennium-steam-patcher/actions/workflows/build.yml
[discord-badge]: https://img.shields.io/badge/discord-green?labelColor=0c0d10&color=7289da&logo=discord&logoColor=7289da
[discord-link]: https://millennium.web.app/discord

[website-badge]: https://img.shields.io/badge/website-green?labelColor=0c0d10&color=3a71c1&logo=firefoxbrowser&logoColor=3a71c1
[website-link]: https://millennium.web.app/

[docs-badge]: https://img.shields.io/badge/docs-green?labelColor=0c0d10&color=3a71c1&logo=readthedocs&logoColor=3a71c1
[docs-link]: https://millennium.gitbook.io/steam-patcher/


<br>

## Prerequisites
- [Git](https://git-scm.com)
- [Node.js](https://nodejs.org/en/)
- npm (pre-installed with node)
- Command line of your choice.

## Building 

### Clone the repository.
```ps
git clone https://github.com/SteamClientHomebrew/Installer.git && cd Installer
```
This will create a local copy of this repository and navigate you to the root folder of the repository.

### Install Dependencies
```ps
npm install
```
This installs node module dependencies needed

### Start Development
```ps
npm run dev
```
This runs the installer in development mode


### Compiling
```ps
npm run build
```
compile the installer to an executable
