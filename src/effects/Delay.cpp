/*
 * Underground Beats
 * Delay.cpp
 * 
 * Implementation of stereo delay effect
 */

#include "Delay.h"

namespace UndergroundBeats {

Delay::Delay()
    : Effect("Delay")
    , delayTimeMs({500.0f, 500.0f})
    , delayTimeSync({DelayTimeSync::Free, DelayTimeSync::Free})
    , feedback({0.5f, 0.5f})
    , crossFeedback({0.0f, 0.0f})
    , tempo(120.0f)
    , writePosition({0, 0})
    , delayLength({0, 0})
{
    // Initialize delay buffers (will be properly sized in prepare())
    for (auto& buffer : delayBuffer)
    {
        buffer = std::make_unique<juce::AudioBuffer<float>>();
    }
}

Delay::~Delay()
{
}

void Delay::setDelayTime(int channel, float timeMs)
{
    if (channel >= 0 && channel < 2)
    {
        delayTimeMs[channel] = timeMs;
        
        // If not in synced mode, update the delay length
        if (delayTimeSync[channel] == DelayTimeSync::Free)
        {
            delayLength[channel] = static_cast<int>((timeMs / 1000.0f) * currentSampleRate);
            
            // Ensure delay length is at least 1 sample
            delayLength[channel] = std::max(1, delayLength[channel]);
            
            // Ensure delay buffer is large enough
            if (delayBuffer[channel]->getNumSamples() < delayLength[channel])
            {
                // Double buffer size for future flexibility
                int newSize = std::max(delayLength[channel] * 2, delayBuffer[channel]->getNumSamples());
                delayBuffer[channel]->setSize(1, newSize, false, true, true);
            }
        }
    }
}

float Delay::getDelayTime(int channel) const
{
    if (channel >= 0 && channel < 2)
    {
        return delayTimeMs[channel];
    }
    
    return 0.0f;
}

void Delay::setDelayTimeSync(int channel, DelayTimeSync syncMode)
{
    if (channel >= 0 && channel < 2)
    {
        delayTimeSync[channel] = syncMode;
        updateDelayTimes();
    }
}

DelayTimeSync Delay::getDelayTimeSync(int channel) const
{
    if (channel >= 0 && channel < 2)
    {
        return delayTimeSync[channel];
    }
    
    return DelayTimeSync::Free;
}

void Delay::setFeedback(int channel, float amount)
{
    if (channel >= 0 && channel < 2)
    {
        feedback[channel] = juce::jlimit(0.0f, 0.99f, amount);
    }
}

float Delay::getFeedback(int channel) const
{
    if (channel >= 0 && channel < 2)
    {
        return feedback[channel];
    }
    
    return 0.0f;
}

void Delay::setCrossFeedback(int channel, float amount)
{
    if (channel >= 0 && channel < 2)
    {
        crossFeedback[channel] = juce::jlimit(0.0f, 0.99f, amount);
    }
}

float Delay::getCrossFeedback(int channel) const
{
    if (channel >= 0 && channel < 2)
    {
        return crossFeedback[channel];
    }
    
    return 0.0f;
}

void Delay::setTempo(float bpm)
{
    tempo = bpm;
    updateDelayTimes();
}

float Delay::getTempo() const
{
    return tempo;
}

void Delay::prepare(double sampleRate, int blockSize)
{
    Effect::prepare(sampleRate, blockSize);
    
    // Calculate maximum delay length (2 seconds)
    int maxDelaySamples = static_cast<int>(2.0 * sampleRate);
    
    // Initialize delay buffers
    for (int channel = 0; channel < 2; ++channel)
    {
        delayBuffer[channel]->setSize(1, maxDelaySamples, false, true, true);
        delayBuffer[channel]->clear();
        writePosition[channel] = 0;
    }
    
    // Update delay times based on current tempo and sync settings
    updateDelayTimes();
}

void Delay::reset()
{
    Effect::reset();
    
    // Clear delay buffers
    for (int channel = 0; channel < 2; ++channel)
    {
        delayBuffer[channel]->clear();
        writePosition[channel] = 0;
    }
}

std::unique_ptr<juce::XmlElement> Delay::createStateXml() const
{
    auto xml = Effect::createStateXml();
    
    // Add delay-specific attributes
    xml->setAttribute("delayTimeLeft", delayTimeMs[0]);
    xml->setAttribute("delayTimeRight", delayTimeMs[1]);
    xml->setAttribute("delayTimeSyncLeft", static_cast<int>(delayTimeSync[0]));
    xml->setAttribute("delayTimeSyncRight", static_cast<int>(delayTimeSync[1]));
    xml->setAttribute("feedbackLeft", feedback[0]);
    xml->setAttribute("feedbackRight", feedback[1]);
    xml->setAttribute("crossFeedbackLeft", crossFeedback[0]);
    xml->setAttribute("crossFeedbackRight", crossFeedback[1]);
    xml->setAttribute("tempo", tempo);
    
    return xml;
}

bool Delay::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (!Effect::restoreStateFromXml(xml))
    {
        return false;
    }
    
