#include "Delay.h"

Delay::Delay()
    : delayTimeMs(500.0f),      // 500ms default delay
      feedbackAmount(0.5f),     // 50% default feedback
      dryWetMix(0.3f),          // 30% default wet mix
      delayBufferSize(0),
      delayWritePosition(0),
      delaySamples(0)
{
    // Set parameter defaults
    parameters[DelayTimeParam].store(delayTimeMs);
    parameters[FeedbackParam].store(feedbackAmount);
    parameters[DryWetParam].store(dryWetMix);
}

Delay::~Delay()
{
    // No special cleanup needed
}

void Delay::setDelayTime(float newDelayTimeMs)
{
    // Clamp to reasonable delay time
    delayTimeMs = juce::jlimit(10.0f, MAX_DELAY_TIME_MS, newDelayTimeMs);
    parameters[DelayTimeParam].store(delayTimeMs);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        delayTimeSmoothed.setTargetValue(delayTimeMs);
        updateParameters();
    }
}

void Delay::setFeedback(float newFeedback)
{
    // Clamp to prevent unstable feedback (>1.0)
    feedbackAmount = juce::jlimit(0.0f, 0.95f, newFeedback);
    parameters[FeedbackParam].store(feedbackAmount);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        feedbackSmoothed.setTargetValue(feedbackAmount);
    }
}

void Delay::setDryWet(float newDryWet)
{
    // Clamp to 0-1 range
    dryWetMix = juce::jlimit(0.0f, 1.0f, newDryWet);
    parameters[DryWetParam].store(dryWetMix);
    
    // Update smoothed value if prepared
    if (isPrepared)
    {
        dryWetSmoothed.setTargetValue(dryWetMix);
    }
}

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    ProcessorNode::prepareToPlay(sampleRate, samplesPerBlock);
    
    // Set up process spec
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = 2;  // Stereo by default
    
    // Initialize parameter smoothing
    delayTimeSmoothed.reset(sampleRate, 0.1);  // 100ms smoothing for delay time
    delayTimeSmoothed.setCurrentAndTargetValue(delayTimeMs);
    
    feedbackSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    feedbackSmoothed.setCurrentAndTargetValue(feedbackAmount);
    
    dryWetSmoothed.reset(sampleRate, 0.05);  // 50ms smoothing
    dryWetSmoothed.setCurrentAndTargetValue(dryWetMix);
    
    // Calculate and allocate delay buffer
    delayBufferSize = calculateDelayBufferSize(sampleRate, MAX_DELAY_TIME_MS);
    delayBuffer.setSize(processSpec.numChannels, delayBufferSize);
    delayBuffer.clear();
    
    // Initialize delay write position
    delayWritePosition = 0;
    
    // Initialize delay time in samples
    delaySamples = static_cast<int>((delayTimeMs / 1000.0f) * sampleRate);
    
    // Update parameters for initial settings
    updateParameters();
}

void Delay::releaseResources()
{
    ProcessorNode::releaseResources();
}

void Delay::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Use the SIMD-optimized processing
    processBlockSIMD(buffer, midiMessages);
}

void Delay::processBlockSIMD(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Create a temporary buffer for the dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    // Check if delay time has changed
    float currentDelayTime = parameters[DelayTimeParam].load();
    if (std::abs(currentDelayTime - delayTimeMs) > 0.1f)
    {
        delayTimeMs = currentDelayTime;
        delayTimeSmoothed.setTargetValue(delayTimeMs);
        updateParameters();
    }
    
    // Process each channel
    for (int channel = 0; channel < numChannels; ++channel)
    {
        // Get pointers to the current channel data
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer.getWritePointer(channel);
        
        // Process each sample
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get the smoothed parameter values
            float currentDelayTimeSamples = (delayTimeSmoothed.getNextValue() / 1000.0f) * processSpec.sampleRate;
            float currentFeedback = feedbackSmoothed.getNextValue();
            float currentDryWet = dryWetSmoothed.getNextValue();
            
            // Calculate the read position
            int readPosition = delayWritePosition - static_cast<int>(currentDelayTimeSamples);
            if (readPosition < 0)
                readPosition += delayBufferSize;
                
            // Linear interpolation for fractional delay times
            float fraction = currentDelayTimeSamples - std::floor(currentDelayTimeSamples);
            int readPosition2 = (readPosition + 1) % delayBufferSize;
            
            float delayedSample = delayData[readPosition] * (1.0f - fraction) + delayData[readPosition2] * fraction;
            
            // Write the input plus the delayed and fedback signal into the delay buffer
            delayData[delayWritePosition] = channelData[sample] + delayedSample * currentFeedback;
            
            // Mix the dry and wet signals based on the dry/wet mix parameter
            channelData[sample] = channelData[sample] * (1.0f - currentDryWet) + delayedSample * currentDryWet;
        }
        
        // Update the delay write position at the end of the block
        delayWritePosition += numSamples;
        delayWritePosition %= delayBufferSize;
    }
}

int Delay::calculateDelayBufferSize(double sampleRate, float maxDelayMs)
{
    // Calculate the buffer size needed for the maximum delay time
    // Add some extra space to account for block processing
    float maxDelaySamples = (maxDelayMs / 1000.0f) * sampleRate;
    return static_cast<int>(maxDelaySamples) + 512; // Add a safety margin
}

void Delay::updateParameters()
{
    // Calculate the number of samples for the current delay time
    if (processSpec.sampleRate > 0.0)
    {
        delaySamples = static_cast<int>((delayTimeMs / 1000.0f) * processSpec.sampleRate);
    }
}
