# Underground Beats

A professional-grade audio application that allows artists to deconstruct audio samples into component elements, manipulate these elements individually, and automatically generate variations with integrated effects processing.

## Getting Started

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- JUCE framework

### Building the Project

1. Clone the JUCE repository into the project directory:
   ```
   git clone https://github.com/juce-framework/JUCE.git
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Run CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build .
   ```

5. Run the application:
   ```
   ./Underground_Beats
   ```

## Project Structure

- `src/`: Source code
  - `audio-engine/`: Core audio processing
  - `synthesis/`: Sound generation
  - `effects/`: Audio effects processing
  - `sequencer/`: Pattern-based sequencing
  - `ui/`: User interface components
  - `project/`: Project management
  - `utils/`: Utility classes

## Initial Development Focus

The current implementation includes a minimal working prototype with:

1. Basic application setup
2. Simple audio engine with oscillator
3. User interface with transport controls

## Next Steps

Following our development roadmap, we'll be implementing:

1. Complete audio processor graph
2. SIMD-optimized DSP
3. Basic synthesis capabilities
4. Effects processing

For a complete overview of the project design and architecture, see `underground-beats-documentation.md`.
