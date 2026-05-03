<div align="center">
<img src="data/sshot_icon.svg" alt="Sshot Icon" width="100" height="100">

# sShot 
Minimalist screenshot tool
</div>

sShot is a minimalist screenshot tool written in C using the SDL3 library. It works on both Wayland and X11. It allows users to take screenshots of their entire screen or a selected area, save them in PNG format, and copy them to the clipboard. The application is designed to be simple, minimal and easy to use. 

## Preview
![sShot Preview](data/preview.gif)

## Index
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Features
- Take screenshots of the entire screen or a selected area
- Save screenshots in PNG format
- Save screenshots to a specified directory
- Copy screenshots to the clipboard
- Simple and easy to use interface

## Installation
1. Install dependencies:

#### On Debian/Ubuntu:

```bash
sudo apt-get install libsdl3-dev libsdl3-image-dev libnotify-dev
```

> [!NOTE]

> Important Note: The sdl3 packages may not be available in the default repositories of some distributions. In that case, you may need to build > > SDL3 from source or use a third-party repository that provides SDL3 packages. In that case see the [SDL3 installation guide](https://gist.github.com/NoxFly/1067c9fc24024d26b51a6825de5cff74#file-install-md)
>

#### On Arch Linux:

```bash
sudo pacman -S sdl3 sdl3_image libnotify
```

2. Clone the repository:
   ```bash
   git clone https://github.com/MertYksl03/sShot.git
   ```
3. Navigate to the project directory:
   ```bash
   cd sShot
   ```
4. Build the project:
   ```bash
   make build 
   ```
5. Install the binary to your system:
   ```bash
   sudo make install
   ```

## Usage
To take a screenshot, simply run the `sshot` command in your terminal or from the application menu. You can choose to capture the entire screen or select a specific area. The captured screenshot will be saved in the specified directory and copied to the clipboard.

### Keybindings
|Key|Action|
|-----|--------|
|<kbd>Left Click</kbd>|Start selecting an area to crop|
|<kbd>Right Click</kbd>|Crop the selected area|
|<kbd>Ctrl</kbd> + <kbd>S</kbd>|Save screenshot|
|<kbd>Ctrl</kbd> + <kbd>C</kbd>|Copy screenshot to clipboard|
|<kbd>Ctrl</kbd> + <kbd>Z</kbd>|Undo last action|
|<kbd>Mouse Wheel</kbd> |Zoom in and out|
|<kbd>Esc</kbd> or <kbd>Q</kbd>|Exit the application|

## Uninstallation
To uninstall sShot, run the following command in your terminal:
1. First navigate to the project directory:
   ```bash
   cd sShot
   ```
2. Then run the uninstall command:
    ```bash
    sudo make uninstall
    ```

## Contributing
Contributions are welcome! If you have any ideas for new features or improvements, please feel free to submit a pull request or open an issue.

## License
This project is licensed under the GNU General Public License v3. See the [LICENSE](LICENSE) file for details