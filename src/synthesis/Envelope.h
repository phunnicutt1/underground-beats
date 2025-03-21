/*
 * Underground Beats
 * Envelope.h
 * 
 * ADSR envelope generator for amplitude modulation
 */

#pragma once

#include <JuceHeader.h>

namespace UndergroundBeats {

/**
 * @brief Enumeration of envelope stages
 */
enum class EnvelopeStage {
    Idle,
    Attack,
    Decay,
    Sustain,
    Release
};

/**
 * @class Envelope
 * @brief ADSR envelope generator for modulating amplitude
 * 
 * The Envelope class implements an ADSR (Attack, Decay, Sustain, Release) envelope
 * for modulating the amplitude of a signal over time. It includes options for
 * different curve shapes for each stage and supports sample-accurate timing.
 */
class Envelope {
public:
    Envelope();
    ~Envelope();
    
    /**
     * @brief Set the attack time
     * 
     * @param timeMs Attack time in milliseconds
     */
    void setAttackTime(float timeMs);
    
    /**
     * @brief Set the decay time
     * 
     * @param timeMs Decay time in milliseconds
     */
    void setDecayTime(float timeMs);
    
    /**
     * @brief Set the sustain level
     * 
     * @param level Sustain level (0 to 1)
     */
    void setSustainLevel(float level);
    
    /**
     * @brief Set the release time
     * 
     * @param timeMs Release time in milliseconds
     */
    void setReleaseTime(float timeMs);
    
    /**
     * @brief Get the current envelope stage
     * 
     * @return The current envelope stage
     */
    EnvelopeStage getCurrentStage() const;
    
    /**
     * @brief Get the current envelope value
     * 
     * @return The current envelope value (0 to 1)
     */
    float getCurrentValue() const;
    
    /**
     * @brief Check if the envelope is active
     * 
     * @return true if envelope is in any stage other than Idle
     */
    bool isActive() const;
    
    /**
     * @brief Trigger the envelope (start attack phase)
     */
    void noteOn();
    
    /**
     * @brief Release the envelope (start release phase)
     */
    void noteOff();
    
    /**
     * @brief Get the next envelope sample
     * 
     * @return The next envelope value (0 to 1)
     */
    float getNextSample();
    
    /**
     * @brief Process a buffer of samples with the envelope
     * 
     * @param buffer Buffer to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Apply envelope to an input buffer and write to output buffer
     * 
     * @param inputBuffer Buffer containing input samples
     * @param outputBuffer Buffer to write processed samples to
     * @param numSamples Number of samples to process
     */
    void process(const float* inputBuffer, float* outputBuffer, int numSamples);
    
    /**
     * @brief Prepare the envelope for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Reset the envelope to idle state
     */
    void reset();
    
private:
    // ADSR parameters
    float attackTime;   // in milliseconds
    float decayTime;    // in milliseconds
    float sustainLevel; // 0 to 1
    float releaseTime;  // in milliseconds
    
    // Envelope state
    EnvelopeStage currentStage;
    float currentValue;
    float currentSampleRate;
    
    // Sample counters for each stage
    int attackSamples;
    int decaySamples;
    int releaseSamples;
    
    // Current position in the envelope
    int currentSampleIndex;
    
    // Last output value (used for release)
    float releaseStartValue;
    
    // Recalculate internal sample counts based on time values
    void updateSampleCounts();
    
    // Calculate the next value for the current stage
    float calculateNextValue();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Envelope)
};

} // namespace UndergroundBeats
