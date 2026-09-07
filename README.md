# NetCfgTool

A Qt6/C++ desktop GUI for viewing and managing basic IPv4 network settings on Ubuntu, built on top of NetworkManager (`nmcli`).

## Overview

NetCfgTool gives users a simple graphical way to inspect and modify IPv4 configuration for Ethernet and Wi-Fi interfaces already managed by NetworkManager — without touching netplan/YAML files directly. Built as an OOP exercise following Clean Code principles (Robert C. Martin).

## Features

- List network interfaces with current state (name, type, status, IPv4, gateway, DNS)
- Switch IPv4 mode: DHCP ↔ Static
- Configure static IP/CIDR, gateway, and DNS servers
- Enable / disable network interfaces
- Apply and revert changes
- Save, load, and remove local configuration profiles (JSON)
- Action and error logging with automatic rotation
- Input validation (IP/CIDR, gateway, DNS)

**Out of scope:** Wi-Fi network scanning/management, VPN, VLAN, IPv6, Proxy/Firewall, direct netplan editing, background service/autostart.

## Tech Stack

- **Language:** C++20
- **Framework:** Qt 6.5 LTS (Core, Widgets, Gui)
- **Build:** CMake ≥ 3.22
- **System integration:** NetworkManager (`nmcli`) via `QProcess`; privileged changes via `pkexec`
- **Testing:** QtTest / GoogleTest

## Architecture

The project is split into three layers, with dependencies injected through constructors (no singletons, no hidden globals):

- **UI layer** — Qt widgets following an MVP-style approach (`MainWindow` with `OverviewTab`, `DetailsTab`, `IPv4ConfigTab`, `ProfilesTab`, `LogsTab`)
- **Domain layer** — interfaces and models (`INetworkService`, `IProfileStore`, `ILogger`, `Validator`)
- **Infrastructure layer** — concrete implementations (`NmcliNetworkService`, `JsonProfileStore`, `FileLogger`)

## Requirements

- Ubuntu 22.04.3 LTS (or compatible)
- GCC 13.3.0+
- Qt 6.5 dev packages
- `network-manager`, `policykit-1`

## Build & Run

```bash
git clone <repo-url>
cd NetCfgTool
mkdir build && cd build
cmake ..
cmake --build .
./NetCfgTool
```

## Testing

```bash
ctest
```

Covers input validators, JSON profile storage, and `nmcli` output parsing. Target coverage: ≥70% for validators and profile store.

## Data & Logs

- Profiles: `~/.config/NetCfgTool/profiles.json`
- Logs: `~/.local/share/NetCfgTool/logs/app.log` (rotated at 5 MB, up to 3 files)

## Permissions

Reading network state requires no elevated privileges. Applying configuration changes runs via `pkexec nmcli ...` and will prompt for system authentication.

## License

MIT

