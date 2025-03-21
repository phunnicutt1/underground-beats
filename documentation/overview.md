# Underground Beats - Documentation Overview

## Introduction

This document serves as the central hub for all Underground Beats design and implementation documentation. It provides links to detailed documentation for each component and summarizes the key architectural decisions that shape the application.

## System Architecture

Underground Beats follows a modular architecture organized around these primary components:

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Core                         │
├─────────────┬─────────────────────────────┬─────────────────┤
│ Audio Engine│        Sequencer            │  Project State  │
│             │                             │                 │
│ - Real-time │ - Pattern Sequencing        │ - Session Data  │
│   Processing│ - MIDI Handling             │ - Settings      │
│ - Synthesis │ - Automation                │ - Preferences   │
│ - Routing   │ - Quantization              │                 │
└─────┬───────┴───────────┬─────────────────┴────────┬────────┘
      │                   │                          │
┌─────▼───────┐   ┌───────▼───────┐          ┌───────▼────────┐
│ Plugin Host │   │ Effects Chain │          │ Sample Manager │
│             │   │               │          │                │
│ - VST/AU    │   │ - DSP Units   │          │ - Sample Library│
│   Support   │   │ - Routing     │          │ - File I/O     │
│ - Parameter │   │ - Presets     │          │ - Format       │
│   Mapping   │   │               │          │   Conversion   │
└─────────────┘   └───────────────┘          └────────────────┘
      │                   │                          │
      └───────────────────▼──────────────────────────┘
                          │
                ┌─────────▼────────┐
                │    User Interface │
                │                  │
                │ - Pattern Editor │
                │ - Mixer View     │
                │ - Device Panels  │
                │ - Library Browser│
                └──────────────────┘
```

## Core Technology Decisions

Underground Beats is built with:

- **C++**: Chosen for real-time performance, deterministic memory management, and direct hardware access
- **JUCE Framework**: Provides cross-platform audio capabilities, plugin support, and audio-focused UI components

## Component Documentation

### Implemented Components

- [Audio Engine](audio-engine.md): Core real-time audio processing system
  - Audio device handling
  - Processor graph management
  - Thread-safe audio processing

- [Synthesis Engine](synthesis-engine.md): Sound generation components
  - Oscillator with multiple waveform types
  - ADSR envelope generator
  - Multi-mode filter
  - Polyphonic voice management

- [Effects Processing](effects-processing.md): Audio signal processing
  - Effect base class
  - Delay effect with feedback and tempo sync
  - Reverb effect for spatial processing
  - Effect chain management

- [Sequencer](sequencer.md): Pattern-based composition
  - Pattern data structure for notes and automation
  - Timeline for pattern arrangement
  - Transport controls and playback
  - MIDI input/output handling

- [User Interface](user-interface.md): User interaction and visual components
  - Application component structure
  - Pattern editor with piano roll interface
  - Mixer interface with channel strips and routing
  - Control panels for settings and parameters
  - Tabbed interface for organizing functionality

### Planned Components

- **Project State** *(Coming Soon)*: Session data management
  - Project saving/loading
  - Settings management
  - State persistence

## Implementation Progress

The current implementation includes:

1. **Audio Engine**:
   - AudioEngine class for core audio handling
   - ProcessorGraph class for audio signal routing
   - AudioDeviceManager for device handling

2. **Synthesis Engine**:
   - Oscillator class with multiple waveform types (sine, triangle, sawtooth, square, noise, wavetable)
   - Envelope class with ADSR functionality and customizable curves
   - Filter class with multiple filter types (low-pass, high-pass, band-pass, notch, shelf)
   - SynthVoice class for individual synthesis voices with 2 oscillators, filter and envelope
   - SynthModule class for polyphonic synthesis with voice management

3. **Effects Processing**:
   - Effect base class for common effect functionality with wet/dry mixing
   - Delay effect with stereo, feedback, cross-feedback and tempo-sync capabilities
   - Reverb effect for spatial processing with room size, damping, and width controls
   - EffectsChain class for managing multiple effects with flexible ordering

4. **Sequencer**:
   - Pattern class for storing MIDI notes and parameter automation
   - Timeline class for arranging patterns with non-destructive editing
   - Sequencer class for playback with transport controls and timing management
   - MidiEngine class for handling MIDI input/output with device selection

5. **User Interface**:
   - AppComponent class as the main application component with tabbed interface
   - PatternEditor class for piano roll-style note editing
   - PatternControlPanel class for editing settings and transport controls
   - PatternEditorView class combining editor and controls in a complete interface
   - MixerChannel class for channel strip controls (level, pan, mute, solo)
   - MixerView class with multiple channels and effect sends

## Code Organization

The project follows a structured directory organization:

```
/src
  /audio-engine           # Audio processing core
  /synthesis              # Sound generation components
  /effects                # Audio effects processing
  /sequencer              # Pattern sequencing and MIDI
  /ui                     # User interface components
    /components           # Reusable UI components
    /views                # Application views
  /project                # Project state management (planned)
  /utils                  # Utility functions and helpers (planned)
  /tests                  # Test suites for all components (planned)
```

## Environment-Specific Configurations

The application supports different environment configurations:

- **Development**: Additional logging, validation checks, and debugging tools
- **Testing**: Performance profiling, automated testing, and quality validation
- **Production**: Optimized performance, minimal runtime checks, and error recovery

## Performance Considerations

Key performance optimizations include:

- **SIMD Vectorization**: Using SIMD instructions for parallel audio processing
- **Lock-Free Programming**: Ensuring glitch-free audio with lock-free data structures
- **Memory Management**: Strategic memory handling to avoid allocations in audio threads
- **Cache Optimization**: Designing data structures for optimal cache utilization
- **Efficient UI Updates**: Throttled UI updates to minimize CPU usage

## Integration Between Components

The Underground Beats application components are designed to work seamlessly together:

1. **Audio Engine + Synthesis**:
   - Synthesizer modules can be added as processor nodes in the audio graph
   - Thread-safe parameter control allows real-time synthesis adjustment

2. **Audio Engine + Effects**:
   - Effect chains can be inserted at any point in the processor graph
   - Parameter automation enables dynamic effect control

3. **Sequencer + Synthesis**:
   - MIDI notes from sequencer drive synthesizer voices
   - Parameter automation from patterns controls synthesis parameters

4. **Sequencer + Effects**:
   - Tempo information syncs delay times and modulation rates
   - Parameter automation controls effect parameters

5. **UI + All Components**:
   - UI controls interact with all underlying components
   - Real-time state updates reflect component status in the UI

6. **Mixer + Audio Engine**:
   - Mixer channels control audio routing and levels
   - Effect sends route audio to effect processors

## Future Development

The roadmap for future development includes:

1. **Project State**:
   - Project file format for saving all settings and patterns
   - Preset system for synths and effects
   - User preferences management

2. **UI Enhancements**:
   - Synth parameter panels for detailed synthesis control
   - Effects configuration interface
   - Advanced visualization components
   - Automation editing interface

3. **Advanced Features**:
   - Additional synthesis methods (FM, granular, physical modeling)
   - More audio effects (EQ, compression, distortion)
   - Audio recording and sample manipulation
   - External plugin support (VST/AU)

## Contributing

When contributing to the Underground Beats project, please follow these guidelines:

- Keep files under 300 lines to maintain readability
- Follow the established design patterns for each component
- Include comprehensive unit tests for all new functionality
- Update documentation for any changes or additions
- Consider performance implications, especially for audio processing components
