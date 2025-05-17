# My_Project
# ⏱️ Mbed 7-Segment Display Timer & Voltage Meter

This embedded system project for the **NUCLEO-F401RE** board uses Mbed OS to:
- Display elapsed time in `MM:SS` format.
- Display analog voltage input using a potentiometer.
- Use buttons to switch modes and reset time.
- Control a 4-digit 7-segment display using shift registers.

---

## 🔧 Hardware Setup

| Component              | Connection (NUCLEO-F401RE) |
|------------------------|-----------------------------|
| Shift Register Latch   | `PB_5` (D4)                 |
| Shift Register Clock   | `PA_8` (D7)                 |
| Shift Register Data    | `PA_9` (D8)                 |
| Mode Button (S3)       | `PB_0` (A3)                 |
| Reset Button (S1)      | `PA_1` (A1)                 |
| Potentiometer (Voltage)| `PA_0` (A0)                 |
| 7-Segment Display      | Via 2 shift registers       |

---

## 📁 Project Structure

```
MyProject/
├── main.cpp              # Main embedded application code
├── mbed-os/              # Mbed OS submodule
├── .mbed                 # Mbed configuration
├── .gitignore            # Ignore file for Git
└── README.md             # Project documentation
```

---

## 🚀 How It Works

### Modes of Operation

- **Default (Time Mode)**: Displays elapsed time in `MM:SS` format.
- **Voltage Mode**: Displays analog input (from potentiometer) as voltage (`x.yy V`).

### Controls

- `S1 (Reset)` → Resets timer to `00:00`.
- `S3 (Mode)` → Hold to switch to voltage mode; release to go back to timer.

---

## 🧠 Key Functional Highlights

- **Timers:**
  - Uses `Ticker` to increment time every second.
  - Uses another `Ticker` to refresh the display every 2 ms for multiplexing.
- **Display:**
  - Uses two shift registers to drive a 4-digit 7-segment display.
  - Multiplexed display updates each digit one at a time.
- **Voltage Reading:**
  - Uses `AnalogIn` to read voltage from potentiometer.
  - Converts to centivolts and displays with one decimal.

---

## 🛠️ Build & Flash Instructions

### Prerequisites

- [Mbed Studio](https://os.mbed.com/studio/) **or** [Mbed CLI](https://os.mbed.com/docs/mbed-os/latest/build-tools/mbed-cli.html)
- Git with submodule support

### Clone with Submodules

```bash
git clone --recurse-submodules <repo-url>
```

> If you already cloned the repo:
```bash
git submodule update --init --recursive
```

### Compile & Flash

```bash
mbed compile -m NUCLEO_F401RE -t GCC_ARM --flash
```

---

## ⚠️ Troubleshooting

- **Permission denied errors during git submodule add:**  
  Ensure your IDE or terminal has administrator rights and no `.git` folders are locked or read-only.

- **Display not showing expected digits:**  
  Check wiring to shift registers and ensure the segment map matches your display type (common anode).
