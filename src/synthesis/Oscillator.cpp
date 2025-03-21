/*
 * Underground Beats
 * Oscillator.cpp
 * 
 * Implementation of oscillator waveform generation
 */

#include "Oscillator.h"

namespace UndergroundBeats {

Oscillator::Oscillator()
    : waveformType(WaveformType::Sine)
    , frequency(440.0f)
    , phase(0.0f)
    , phaseIncrement(0.0f)
    , currentSampleRate(44100.0)
    , wavetableSize(0)
    , lastOutput(0.0f)
{
    updatePhaseIncrement();
}

Oscillator::~Oscillator()
{
}

void Oscillator::setWaveform(WaveformType type)
{
    waveformType = type;
}

WaveformType Oscillator::getWaveform() const
{
    return waveformType;
}

void Oscillator::setFrequency(float frequencyHz)
{
    frequency = frequencyHz;
    updatePhaseIncrement();
}

float Oscillator::getFrequency() const
{
    return frequency;
}

void Oscillator::setPhase(float newPhase)
{
    // Normalize phase to 0-1 range
    phase = std::fmod(newPhase, juce::MathConstants<float>::twoPi);
    if (phase < 0.0f)
        phase += juce::MathConstants<float>::twoPi;
}

float Oscillator::getPhase() const
{
    return phase;
}

void Oscillator::resetPhase(float newPhase)
{
    setPhase(newPhase);
}

void Oscillator::setWavetable(const float* wavetable, int size)
{
    if (size > 0 && wavetable != nullptr)
    {
        wavetableData.resize(size);
        std::copy(wavetable, wavetable + size, wavetableData.begin());
        wavetableSize = size;
    }
}

float Oscillator::getSample(float frequencyModulation)
{
    // Apply frequency modulation (scale by octave range, e.g., -1 to 1 = one octave down to one octave up)
    float modulatedPhaseIncrement = phaseIncrement;
    if (frequencyModulation != 0.0f)
    {
        // Convert -1 to 1 range to a frequency multiplier (0.5 to 2.0 for one octave)
        float multiplier = std::pow(2.0f, frequencyModulation);
        modulatedPhaseIncrement *= multiplier;
    }
    
    // Generate sample based on current waveform type
    float output = 0.0f;
    
    switch (waveformType)
    {
        case WaveformType::Sine:
            output = generateSine(phase);
            break;
        case WaveformType::Triangle:
            output = generateTriangle(phase);
            break;
        case WaveformType::Sawtooth:
            output = generateSawtooth(phase);
            break;
        case WaveformType::Square:
            output = generateSquare(phase);
            break;
        case WaveformType::Noise:
            output = generateNoise();
            break;
        case WaveformType::Wavetable:
            output = generateWavetable(phase);
            break;
    }
    
    // Update phase for next sample
    phase += modulatedPhaseIncrement;
    
    // Keep phase in the range [0, 2π)
    while (phase >= juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;
    
    lastOutput = output;
    return output;
}

void Oscillator::process(float* buffer, int numSamples, const float* frequencyModulation)
{
    if (frequencyModulation == nullptr)
    {
        // No frequency modulation, generate samples directly
        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = getSample(0.0f);
        }
    }
    else
    {
        // Apply per-sample frequency modulation
        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = getSample(frequencyModulation[i]);
        }
    }
}

void Oscillator::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updatePhaseIncrement();
}

void Oscillator::updatePhaseIncrement()
{
    // Calculate phase increment based on frequency and sample rate
    // Phase increment = (2π * frequency) / sampleRate
    phaseIncrement = (juce::MathConstants<float>::twoPi * frequency) / static_cast<float>(currentSampleRate);
}

// Waveform generation methods

float Oscillator::generateSine(float phase)
{
    return std::sin(phase);
}

float Oscillator::generateTriangle(float phase)
{
    // Convert phase to a value between -1 and 1
    float normalizedPhase = phase / juce::MathConstants<float>::pi - 1.0f;
    
    // Take absolute value and multiply by 2 to get range [0,2]
    float value = std::abs(normalizedPhase) * 2.0f;
    
    // Shift to get range [-1,1]
    return 1.0f - value;
}

float Oscillator::generateSawtooth(float phase)
{
    // Basic sawtooth: phase / π - 1 for range [-1,1]
    float rawSawtooth = phase / juce::MathConstants<float>::pi - 1.0f;
    
    // Simple anti-aliasing using first-order differentiation
    // This is a basic approach; for better results, PolyBLEP or other
    // techniques should be implemented
    return rawSawtooth;
}

float Oscillator::generateSquare(float phase)
{
    // Basic square wave
    return (phase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;
}

float Oscillator::generateNoise()
{
    // Simple white noise
    return juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
}

float Oscillator::generateWavetable(float phase)
{
    if (wavetableSize == 0)
    {
        // Fall back to sine if no wavetable is set
        return generateSine(phase);
    }
    
    // Convert phase to wavetable index
    float index = (phase / juce::MathConstants<float>::twoPi) * wavetableSize;
    
    // Get indices for interpolation
    int index1 = static_cast<int>(index) % wavetableSize;
    int index2 = (index1 + 1) % wavetableSize;
    
    // Linear interpolation between adjacent samples
    float fraction = index - static_cast<float>(index1);
    return wavetableData[index1] * (1.0f - fraction) + wavetableData[index2] * fraction;
}

} // namespace UndergroundBeats
