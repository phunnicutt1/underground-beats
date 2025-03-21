#include "ProcessorNode.h"

ProcessorNode::ProcessorNode()
{
    // Initialize parameters to default values
    for (auto& param : parameters)
        param.store(0.0f);
}

ProcessorNode::~ProcessorNode()
{
    // Clean-up if needed
}

void ProcessorNode::setParameter(int index, float newValue)
{
    if (index >= 0 && index < MAX_PARAMETERS)
        parameters[index].store(newValue);
}

float ProcessorNode::getParameter(int index) const
{
    if (index >= 0 && index < MAX_PARAMETERS)
        return parameters[index].load();
    
    return 0.0f;
}

void ProcessorNode::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    // Allocate processing buffer
    processingBuffer.setSize(2, samplesPerBlock);
    
    isPrepared = true;
}

void ProcessorNode::releaseResources()
{
    isPrepared = false;
}

void ProcessorNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Base implementation just passes audio through
    // Derived classes will override this with actual processing
    
    // For SIMD-optimized processing
    processBlockSIMD(buffer, midiMessages);
}

void ProcessorNode::processBlockSIMD(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Basic implementation - will be enhanced later with SIMD optimizations
    juce::ScopedNoDenormals noDenormals;
    
    // This base implementation just passes audio through
    // Subclasses will override with real DSP
}
