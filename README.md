# FixAccidentalTouch

A Minecraft Bedrock Edition mod that fixes accidental key conflicts when using auto-sprint.

---

## Features

- Hold **W** to auto-sprint (Ctrl)
- Press **Ctrl** to toggle auto-sprint mode
- Automatically releases Ctrl when pressing **B** to prevent Ctrl+B (repeat chat)
- Only works in-game, disabled in menus/settings/chat

---

## Installation

### 1. Download DLL

Download the latest `.dll` file from [Releases](https://github.com/yourusername/FixAccidentalTouch/releases).

### 2. Install

**For LeviLauncher or mc-w10-version-launcher:**

1. Open the game folder
2. Find the `mods` folder
3. Place the `.dll` file in the mods folder

---

## Usage

1. Enter the game
2. Press **Ctrl** to enable auto-sprint mode
3. Hold **W** to auto-sprint
4. When pressing **B**, Ctrl is automatically released to prevent repeat chat

---

## Build

```bash
# Install xmake
# Clone project
git clone https://github.com/yourusername/FixAccidentalTouch.git
cd FixAccidentalTouch

# Build
xmake build
```

---

## License

MIT License
