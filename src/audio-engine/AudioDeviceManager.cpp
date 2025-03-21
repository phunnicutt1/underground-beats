/*
 * Underground Beats
 * AudioDeviceManager.cpp
 * 
 * Implementation of audio device configuration and handling
 */

#include "AudioDeviceManager.h"

namespace UndergroundBeats {

AudioDeviceManager::AudioDeviceManager()
    : errorCallback(nullptr)
{
}

AudioDeviceManager::~AudioDeviceManager()
{
}

juce::String AudioDeviceManager::initialize(int numInputChannels, 
                                          int numOutputChannels,
                                          double preferredSampleRate,
                                          int preferredBufferSize)
{
    // Set up the device with the preferred settings
    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceSetup.sampleRate = preferredSampleRate;
    deviceSetup.bufferSize = preferredBufferSize;
    
    // Initialize the device
    juce::String error = initialise(numInputChannels, numOutputChannels, nullptr, true, "", &deviceSetup);
    
    if (error.isNotEmpty() && errorCallback != nullptr)
    {
        errorCallback(error);
    }
    
    return error;
}

bool AudioDeviceManager::saveConfiguration(const juce::String& filePath)
{
    // Create an XML element to store the configuration
    auto xml = createStateXml();
    
    if (xml != nullptr)
    {
        // Save the XML to the file
        juce::File file(filePath);
        if (file.create())
        {
            file.replaceWithText(xml->toString());
            return true;
        }
    }
    
    if (errorCallback != nullptr)
    {
        errorCallback("Failed to save audio device configuration");
    }
    
    return false;
}

bool AudioDeviceManager::loadConfiguration(const juce::String& filePath)
{
    // Load the XML from the file
    juce::File file(filePath);
    
    if (file.existsAsFile())
    {
        auto xml = juce::XmlDocument::parse(file);
        
        if (xml != nullptr)
        {
            // Apply the configuration
            auto error = initialise(2, 2, xml.get(), true);
            
            if (error.isEmpty())
            {
                return true;
            }
            else if (errorCallback != nullptr)
            {
                errorCallback("Failed to apply audio device configuration: " + error);
            }
        }
        else if (errorCallback != nullptr)
        {
            errorCallback("Failed to parse audio device configuration file");
        }
    }
    else if (errorCallback != nullptr)
    {
        errorCallback("Audio device configuration file not found");
    }
    
    return false;
}

void AudioDeviceManager::setErrorCallback(std::function<void(const juce::String&)> callback)
{
    errorCallback = callback;
}

juce::StringArray AudioDeviceManager::getAvailableDeviceNames() const
{
    juce::StringArray devices;
    
    auto& deviceTypes = getAvailableDeviceTypes();
    
    for (auto deviceType : deviceTypes)
    {
        auto names = deviceType->getDeviceNames();
        devices.addArray(names);
    }
    
    return devices;
}

juce::Array<double> AudioDeviceManager::getAvailableSampleRates() const
{
    if (getCurrentAudioDevice() != nullptr)
    {
        return getCurrentAudioDevice()->getAvailableSampleRates();
    }
    
    return {};
}

juce::Array<int> AudioDeviceManager::getAvailableBufferSizes() const
{
    if (getCurrentAudioDevice() != nullptr)
    {
        return getCurrentAudioDevice()->getAvailableBufferSizes();
    }
    
    return {};
}

void AudioDeviceManager::audioDeviceError(const juce::String& errorMessage)
{
    // Forward the error to the callback if set
    if (errorCallback != nullptr)
    {
        errorCallback(errorMessage);
    }
    
    // Call the base class implementation
    juce::AudioDeviceManager::audioDeviceError(errorMessage);
}

} // namespace UndergroundBeats
