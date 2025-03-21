/*
 * Underground Beats
 * AudioEngine.h
 * 
 * Core audio processing engine responsible for real-time audio handling
 */

#pragma once

#include <JuceHeader.h>

namespace UndergroundBeats {

/**
 * @class AudioEngine
 * @brief Core audio processing engine that manages the real-time audio thread and processing chain
 * 
 * The AudioEngine class serves as the foundation of the application's audio processing system.
 * It handles the initialization of audio devices, manages the audio processing graph,
 * and ensures thread-safe communication between the audio thread and the rest of the application.
 */
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    /**
     * @brief Initialize the audio engine with the selected audio device
     * 
     * @param sampleRate The desired sample rate
     * @param bufferSize The desired buffer size
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(double sampleRate = 44100.0, int bufferSize = 512);
    
    /**
     * @brief Start audio processing
     * 
     * @return true if the audio engine started successfully, false otherwise
     */
    bool start();
    
    /**
     * @brief Stop audio processing
     */
    void stop();
    
    /**
     * @brief Check if the audio engine is currently running
     * 
     * @return true if the audio engine is running, false otherwise
     */
    bool isRunning() const;
    
    /**
     * @brief Get the current sample rate
     * 
     * @return The current sample rate in Hz
     */
    double getSampleRate() const;
    
    /**
     * @brief Get the current buffer size
     * 
     * @return The current buffer size in samples
     */
    int getBufferSize() const;
    
    /**
     * @brief Get access to the audio processor graph
     * 
     * @return Reference to the audio processor graph
     */
    juce::AudioProcessorGraph& getProcessorGraph();
    
private:
    std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager;
    std::unique_ptr<juce::AudioProcessorGraph> processorGraph;
    std::unique_ptr<juce::AudioProcessorPlayer> audioProcessorPlayer;
    
    bool running;
    double currentSampleRate;
    int currentBufferSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

} // namespace UndergroundBeats
