/*
 * Underground Beats
 * AudioDeviceManager.h
 * 
 * Manages audio device configuration and handling
 */

#pragma once

#include <JuceHeader.h>
#include <functional>

namespace UndergroundBeats {

/**
 * @class AudioDeviceManager
 * @brief Manages audio device setup, configuration, and I/O handling
 * 
 * This class extends JUCE's AudioDeviceManager to provide additional functionality
 * specific to Underground Beats, including device selection, configuration persistence,
 * and error handling.
 */
class AudioDeviceManager : public juce::AudioDeviceManager {
public:
    AudioDeviceManager();
    ~AudioDeviceManager() override;
    
    /**
     * @brief Initialize with default settings
     * 
     * @param numInputChannels Number of input channels to enable
     * @param numOutputChannels Number of output channels to enable
     * @param preferredSampleRate Preferred sample rate
     * @param preferredBufferSize Preferred buffer size
     * @return Error message if initialization failed, empty string otherwise
     */
    juce::String initialize(int numInputChannels = 2, 
                          int numOutputChannels = 2,
                          double preferredSampleRate = 44100.0,
                          int preferredBufferSize = 512);
    
    /**
     * @brief Save the current device settings to a configuration file
     * 
     * @param filePath The path to save the configuration
     * @return true if saved successfully, false otherwise
     */
    bool saveConfiguration(const juce::String& filePath);
    
    /**
     * @brief Load device settings from a configuration file
     * 
     * @param filePath The path to load the configuration from
     * @return true if loaded successfully, false otherwise
     */
    bool loadConfiguration(const juce::String& filePath);
    
    /**
     * @brief Set a callback for audio device error notifications
     * 
     * @param callback The function to call when an error occurs
     */
    void setErrorCallback(std::function<void(const juce::String&)> callback);
    
    /**
     * @brief Get a list of available audio devices
     * 
     * @return StringArray containing the names of available devices
     */
    juce::StringArray getAvailableDeviceNames() const;
    
    /**
     * @brief Get a list of available sample rates for the current device
     * 
     * @return Array of available sample rates
     */
    juce::Array<double> getAvailableSampleRates() const;
    
    /**
     * @brief Get a list of available buffer sizes for the current device
     * 
     * @return Array of available buffer sizes
     */
    juce::Array<int> getAvailableBufferSizes() const;
    
private:
    std::function<void(const juce::String&)> errorCallback;
    
    // Overridden to handle audio device errors
    void audioDeviceError(const juce::String& errorMessage) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDeviceManager)
};

} // namespace UndergroundBeats
