#include "Reverb.h"

Reverb::Reverb()
    : roomSize(0.5f),        // 50% default room size
      dampingAmount(0.5f),   // 50% default damping
      stereoWidth(1.0f),     // 100% default width
      dryWetMix(0.3f)        // 30% default wet mix
{
    // Set parameter defaults
    parameters[RoomSizeParam].store(roomSize);
    parameters[DampingParam].store(dampingAmount);
    parameters[WidthParam].store(stereoWidth);
    parameters[DryWetParam].store(dryWetMix);
    
    // Initialize reverb parameters
    reverbParams.roomSize = roomSize;
    reverbParams.damping = dampingAmount;
    reverbParams.wetLevel = dryWetMix;
    reverbParams.dryLevel = 1.0f - dryWetMix;
    reverbParams.width = stereoWidth;
    reverbParams.freezeMode = 0.0f;
    
    // Configure the reverb with initial parameters
    reverb.setParameters(reverbParams);
}

Reverb::~Reverb()
{
    // No special cleanup needed
}

void Reverb::setRoomSize(float size)
{
    // Clamp to 0-1 range
    roomSize = juce::jlimit(0.0f, 1.0f, size);
    parameters[RoomSizeParam].store(roomSize);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        roomSizeSmoothed.setTargetValue(roomSize);
        updateParameters();
    }
}

void Reverb::setDamping(float damping)
{
    // Clamp to 0-1 range
    dampingAmount = juce::jlimit(0.0f, 1.0f, damping);
    parameters[DampingParam].store(dampingAmount);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        dampingSmoothed.setTargetValue(dampingAmount);
        updateParameters();
    }
}

void Reverb::setWidth(float width)
{
    // Clamp to 0-1 range
    stereoWidth = juce::jlimit(0.0f, 1.0f, width);
    parameters[WidthParam].store(stereoWidth);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        widthSmoothed.setTargetValue(stereoWidth);
        updateParameters();
    }
}

void Reverb::setDryWet(float dryWet)
{
    // Clamp to 0-1 range
    dryWetMix = juce::jlimit(0.0f, 1.0f, dryWet);
    parameters[DryWetParam].store(dryWetMix);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        dryWetSmoothed.setTargetValue(dryWetMix);
        updateParameters();
    }
}

void Reverb::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    ProcessorNode::prepareToPlay(sampleRate, samplesPerBlock);
    
    // Set up process spec
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = 2;  // Stereo by default
    
    // Initialize parameter smoothing
    roomSizeSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    roomSizeSmoothed.setCurrentAndTargetValue(roomSize);
    
    dampingSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    dampingSmoothed.setCurrentAndTargetValue(dampingAmount);
    
    widthSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    widthSmoothed.setCurrentAndTargetValue(stereoWidth);
    
    dryWetSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    dryWetSmoothed.setCurrentAndTargetValue(dryWetMix);
    
    // Prepare the reverb processor
    reverb.setSampleRate(sampleRate);
    
    // Update parameters for initial settings
    updateParameters();
}

void Reverb::releaseResources()
{
    ProcessorNode::releaseResources();
}

void Reverb::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Use the SIMD-optimized processing
    processBlockSIMD(buffer, midiMessages);
}

void Reverb::processBlockSIMD(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Create a temporary buffer for the dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    // Check if parameters have changed
    bool paramsChanged = false;
    
    // Update room size
    if (!roomSizeSmoothed.isSmoothing())
    {
        float newSize = parameters[RoomSizeParam].load();
        if (std::abs(newSize - roomSize) > 0.01f)
        {
            roomSize = newSize;
            roomSizeSmoothed.setTargetValue(roomSize);
            paramsChanged = true;
        }
    }
    else
    {
        // If still smoothing, get the next value
        roomSizeSmoothed.skip(numSamples - 1);
        float newSize = roomSizeSmoothed.getNextValue();
        if (std::abs(newSize - roomSize) > 0.01f)
        {
            roomSize = newSize;
            paramsChanged = true;
        }
    }
    
    // Update damping
    if (!dampingSmoothed.isSmoothing())
    {
        float newDamping = parameters[DampingParam].load();
        if (std::abs(newDamping - dampingAmount) > 0.01f)
        {
            dampingAmount = newDamping;
            dampingSmoothed.setTargetValue(dampingAmount);
            paramsChanged = true;
        }
    }
    else
    {
        // If still smoothing, get the next value
        dampingSmoothed.skip(numSamples - 1);
        float newDamping = dampingSmoothed.getNextValue();
        if (std::abs(newDamping - dampingAmount) > 0.01f)
        {
            dampingAmount = newDamping;
            paramsChanged = true;
        }
    }
    
    // Update width
    if (!widthSmoothed.isSmoothing())
    {
        float newWidth = parameters[WidthParam].load();
        if (std::abs(newWidth - stereoWidth) > 0.01f)
        {
            stereoWidth = newWidth;
            widthSmoothed.setTargetValue(stereoWidth);
            paramsChanged = true;
        }
    }
    else
    {
        // If still smoothing, get the next value
        widthSmoothed.skip(numSamples - 1);
        float newWidth = widthSmoothed.getNextValue();
        if (std::abs(newWidth - stereoWidth) > 0.01f)
        {
            stereoWidth = newWidth;
            paramsChanged = true;
        }
    }
    
    // Update dry/wet mix
    if (!dryWetSmoothed.isSmoothing())
    {
        float newDryWet = parameters[DryWetParam].load();
        if (std::abs(newDryWet - dryWetMix) > 0.01f)
        {
            dryWetMix = newDryWet;
            dryWetSmoothed.setTargetValue(dryWetMix);
            paramsChanged = true;
        }
    }
    else
    {
        // If still smoothing, get the next value
        dryWetSmoothed.skip(numSamples - 1);
        float newDryWet = dryWetSmoothed.getNextValue();
        if (std::abs(newDryWet - dryWetMix) > 0.01f)
        {
            dryWetMix = newDryWet;
            paramsChanged = true;
        }
    }
    
    // Update reverb parameters if needed
    if (paramsChanged)
    {
        updateParameters();
    }
    
    // Process the buffer through the reverb
    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), numSamples);
}

void Reverb::updateParameters()
{
    // Update the reverb parameters
    reverbParams.roomSize = roomSize;
    reverbParams.damping = dampingAmount;
    reverbParams.wetLevel = dryWetMix;
    reverbParams.dryLevel = 1.0f - dryWetMix;
    reverbParams.width = stereoWidth;
    reverbParams.freezeMode = 0.0f;
    
    // Apply the parameters to the reverb processor
    reverb.setParameters(reverbParams);
}
