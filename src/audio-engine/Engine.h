#pragma once

#include <JuceHeader.h>
#include "ProcessorNode.h"
#include "ProcessorGraph.h"

// Audio device settings structure
struct AudioDeviceSettings
{
    double sampleRate = 44100.0;
    int bufferSize = 256;
    int inputChannels = 0;
    int outputChannels = 2;
};

// Transport state enumeration
enum class TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//==============================================================================
class Engine
{
public:
    Engine();
    ~Engine();

    // Core initialization methods
    bool initialize(const AudioDeviceSettings& settings);
    bool shutdown();
    
    // Audio processing
    void processAudio(const juce::AudioSourceChannelInfo& bufferToFill);
    
    // Engine state control
    bool start();
    bool stop();
    bool isRunning() const;
    
    // Graph management (basic implementation for initial setup)
    NodeID addProcessor(std::unique_ptr<ProcessorNode> processor);
    bool connectNodes(NodeID source, int sourceChannel, NodeID destination, int destChannel);
    
    // Parameter management (thread-safe)
    void setParameter(NodeID node, int paramIndex, float value);
    
    // Transport control
    void setTransportState(TransportState newState);
    TransportState getTransportState() const;

private:
    // Audio device management
    AudioDeviceSettings deviceSettings;
    
    // Audio processor graph
    std::unique_ptr<ProcessorGraph> processorGraph;
    
    // Thread-safe parameter updates (simple version for now)
    std::mutex parameterMutex;
    
    // Lock-free transport state
    std::atomic<TransportState> transportState{TransportState::Stopped};
    
    // Test oscillator for initial implementation
    juce::dsp::Oscillator<float> testOscillator;
    juce::dsp::Gain<float> outputGain;
    juce::LinearSmoothedValue<float> frequencySmoothed{440.0f};
    
    // Processing chain for basic sound
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> processingChain;
    
    // DSP context for processing
    juce::dsp::ProcessSpec processSpec;
    bool initialized = false;
};
