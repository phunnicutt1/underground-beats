/*
 * Underground Beats
 * Envelope.cpp
 * 
 * Implementation of ADSR envelope generator
 */

#include "Envelope.h"

namespace UndergroundBeats {

Envelope::Envelope()
    : attackTime(10.0f)
    , decayTime(100.0f)
    , sustainLevel(0.7f)
    , releaseTime(200.0f)
    , currentStage(EnvelopeStage::Idle)
    , currentValue(0.0f)
    , currentSampleRate(44100.0)
    , attackSamples(0)
    , decaySamples(0)
    , releaseSamples(0)
    , currentSampleIndex(0)
    , releaseStartValue(0.0f)
{
    updateSampleCounts();
}

Envelope::~Envelope()
{
}

void Envelope::setAttackTime(float timeMs)
{
    attackTime = timeMs;
    updateSampleCounts();
}

void Envelope::setDecayTime(float timeMs)
{
    decayTime = timeMs;
    updateSampleCounts();
}

void Envelope::setSustainLevel(float level)
{
    // Clamp sustain level to 0-1 range
    sustainLevel = juce::jlimit(0.0f, 1.0f, level);
}

void Envelope::setReleaseTime(float timeMs)
{
    releaseTime = timeMs;
    updateSampleCounts();
}

EnvelopeStage Envelope::getCurrentStage() const
{
    return currentStage;
}

float Envelope::getCurrentValue() const
{
    return currentValue;
}

bool Envelope::isActive() const
{
    return currentStage != EnvelopeStage::Idle;
}

void Envelope::noteOn()
{
    // Start the attack phase
    currentStage = EnvelopeStage::Attack;
    currentSampleIndex = 0;
    
    // If we're already in the middle of the envelope, start attack from current value
    // Otherwise start from 0
    if (currentValue <= 0.0f)
    {
        currentValue = 0.0f;
    }
}

void Envelope::noteOff()
{
    // Only enter release phase if we're currently in a active phase
    if (currentStage != EnvelopeStage::Idle)
    {
        currentStage = EnvelopeStage::Release;
        currentSampleIndex = 0;
        releaseStartValue = currentValue;
    }
}

float Envelope::getNextSample()
{
    if (currentStage == EnvelopeStage::Idle)
    {
        return 0.0f;
    }
    
    // Calculate the next value based on the current stage
    currentValue = calculateNextValue();
    currentSampleIndex++;
    
    // Check for stage transitions
    switch (currentStage)
    {
        case EnvelopeStage::Attack:
            if (currentSampleIndex >= attackSamples)
            {
                currentStage = EnvelopeStage::Decay;
                currentSampleIndex = 0;
            }
            break;
            
        case EnvelopeStage::Decay:
            if (currentSampleIndex >= decaySamples)
            {
                currentStage = EnvelopeStage::Sustain;
                currentSampleIndex = 0;
                currentValue = sustainLevel;
            }
            break;
            
        case EnvelopeStage::Sustain:
            // Sustain stage holds the value until noteOff is called
            currentValue = sustainLevel;
            break;
            
        case EnvelopeStage::Release:
            if (currentSampleIndex >= releaseSamples)
            {
                currentStage = EnvelopeStage::Idle;
                currentSampleIndex = 0;
                currentValue = 0.0f;
            }
            break;
            
        default:
            break;
    }
    
    return currentValue;
}

void Envelope::process(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = getNextSample();
    }
}

void Envelope::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        outputBuffer[i] = inputBuffer[i] * getNextSample();
    }
}

void Envelope::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updateSampleCounts();
}

void Envelope::reset()
{
    currentStage = EnvelopeStage::Idle;
    currentValue = 0.0f;
    currentSampleIndex = 0;
}

void Envelope::updateSampleCounts()
{
    // Convert times from milliseconds to samples
    attackSamples = static_cast<int>((attackTime / 1000.0f) * currentSampleRate);
    decaySamples = static_cast<int>((decayTime / 1000.0f) * currentSampleRate);
    releaseSamples = static_cast<int>((releaseTime / 1000.0f) * currentSampleRate);
    
    // Ensure at least 1 sample for each stage
    attackSamples = std::max(1, attackSamples);
    decaySamples = std::max(1, decaySamples);
    releaseSamples = std::max(1, releaseSamples);
}

float Envelope::calculateNextValue()
{
    float output = 0.0f;
    
    switch (currentStage)
    {
        case EnvelopeStage::Attack:
            // Linear attack from current value to 1.0
            if (attackSamples <= 1)
            {
                output = 1.0f;
            }
            else
            {
                // Use a slight curve (cubic) for more natural attack
                float progress = static_cast<float>(currentSampleIndex) / static_cast<float>(attackSamples);
                float curvedProgress = progress * progress * (3.0f - 2.0f * progress); // Cubic interpolation
                output = currentValue + (1.0f - currentValue) * curvedProgress;
            }
            break;
            
        case EnvelopeStage::Decay:
            // Exponential decay from 1.0 to sustain level
            if (decaySamples <= 1)
            {
                output = sustainLevel;
            }
            else
            {
                float progress = static_cast<float>(currentSampleIndex) / static_cast<float>(decaySamples);
                // Exponential curve for more natural decay
                output = 1.0f - (1.0f - sustainLevel) * progress;
            }
            break;
            
        case EnvelopeStage::Sustain:
            // Just hold the sustain level
            output = sustainLevel;
            break;
            
        case EnvelopeStage::Release:
            // Exponential release from current value to 0
            if (releaseSamples <= 1)
            {
                output = 0.0f;
            }
            else
            {
                float progress = static_cast<float>(currentSampleIndex) / static_cast<float>(releaseSamples);
                // Exponential curve for more natural release
                output = releaseStartValue * (1.0f - progress);
            }
            break;
            
        default:
            output = 0.0f;
            break;
    }
    
    return output;
}

} // namespace UndergroundBeats
