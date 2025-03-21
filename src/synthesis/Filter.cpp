/*
 * Underground Beats
 * Filter.cpp
 * 
 * Implementation of multi-mode filter
 */

#include "Filter.h"
#include <cmath>

namespace UndergroundBeats {

Filter::Filter()
    : filterType(FilterType::LowPass)
    , cutoffFrequency(1000.0f)
    , resonance(0.5f)
    , gain(0.0f)
    , currentSampleRate(44100.0)
    , a0(1.0f), a1(0.0f), a2(0.0f), b1(0.0f), b2(0.0f)
    , z1(0.0f), z2(0.0f)
    , z1Right(0.0f), z2Right(0.0f)
{
    updateCoefficients();
}

Filter::~Filter()
{
}

void Filter::setType(FilterType type)
{
    if (filterType != type)
    {
        filterType = type;
        updateCoefficients();
    }
}

FilterType Filter::getType() const
{
    return filterType;
}

void Filter::setCutoff(float frequencyHz)
{
    // Limit to Nyquist frequency
    float nyquist = static_cast<float>(currentSampleRate) * 0.5f;
    frequencyHz = juce::jlimit(20.0f, nyquist, frequencyHz);
    
    if (cutoffFrequency != frequencyHz)
    {
        cutoffFrequency = frequencyHz;
        updateCoefficients();
    }
}

float Filter::getCutoff() const
{
    return cutoffFrequency;
}

void Filter::setResonance(float amount)
{
    // Limit resonance to a reasonable range
    amount = juce::jlimit(0.0f, 0.99f, amount);
    
    if (resonance != amount)
    {
        resonance = amount;
        updateCoefficients();
    }
}

float Filter::getResonance() const
{
    return resonance;
}

void Filter::setGain(float gainDb)
{
    if (gain != gainDb)
    {
        gain = gainDb;
        updateCoefficients();
    }
}

float Filter::getGain() const
{
    return gain;
}

float Filter::processSample(float sample)
{
    // Direct form II transposed implementation
    float output = a0 * sample + z1;
    z1 = a1 * sample - b1 * output + z2;
    z2 = a2 * sample - b2 * output;
    
    return output;
}

void Filter::process(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = processSample(buffer[i]);
    }
}

void Filter::processStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        // Process left channel
        float outputL = a0 * leftBuffer[i] + z1;
        z1 = a1 * leftBuffer[i] - b1 * outputL + z2;
        z2 = a2 * leftBuffer[i] - b2 * outputL;
        leftBuffer[i] = outputL;
        
        // Process right channel
        float outputR = a0 * rightBuffer[i] + z1Right;
        z1Right = a1 * rightBuffer[i] - b1 * outputR + z2Right;
        z2Right = a2 * rightBuffer[i] - b2 * outputR;
        rightBuffer[i] = outputR;
    }
}

void Filter::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updateCoefficients();
    reset();
}

void Filter::reset()
{
    z1 = z2 = z1Right = z2Right = 0.0f;
}

void Filter::updateCoefficients()
{
    // Normalize cutoff frequency to [0, 1] range
    float omega = 2.0f * juce::MathConstants<float>::pi * cutoffFrequency / static_cast<float>(currentSampleRate);
    float cosOmega = std::cos(omega);
    float sinOmega = std::sin(omega);
    float alpha = sinOmega / (2.0f * (1.0f - resonance));
    
    // Convert gain from dB to linear
    float gainLinear = std::pow(10.0f, gain / 20.0f);
    float sqrtGain = std::sqrt(gainLinear);
    
    // Calculate the filter coefficients based on the filter type
    switch (filterType)
    {
        case FilterType::LowPass:
            // Low pass filter
            a0 = (1.0f - cosOmega) / 2.0f;
            a1 = 1.0f - cosOmega;
            a2 = (1.0f - cosOmega) / 2.0f;
            b1 = -2.0f * cosOmega;
            b2 = 1.0f - alpha;
            break;
            
        case FilterType::HighPass:
            // High pass filter
            a0 = (1.0f + cosOmega) / 2.0f;
            a1 = -(1.0f + cosOmega);
            a2 = (1.0f + cosOmega) / 2.0f;
            b1 = -2.0f * cosOmega;
            b2 = 1.0f - alpha;
            break;
            
        case FilterType::BandPass:
            // Band pass filter
            a0 = alpha;
            a1 = 0.0f;
            a2 = -alpha;
            b1 = -2.0f * cosOmega;
            b2 = 1.0f - alpha;
            break;
            
        case FilterType::Notch:
            // Notch filter
            a0 = 1.0f;
            a1 = -2.0f * cosOmega;
            a2 = 1.0f;
            b1 = -2.0f * cosOmega;
            b2 = 1.0f - alpha;
            break;
            
        case FilterType::LowShelf:
            // Low shelf filter
            a0 = gainLinear * ((gainLinear + 1.0f) - (gainLinear - 1.0f) * cosOmega + 2.0f * sqrtGain * alpha);
            a1 = 2.0f * gainLinear * ((gainLinear - 1.0f) - (gainLinear + 1.0f) * cosOmega);
            a2 = gainLinear * ((gainLinear + 1.0f) - (gainLinear - 1.0f) * cosOmega - 2.0f * sqrtGain * alpha);
            b1 = -2.0f * ((gainLinear - 1.0f) + (gainLinear + 1.0f) * cosOmega);
            b2 = (gainLinear + 1.0f) + (gainLinear - 1.0f) * cosOmega - 2.0f * sqrtGain * alpha;
            break;
            
        case FilterType::HighShelf:
            // High shelf filter
            a0 = gainLinear * ((gainLinear + 1.0f) + (gainLinear - 1.0f) * cosOmega + 2.0f * sqrtGain * alpha);
            a1 = -2.0f * gainLinear * ((gainLinear - 1.0f) + (gainLinear + 1.0f) * cosOmega);
            a2 = gainLinear * ((gainLinear + 1.0f) + (gainLinear - 1.0f) * cosOmega - 2.0f * sqrtGain * alpha);
            b1 = 2.0f * ((gainLinear - 1.0f) - (gainLinear + 1.0f) * cosOmega);
            b2 = (gainLinear + 1.0f) - (gainLinear - 1.0f) * cosOmega - 2.0f * sqrtGain * alpha;
            break;
            
        case FilterType::Peak:
            // Peak filter
            a0 = 1.0f + alpha * gainLinear;
            a1 = -2.0f * cosOmega;
            a2 = 1.0f - alpha * gainLinear;
            b1 = -2.0f * cosOmega;
            b2 = 1.0f - alpha / gainLinear;
            break;
    }
    
    // Normalize the coefficients
    float norm = 1.0f / (1.0f + b1 + b2);
    a0 *= norm;
    a1 *= norm;
    a2 *= norm;
    b1 *= norm;
    b2 *= norm;
}

} // namespace UndergroundBeats
