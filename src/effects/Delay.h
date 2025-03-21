/*
 * Underground Beats
 * Delay.h
 * 
 * Stereo delay effect with feedback and tempo sync
 */

#pragma once

#include "Effect.h"
#include <array>

namespace UndergroundBeats {

/**
 * @brief Enumeration of tempo sync note values
 */
enum class DelayTimeSync {
    Free,       // Free time (not synced)
    Whole,      // Whole note
    Half,       // Half note
    Quarter,    // Quarter note
    Eighth,     // Eighth note
    Sixteenth,  // Sixteenth note
    DottedHalf, // Dotted half note
    DottedQuarter, // Dotted quarter note
    DottedEighth,  // Dotted eighth note
    TripletHalf,   // Triplet half note
    TripletQuarter,// Triplet quarter note
    TripletEighth  // Triplet eighth note
};

/**
 * @class Delay
 * @brief Stereo delay effect with feedback and tempo sync
 * 
 * The Delay class implements a stereo delay effect with adjustable delay time,
 * feedback, and cross-feedback between channels. It supports both free time
 * and tempo-synced delay times.
 */
class Delay : public Effect {
public:
    Delay();
    ~Delay() override;
    
    /**
     * @brief Set the delay time for a channel
     * 
     * @param channel Channel to set (0 = left, 1 = right)
     * @param timeMs Delay time in milliseconds
     */
    void setDelayTime(int channel, float timeMs);
    
    /**
     * @brief Get the current delay time for a channel
     * 
     * @param channel Channel to get (0 = left, 1 = right)
     * @return Delay time in milliseconds
     */
    float getDelayTime(int channel) const;
    
    /**
     * @brief Set the delay time sync mode
     * 
     * @param channel Channel to set (0 = left, 1 = right)
     * @param syncMode The sync mode to use
     */
    void setDelayTimeSync(int channel, DelayTimeSync syncMode);
    
    /**
     * @brief Get the current delay time sync mode
     * 
     * @param channel Channel to get (0 = left, 1 = right)
     * @return The current sync mode
     */
    DelayTimeSync getDelayTimeSync(int channel) const;
    
    /**
     * @brief Set the feedback amount
     * 
     * @param channel Channel to set (0 = left, 1 = right)
     * @param amount Feedback amount (0 to 1)
     */
    void setFeedback(int channel, float amount);
    
    /**
     * @brief Get the current feedback amount
     * 
     * @param channel Channel to get (0 = left, 1 = right)
     * @return The current feedback amount
     */
    float getFeedback(int channel) const;
    
    /**
     * @brief Set the cross-feedback amount
     * 
     * @param channel Channel to set (0 = left, 1 = right)
     * @param amount Cross-feedback amount (0 to 1)
     */
    void setCrossFeedback(int channel, float amount);
    
    /**
     * @brief Get the current cross-feedback amount
     * 
     * @param channel Channel to get (0 = left, 1 = right)
     * @return The current cross-feedback amount
     */
    float getCrossFeedback(int channel) const;
    
    /**
     * @brief Set the current tempo (for synced delay times)
     * 
     * @param bpm Tempo in beats per minute
     */
    void setTempo(float bpm);
    
    /**
     * @brief Get the current tempo
     * 
     * @return The current tempo in beats per minute
     */
    float getTempo() const;
    
    /**
     * @brief Prepare the effect for processing
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    void prepare(double sampleRate, int blockSize) override;
    
    /**
     * @brief Reset the effect state
     */
    void reset() override;
    
    /**
     * @brief Create an XML element containing the effect's state
     * 
     * @return XML element containing effect state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const override;
    
    /**
     * @brief Restore effect state from an XML element
     * 
     * @param xml XML element containing effect state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml) override;
    
protected:
    /**
     * @brief Process a single sample (mono)
     * 
     * @param sample The input sample
     * @return The processed sample
     */
    float processSample(float sample) override;
    
    /**
     * @brief Process a single sample (stereo)
     * 
     * @param leftSample The left channel input sample
     * @param rightSample The right channel input sample
     * @param leftOutput Pointer to store left channel output
     * @param rightOutput Pointer to store right channel output
     */
    void processSampleStereo(float leftSample, float rightSample, float* leftOutput, float* rightOutput) override;
    
    /**
     * @brief Process a stereo buffer
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples) override;
    
private:
    // Delay parameters
    std::array<float, 2> delayTimeMs; // Delay time in milliseconds
    std::array<DelayTimeSync, 2> delayTimeSync; // Delay time sync mode
    std::array<float, 2> feedback; // Feedback amount (0-1)
    std::array<float, 2> crossFeedback; // Cross-feedback amount (0-1)
    float tempo; // Tempo in BPM
    
    // Delay line
    std::array<std::unique_ptr<juce::AudioBuffer<float>>, 2> delayBuffer;
    std::array<int, 2> writePosition;
    std::array<int, 2> delayLength; // Delay length in samples
    
    // Update delay times based on sync mode and tempo
    void updateDelayTimes();
    
    // Convert a sync mode to a delay time in milliseconds
    float syncModeToMs(DelayTimeSync mode, float bpm) const;
    
    // Get a sample from the delay buffer with linear interpolation
    float getSampleFromDelayBuffer(int channel, float delaySamples);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

} // namespace UndergroundBeats
