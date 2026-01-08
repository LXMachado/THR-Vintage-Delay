# Blue Haze - Vintage Delay Plugin

A warm, vintage-style delay plugin inspired by classic tape delay units. Built with JUCE framework for macOS VST3 format.

![Blue Haze Plugin](https://via.placeholder.com/800x200/1a1a2e/ffffff?text=Blue+Haze+Vintage+Delay)

## Features

- **Tape-style Delay**: Warm, musical delay with characteristics reminiscent of vintage tape units
- **Flutter & Drift**: Simulates tape wow and flutter for authentic vintage character
- **Drive/Tape Saturation**: Adds harmonic richness and warmth
- **Filter Section**: Low and high cut filters to shape the delay repeats
- **Sync Tempo**: Optional tempo-synced delay times
- **Smooth Parameter Smoothing**: Prevents clicking andzipper noise during automation

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Time (ms) | 1 - 2000 | 400 | Delay time in milliseconds |
| Feedback | 0 - 95% | 40% | Amount of feedback/regeneration |
| Mix | 0 - 100% | 25% | Wet/dry mix ratio |
| Output (dB) | ±12 | 0 | Output gain |
| Low Cut (Hz) | 20 - 1000 | 120 | High-pass filter frequency |
| Hi Cut (Hz) | 1500 - 18000 | 7000 | Low-pass filter frequency |
| Flutter Amt (ms) | 0 - 2 | 0.3 | Tape flutter intensity |
| Flutter Rate (Hz) | 0.1 - 8 | 4 | Flutter modulation rate |
| Flutter Random | 0 - 100% | 25% | Random wow component |
| Drive | 0 - 100% | 20% | Tape saturation amount |
| Sync | On/Off | Off | Enable tempo sync |
| Sync Division | 1/1 - 1/32 | 1/4 | Tempo-synced delay time |

## Requirements

- **macOS** 10.15 or later
- **JUCE Framework** 7.0 or later
- **CMake** 3.15 or later
- **C++17** compatible compiler

## Building

### Prerequisites

1. Install JUCE framework:
   ```bash
   # Clone JUCE somewhere on your system
   cd /path/to/JUCE
   git clone --depth 1 https://github.com/juce-framework/JUCE.git
   ```

2. Ensure CMake is installed:
   ```bash
   brew install cmake
   ```

### Build Instructions

```bash
# Navigate to project directory
cd /path/to/THR-Vintage-Delay

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make -j4

# The VST3 plugin will be installed automatically to:
# ~/Library/Audio/Plug-Ins/VST3/Blue Haze.vst3
```

### Manual Plugin Installation

If automatic installation doesn't work:

```bash
# Copy plugin manually
cp -r BlueHaze_artefacts/VST3/Blue\ Haze.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

## Architecture

```
THR-Vintage-Delay/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── Source/
│   ├── PluginProcessor.cpp # Audio processing logic
│   ├── PluginProcessor.h   # Processor interface
│   ├── PluginEditor.cpp    # GUI implementation
│   ├── PluginEditor.h      # Editor interface
│   ├── VintageDelayDSP.h   # Delay algorithm implementation
│   └── Params.h            # Parameter definitions
└── .gitignore              # Git ignore rules
```

### DSP Architecture

```
Input → Low Cut → Delay Line → Hi Cut → Drive (tanh) → Feedback Loop
                                              ↓
Output ← Mix ← Gain (dB) ← Saturation ←←←←←←←←←←←
```

The delay line uses 3rd-order Lagrange interpolation for high-quality variable delay times. The feedback path includes both filters for tone shaping and soft clipping for tape-like saturation.

## DSP Features

### Delay Line
- Maximum delay: 2 seconds
- 3rd-order Lagrange interpolation
- Smooth parameter smoothing on all controls

### Filters
- State Variable TPT (Topological Tapped) filters
- High-pass and low-pass in feedback path
- 0.707 Q (critical damping)

### Flutter Simulation
- Sinusoidal pitch modulation
- Random noise component with low-pass filtering
- Independent rate and depth controls

### Saturation
- Soft clipping using tanh function
- Drive amount controls saturation intensity

## License

This project is licensed under the MIT License.

## Credits

- Built with [JUCE Framework](https://juce.com/)
- Inspired by classic tape delay units

## Author

Alexandre Machado - Tinnie House DSP
