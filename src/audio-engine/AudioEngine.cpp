/*
 * Underground Beats
 * AudioEngine.cpp
 * 
 * Implementation of the core audio processing engine
 */

#include "AudioEngine.h"

namespace UndergroundBeats {

AudioEngine::AudioEngine()
    : running(false)
    , currentSampleRate(0.0)
    , currentBufferSize(0)
{
    audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();
    processorGraph = std::make_unique<juce::AudioProcessorGraph>();
    audioProcessorPlayer = std::make_unique<juce::AudioProcessorPlayer>();
}

AudioEngine::~AudioEngine()
{
    stop();
    audioProcessorPlayer.reset();
    processorGraph.reset();
    audioDeviceManager.reset();
}

bool AudioEngine::initialize(double sampleRate, int bufferSize)
{
    // Initialize the audio device manager with default settings
    juce::String audioError;
    
    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceSetup.sampleRate = sampleRate;
    deviceSetup.bufferSize = bufferSize;
    
    audioError = audioDeviceManager->initialise(2, 2, nullptr, true, "", &deviceSetup);
    
    if (audioError.isNotEmpty())
    {
        // Handle initialization error
        return false;
    }
    
    // Set up the processor graph with the correct sample rate and block size
    processorGraph->setPlayConfigDetails(2, 2, sampleRate, bufferSize);
    processorGraph->prepareToPlay(sampleRate, bufferSize);
    
    // Set up the audio processor player
    audioProcessorPlayer->setProcessor(processorGraph.get());
    
    currentSampleRate = sampleRate;
    currentBufferSize = bufferSize;
    
    return true;
}

bool AudioEngine::start()
{
    if (!running)
    {
        // Connect the processor player to the audio device manager
        audioDeviceManager->addAudioCallback(audioProcessorPlayer.get());
        running = true;
        return true;
    }
    
    return false;
}

void AudioEngine::stop()
{
    if (running)
    {
        // Disconnect the processor player from the audio device manager
        audioDeviceManager->removeAudioCallback(audioProcessorPlayer.get());
        running = false;
    }
}

bool AudioEngine::isRunning() const
{
    return running;
}

double AudioEngine::getSampleRate() const
{
    return currentSampleRate;
}

int AudioEngine::getBufferSize() const
{
    return currentBufferSize;
}

juce::AudioProcessorGraph& AudioEngine::getProcessorGraph()
{
    return *processorGraph;
}

} // namespace UndergroundBeats
