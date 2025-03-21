/*
 * Underground Beats
 * Filter.h
 * 
 * Multi-mode filter implementation
 */

#pragma once

#include <JuceHeader.h>

namespace UndergroundBeats {

/**
 * @brief Enumeration of filter types
 */
enum class FilterType {
    LowPass,
    HighPass,
    BandPass,
    Notch,
    LowShelf,
    HighShelf,
    Peak
};

/**
 * @class Filter
 * @brief Multi-mode filter with various filter types and resonance control
 * 
 * The Filter class implements various filter types including low-pass, high-pass,
 * band-pass, and notch filters. It provides control over cutoff frequency,
 * resonance, and filter type, and includes methods for processing audio data.
 */
class Filter {
public:
    Filter();
    ~Filter();
    
    /**
     * @brief Set the filter type
     * 
     * @param type The filter type to use
     */
    void setType(FilterType type);
    
    /**
     * @brief Get the current filter type
     * 
     * @return The current filter type
     */
    FilterType getType() const;
    
    /**
     * @brief Set the filter cutoff frequency
     * 
     * @param frequencyHz Cutoff frequency in Hertz
     */
    void setCutoff(float frequencyHz);
    
    /**
     * @brief Get the current cutoff frequency
     * 
     * @return The current cutoff frequency in Hertz
     */
    float getCutoff() const;
    
    /**
     * @brief Set the filter resonance
     * 
     * @param amount Resonance amount (0 to 1)
     */
    void setResonance(float amount);
    
    /**
     * @brief Get the current resonance amount
     * 
     * @return The current resonance amount
     */
    float getResonance() const;
    
    /**
     * @brief Set the filter gain (for shelf and peak filters)
     * 
     * @param gainDb Gain in decibels
     */
    void setGain(float gainDb);
    
    /**
     * @brief Get the current filter gain
     * 
     * @return The current gain in decibels
     */
    float getGain() const;
    
    /**
     * @brief Process a single sample through the filter
     * 
     * @param sample The input sample
     * @return The filtered sample
     */
    float processSample(float sample);
    
    /**
     * @brief Process a buffer of samples through the filter
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples through the filter
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the filter for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Reset the filter state
     */
    void reset();
    
private:
    // Filter parameters
    FilterType filterType;
    float cutoffFrequency;
    float resonance;
    float gain;
    double currentSampleRate;
    
    // Filter coefficients
    float a0, a1, a2, b1, b2;
    
    // Filter state
    float z1, z2; // Delay line for left/mono channel
    float z1Right, z2Right; // Delay line for right channel (stereo)
    
    // Update filter coefficients based on current parameters
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filter)
};

} // namespace UndergroundBeats