    // Restore delay-specific attributes
    if (xml->hasAttribute("delayTimeLeft"))
    {
        setDelayTime(0, xml->getDoubleAttribute("delayTimeLeft", 500.0f));
    }
    
    if (xml->hasAttribute("delayTimeRight"))
    {
        setDelayTime(1, xml->getDoubleAttribute("delayTimeRight", 500.0f));
    }
    
    if (xml->hasAttribute("delayTimeSyncLeft"))
    {
        setDelayTimeSync(0, static_cast<DelayTimeSync>(xml->getIntAttribute("delayTimeSyncLeft", 0)));
    }
    
    if (xml->hasAttribute("delayTimeSyncRight"))
    {
        setDelayTimeSync(1, static_cast<DelayTimeSync>(xml->getIntAttribute("delayTimeSyncRight", 0)));
    }
    
    if (xml->hasAttribute("feedbackLeft"))
    {
        setFeedback(0, xml->getDoubleAttribute("feedbackLeft", 0.5f));
    }
    
    if (xml->hasAttribute("feedbackRight"))
    {
        setFeedback(1, xml->getDoubleAttribute("feedbackRight", 0.5f));
    }
    
    if (xml->hasAttribute("crossFeedbackLeft"))
    {
        setCrossFeedback(0, xml->getDoubleAttribute("crossFeedbackLeft", 0.0f));
    }
    
    if (xml->hasAttribute("crossFeedbackRight"))
    {
        setCrossFeedback(1, xml->getDoubleAttribute("crossFeedbackRight", 0.0f));
    }
    
    if (xml->hasAttribute("tempo"))
    {
        setTempo(xml->getDoubleAttribute("tempo", 120.0f));
    }
    
    return true;
}

float Delay::processSample(float sample)
{
    // For mono processing, just use the left channel settings
    float delayedSample = getSampleFromDelayBuffer(0, delayLength[0]);
    float output = sample + delayedSample;
    
    // Write to delay buffer with feedback
    float* channelData = delayBuffer[0]->getWritePointer(0);
    channelData[writePosition[0]] = sample + delayedSample * feedback[0];
    
    // Increment write position
    writePosition[0] = (writePosition[0] + 1) % delayBuffer[0]->getNumSamples();
    
    return output;
}

void Delay::processSampleStereo(float leftSample, float rightSample, float* leftOutput, float* rightOutput)
{
    // Get delayed samples from both channels
    float delayedLeft = getSampleFromDelayBuffer(0, delayLength[0]);
    float delayedRight = getSampleFromDelayBuffer(1, delayLength[1]);
    
    // Calculate outputs
    *leftOutput = leftSample + delayedLeft;
    *rightOutput = rightSample + delayedRight;
    
    // Write to delay buffers with feedback and cross-feedback
    float* leftChannelData = delayBuffer[0]->getWritePointer(0);
    float* rightChannelData = delayBuffer[1]->getWritePointer(0);
    
    leftChannelData[writePosition[0]] = leftSample + 
                                       delayedLeft * feedback[0] + 
                                       delayedRight * crossFeedback[0];
    
    rightChannelData[writePosition[1]] = rightSample + 
                                        delayedRight * feedback[1] + 
                                        delayedLeft * crossFeedback[1];
    
    // Increment write positions
    writePosition[0] = (writePosition[0] + 1) % delayBuffer[0]->getNumSamples();
    writePosition[1] = (writePosition[1] + 1) % delayBuffer[1]->getNumSamples();
}

void Delay::processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Process samples one by one
    for (int i = 0; i < numSamples; ++i)
    {
        float leftOut, rightOut;
        processSampleStereo(leftBuffer[i], rightBuffer[i], &leftOut, &rightOut);
        leftBuffer[i] = leftOut;
        rightBuffer[i] = rightOut;
    }
}

