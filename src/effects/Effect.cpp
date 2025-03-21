/*
 * Underground Beats
 * Effect.cpp
 * 
 * Implementation of base class for all audio effects
 */

#include "Effect.h"

namespace UndergroundBeats {

Effect::Effect(const std::string& name)
    : effectName(name)
    , enabled(true)
    , mixLevel(1.0f)
    , currentSampleRate(44100.0)
    , currentBlockSize(512)
{
    // Initialize temporary buffer for wet/dry mixing
    tempBuffer.setSize(2, currentBlockSize);
}

Effect::~Effect()
{
}

std::string Effect::getName() const
{
    return effectName;
}

void Effect::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

bool Effect::isEnabled() const
{
    return enabled;
}

void Effect::setMix(float mix)
{
    // Clamp mix level to 0-1 range
    mixLevel = juce::jlimit(0.0f, 1.0f, mix);
}

float Effect::getMix() const
{
    return mixLevel;
}

void Effect::process(float* buffer, int numSamples)
{
    if (!enabled || mixLevel <= 0.0f)
    {
        // Effect is disabled or fully dry, do nothing
        return;
    }
    
    if (mixLevel >= 1.0f)
    {
        // Effect is fully wet, process in-place
        processBuffer(buffer, numSamples);
        return;
    }
    
    // Ensure temp buffer is large enough
    if (tempBuffer.getNumSamples() < numSamples)
    {
        tempBuffer.setSize(2, numSamples, false, true, true);
    }
    
    // Copy input to temp buffer for wet processing
    float* tempData = tempBuffer.getWritePointer(0);
    std::copy(buffer, buffer + numSamples, tempData);
    
    // Process the wet signal
    processBuffer(tempData, numSamples);
    
    // Mix wet and dry signals
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = buffer[i] * (1.0f - mixLevel) + tempData[i] * mixLevel;
    }
}

void Effect::processStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    if (!enabled || mixLevel <= 0.0f)
    {
        // Effect is disabled or fully dry, do nothing
        return;
    }
    
    if (mixLevel >= 1.0f)
    {
        // Effect is fully wet, process in-place
        processBufferStereo(leftBuffer, rightBuffer, numSamples);
        return;
    }
    
    // Ensure temp buffer is large enough
    if (tempBuffer.getNumSamples() < numSamples)
    {
        tempBuffer.setSize(2, numSamples, false, true, true);
    }
    
    // Copy input to temp buffer for wet processing
    float* tempLeft = tempBuffer.getWritePointer(0);
    float* tempRight = tempBuffer.getWritePointer(1);
    std::copy(leftBuffer, leftBuffer + numSamples, tempLeft);
    std::copy(rightBuffer, rightBuffer + numSamples, tempRight);
    
    // Process the wet signal
    processBufferStereo(tempLeft, tempRight, numSamples);
    
    // Mix wet and dry signals
    for (int i = 0; i < numSamples; ++i)
    {
        leftBuffer[i] = leftBuffer[i] * (1.0f - mixLevel) + tempLeft[i] * mixLevel;
        rightBuffer[i] = rightBuffer[i] * (1.0f - mixLevel) + tempRight[i] * mixLevel;
    }
}

void Effect::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
    
    // Resize temporary buffer
    tempBuffer.setSize(2, blockSize, false, true, true);
    
    reset();
}

void Effect::reset()
{
    // Clear the temporary buffer
    tempBuffer.clear();
}

std::unique_ptr<juce::XmlElement> Effect::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("Effect");
    
    xml->setAttribute("name", effectName);
    xml->setAttribute("enabled", enabled);
    xml->setAttribute("mix", mixLevel);
    
    return xml;
}

bool Effect::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "Effect")
    {
        return false;
    }
    
    if (xml->hasAttribute("enabled"))
    {
        enabled = xml->getBoolAttribute("enabled", true);
    }
    
    if (xml->hasAttribute("mix"))
    {
        mixLevel = xml->getDoubleAttribute("mix", 1.0f);
    }
    
    return true;
}

void Effect::processBuffer(float* buffer, int numSamples)
{
    // Default implementation processes samples one by one
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = processSample(buffer[i]);
    }
}

void Effect::processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Default implementation processes samples one by one
    for (int i = 0; i < numSamples; ++i)
    {
        float leftOut, rightOut;
        processSampleStereo(leftBuffer[i], rightBuffer[i], &leftOut, &rightOut);
        leftBuffer[i] = leftOut;
        rightBuffer[i] = rightOut;
    }
}

} // namespace UndergroundBeats
