/*
 * Underground Beats
 * Reverb.cpp
 * 
 * Implementation of reverb effect
 */

#include "Reverb.h"

namespace UndergroundBeats {

Reverb::Reverb()
    : Effect("Reverb")
    , roomSize(0.5f)
    , damping(0.5f)
    , width(1.0f)
    , freeze(false)
{
    updateParameters();
}

Reverb::~Reverb()
{
}

void Reverb::setRoomSize(float size)
{
    roomSize = juce::jlimit(0.0f, 1.0f, size);
    updateParameters();
}

float Reverb::getRoomSize() const
{
    return roomSize;
}

void Reverb::setDamping(float amount)
{
    damping = juce::jlimit(0.0f, 1.0f, amount);
    updateParameters();
}

float Reverb::getDamping() const
{
    return damping;
}

void Reverb::setWidth(float width)
{
    this->width = juce::jlimit(0.0f, 1.0f, width);
    updateParameters();
}

float Reverb::getWidth() const
{
    return width;
}

void Reverb::setFreeze(bool freeze)
{
    this->freeze = freeze;
    updateParameters();
}

bool Reverb::getFreeze() const
{
    return freeze;
}

void Reverb::prepare(double sampleRate, int blockSize)
{
    Effect::prepare(sampleRate, blockSize);
    
    // Set up the JUCE reverb
    jucereverb.reset();
    jucereverb.setSampleRate(sampleRate);
    
    updateParameters();
}

void Reverb::reset()
{
    Effect::reset();
    
    // Reset the JUCE reverb
    jucereverb.reset();
}

std::unique_ptr<juce::XmlElement> Reverb::createStateXml() const
{
    auto xml = Effect::createStateXml();
    
    // Add reverb-specific attributes
    xml->setAttribute("roomSize", roomSize);
    xml->setAttribute("damping", damping);
    xml->setAttribute("width", width);
    xml->setAttribute("freeze", freeze);
    
    return xml;
}

bool Reverb::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (!Effect::restoreStateFromXml(xml))
    {
        return false;
    }
    
    // Restore reverb-specific attributes
    if (xml->hasAttribute("roomSize"))
    {
        setRoomSize(xml->getDoubleAttribute("roomSize", 0.5f));
    }
    
    if (xml->hasAttribute("damping"))
    {
        setDamping(xml->getDoubleAttribute("damping", 0.5f));
    }
    
    if (xml->hasAttribute("width"))
    {
        setWidth(xml->getDoubleAttribute("width", 1.0f));
    }
    
    if (xml->hasAttribute("freeze"))
    {
        setFreeze(xml->getBoolAttribute("freeze", false));
    }
    
    return true;
}

float Reverb::processSample(float sample)
{
    // Process through the reverb
    // Note: JUCE's reverb is inherently stereo, so we'll process mono as if it's both left and right
    float left = sample;
    float right = sample;
    
    jucereverb.processStereo(&left, &right, 1);
    
    // Return the average of left and right channels
    return (left + right) * 0.5f;
}

void Reverb::processSampleStereo(float leftSample, float rightSample, float* leftOutput, float* rightOutput)
{
    // Process through the reverb
    *leftOutput = leftSample;
    *rightOutput = rightSample;
    
    jucereverb.processStereo(leftOutput, rightOutput, 1);
}

void Reverb::processBuffer(float* buffer, int numSamples)
{
    // Process each sample through the reverb
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = processSample(buffer[i]);
    }
}

void Reverb::processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Process the buffer through the JUCE reverb
    jucereverb.processStereo(leftBuffer, rightBuffer, numSamples);
}

void Reverb::updateParameters()
{
    // Configure JUCE reverb parameters
    juce::Reverb::Parameters params;
    params.roomSize = roomSize;
    params.damping = damping;
    params.wetLevel = 1.0f; // We'll handle dry/wet mix with the base Effect class
    params.dryLevel = 0.0f;
    params.width = width;
    params.freezeMode = freeze ? 1.0f : 0.0f;
    
    jucereverb.setParameters(params);
}

} // namespace UndergroundBeats