void Delay::updateDelayTimes()
{
    for (int channel = 0; channel < 2; ++channel)
    {
        if (delayTimeSync[channel] != DelayTimeSync::Free)
        {
            // Calculate delay time based on sync mode and tempo
            float timeMs = syncModeToMs(delayTimeSync[channel], tempo);
            delayTimeMs[channel] = timeMs;
        }
        
        // Update delay length in samples
        delayLength[channel] = static_cast<int>((delayTimeMs[channel] / 1000.0f) * currentSampleRate);
        
        // Ensure delay length is at least 1 sample
        delayLength[channel] = std::max(1, delayLength[channel]);
        
        // Ensure delay buffer is large enough
        if (delayBuffer[channel]->getNumSamples() < delayLength[channel])
        {
            // Double buffer size for future flexibility
            int newSize = std::max(delayLength[channel] * 2, delayBuffer[channel]->getNumSamples());
            juce::AudioBuffer<float> newBuffer(1, newSize);
            
            // Copy existing data if there is any
            if (delayBuffer[channel]->getNumSamples() > 0)
            {
                newBuffer.clear();
                
                // Copy in two parts to handle wrap-around
                int numSamplesToCopy = delayBuffer[channel]->getNumSamples();
                int firstPart = std::min(numSamplesToCopy - writePosition[channel], numSamplesToCopy);
                int secondPart = numSamplesToCopy - firstPart;
                
                newBuffer.copyFrom(0, 0, *delayBuffer[channel], 0, writePosition[channel], firstPart);
                
                if (secondPart > 0)
                {
                    newBuffer.copyFrom(0, firstPart, *delayBuffer[channel], 0, 0, secondPart);
                }
                
                // Reset write position to the beginning of the buffer
                writePosition[channel] = 0;
            }
            
            // Replace the old buffer with the new one
            delayBuffer[channel]->setSize(1, newSize, true, true, true);
            delayBuffer[channel]->copyFrom(0, 0, newBuffer, 0, 0, newBuffer.getNumSamples());
        }
    }
}

float Delay::syncModeToMs(DelayTimeSync mode, float bpm) const
{
    // Convert tempo to milliseconds per beat (quarter note)
    float msPerBeat = 60000.0f / bpm;
    
    // Calculate delay time based on sync mode
    switch (mode)
    {
        case DelayTimeSync::Whole:
            return msPerBeat * 4.0f;
        case DelayTimeSync::Half:
            return msPerBeat * 2.0f;
        case DelayTimeSync::Quarter:
            return msPerBeat;
        case DelayTimeSync::Eighth:
            return msPerBeat * 0.5f;
        case DelayTimeSync::Sixteenth:
            return msPerBeat * 0.25f;
        case DelayTimeSync::DottedHalf:
            return msPerBeat * 3.0f;
        case DelayTimeSync::DottedQuarter:
            return msPerBeat * 1.5f;
        case DelayTimeSync::DottedEighth:
            return msPerBeat * 0.75f;
        case DelayTimeSync::TripletHalf:
            return msPerBeat * (4.0f / 3.0f);
        case DelayTimeSync::TripletQuarter:
            return msPerBeat * (2.0f / 3.0f);
        case DelayTimeSync::TripletEighth:
            return msPerBeat * (1.0f / 3.0f);
        default: // Free
            return 0.0f;
    }
}

float Delay::getSampleFromDelayBuffer(int channel, float delaySamples)
{
    if (delayBuffer[channel]->getNumSamples() == 0)
    {
        return 0.0f;
    }
    
    // Calculate read position
    int bufferLength = delayBuffer[channel]->getNumSamples();
    float readPosition = static_cast<float>(writePosition[channel]) - delaySamples;
    
    // Wrap around if necessary
    while (readPosition < 0.0f)
    {
        readPosition += static_cast<float>(bufferLength);
    }
    
    // Calculate indices for interpolation
    int index1 = static_cast<int>(readPosition);
    int index2 = (index1 + 1) % bufferLength;
    
    // Get the fractional part for interpolation
    float fraction = readPosition - static_cast<float>(index1);
    
    // Get the samples
    float* data = delayBuffer[channel]->getWritePointer(0);
    float sample1 = data[index1];
    float sample2 = data[index2];
    
    // Linear interpolation
    return sample1 * (1.0f - fraction) + sample2 * fraction;
}

} // namespace UndergroundBeats
