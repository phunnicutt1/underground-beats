/*
 * Underground Beats
 * Oscillator.h
 * 
 * Defines the oscillator classes for various waveform generation
 */

#pragma once

#include <JuceHeader.h>
#include <array>
#include <cmath>

namespace UndergroundBeats {

/**
 * @brief Enumeration of basic oscillator waveform types
 */
enum class WaveformType {
    Sine,
    Triangle,
    Sawtooth,
    Square,
    Noise,
    Wavetable
};

/**
 * @class Oscillator
 * @brief Base class for all oscillator types
 * 
 * The Oscillator class provides a foundation for generating various waveforms.
 * It supports standard waveforms as well as wavetable synthesis, and includes
 * features like frequency modulation and phase offset.
 */
class Oscillator {
public:
    Oscillator();
    virtual ~Oscillator();
    
    /**
     * @brief Set the oscillator's basic waveform type
     * 
     * @param type The waveform type to use
     */
    void setWaveform(WaveformType type);
    
    /**
     * @brief Get the current waveform type
     * 
     * @return The current waveform type
     */
    WaveformType getWaveform() const;
    
    /**
     * @brief Set the oscillator's base frequency
     * 
     * @param frequencyHz The frequency in Hertz
     */
    void setFrequency(float frequencyHz);
    
    /**
     * @brief Get the oscillator's current frequency
     * 
     * @return The current frequency in Hertz
     */
    float getFrequency() const;
    
    /**
     * @brief Set the oscillator's phase offset
     * 
     * @param newPhase The phase offset in radians (0 to 2π)
     */
    void setPhase(float newPhase);
    
    /**
     * @brief Get the oscillator's current phase
     * 
     * @return The current phase in radians
     */
    float getPhase() const;
    
    /**
     * @brief Reset the oscillator's phase to a specific value
     * 
     * @param newPhase The phase to reset to (default is 0)
     */
    void resetPhase(float newPhase = 0.0f);
    
    /**
     * @brief Set a custom wavetable for use with WaveformType::Wavetable
     * 
     * @param wavetable The wavetable data (should be normalized between -1 and 1)
     * @param size The size of the wavetable
     */
    void setWavetable(const float* wavetable, int size);
    
    /**
     * @brief Generate a single sample
     * 
     * @param frequencyModulation Optional frequency modulation value (-1 to 1)
     * @return The generated sample value (-1 to 1)
     */
    float getSample(float frequencyModulation = 0.0f);
    
    /**
     * @brief Process a buffer of samples
     * 
     * @param buffer The buffer to fill with generated samples
     * @param numSamples The number of samples to generate
     * @param frequencyModulation Optional buffer of frequency modulation values
     */
    void process(float* buffer, int numSamples, const float* frequencyModulation = nullptr);
    
    /**
     * @brief Prepare the oscillator for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
private:
    WaveformType waveformType;
    float frequency;
    float phase;
    float phaseIncrement;
    double currentSampleRate;
    
    // Wavetable data
    std::vector<float> wavetableData;
    int wavetableSize;
    
    // Anti-aliasing state
    float lastOutput;
    
    // Sample generation methods for different waveforms
    float generateSine(float phase);
    float generateTriangle(float phase);
    float generateSawtooth(float phase);
    float generateSquare(float phase);
    float generateNoise();
    float generateWavetable(float phase);
    
    // Utility method to calculate phase increment from frequency
    void updatePhaseIncrement();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator)
};

} // namespace UndergroundBeats
