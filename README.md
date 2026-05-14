# Omni-Updater 

A robust, multi-package manager orchestration engine written in C. It updates all the packages on your system so you no more experience headaches keeping everything up to date.

Omni-Updater reads your custom JSON configuration, runs sequentially across distinct package managers, safely tracks its progress to disk, detects kernel updates, and seamlessly resumes incomplete workloads across system reboots.

## Features

*   **Multi-Manager Layout**: Handles `apt`, `pacman`, `flatpak`, `snap`, and custom ecosystems out of the box.
*   **JSON Configured**: Simple configuration structure allows you to enable, disable, or custom-tailor update pipelines.
*   **Reboot-Safe Persistence**: Saves current execution checkpoints to disk to safely handle interruption states.
*   **Kernel Intervention**: Safely interrupts timelines to perform system reboots if kernel upgrades are detected mid-run.
*   **Session Continuity**: Integrates into desktop autostart targets to continue updates right where they left off post-login.

## Core Prerequisites

Ensure the following tracking dependencies are present on your development environment before compilation:

*   **CMake** (v3.25+)
*   **C Compiler** (`gcc` or `clang`)
*   **cJSON Library** (Development headers)

On Debian/Ubuntu environments, run:
```bash
sudo apt install cmake build-essential libcjson-dev
```

## Compilation and Installation

Build the binary from source using the provided `CMakeLists.txt`:

```bash
# Clone or step into the project codebase directory
mkdir build && cd build

# Generate build engine targets and compile
cmake ..
make

# Create a package archive for your OS
cpack
```

## System Setup

### 1. Configuration File
Create the folder directory and save your system update profile at `/etc/omni-updater/config.json`:

```json
{
  "package_managers": [
    {
      "name": "apt",
      "enabled": true,
      "update_cmd": "apt-get update",
      "upgrade_cmd": "apt-get dist-upgrade -y"
    },
    {
      "name": "flatpak",
      "enabled": true,
      "update_cmd": "flatpak update --appstream -y",
      "upgrade_cmd": "flatpak update -y"
    }
  ]
}
```

### 2. Autostart Session Recovery (Optional)
To automatically launch an elevated terminal wrapper that resumes pending updating pipelines upon logging back into your visual desktop workspace, place the following script at `/etc/xdg/autostart/omni-updater-resume.desktop`:

```ini
[Desktop Entry]
Type=Application
Name=Omni Updater Resumer
Exec=sudo x-terminal-emulator -e "/usr/local/bin/omni-updater"
Terminal=true
NoDisplay=true
X-GNOME-Autostart-enabled=true
```

## How to Run

Because Omni-Updater communicates directly with core system package managers, execution must be initialized with root permissions:

```bash
sudo omni-updater
```
