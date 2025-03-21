/*
 * Underground Beats
 * Reverb.h
 * 
 * Reverb effect for creating spatial depth
 */

#pragma once

#include "Effect.h"

namespace UndergroundBeats {

/**
 * @class Reverb
 * @brief Reverb effect for creating spatial depth
 * 
 * The Reverb class implements a reverb effect with controls for
 * room size, damping, width, and freeze mode.
 */
class Reverb : public Effect {
public:
    Reverb();
    ~Reverb() override;
    
    /**
     * @brief Set the room size
     * 
     * @param size Room size (0 to 1)
     */
    void setRoomSize(float size);
    
    /**
     * @brief Get the current room size
     * 
     * @return The current room size
     */
    float getRoomSize() const;
    
    /**
     * @brief Set the damping amount
     * 
     * @param amount Damping amount (0 to 1)
     */
    void setDamping(float amount);
    
    /**
     * @brief Get the current damping amount
     * 
     * @return The current damping amount
     */
    float getDamping() const;
    
    /**
     * @brief Set the stereo width
     * 
     * @param width Stereo width (0 to 1)
     */
    void setWidth(float width);
    
    /**
     * @brief Get the current stereo width
     * 
     * @return The current stereo width
     */
    float getWidth() const;
    
    /**
     * @brief Set freeze mode
     * 
     * @param freeze true for freeze mode, false for normal mode
     */
    void setFreeze(bool freeze);
    
    /**
     * @brief Check if freeze mode is enabled
     * 
     * @return true if freeze mode is enabled
     */
    bool getFreeze() const;
    
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
     * @brief Process a buffer of samples
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void processBuffer(float* buffer, int numSamples) override;
    
    /**
     * @brief Process a stereo buffer
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples) override;
    
private:
    // Reverb parameters
    float roomSize;
    float damping;
    float width;
    bool freeze;
    
    // JUCE reverb implementation
    juce::Reverb jucereverb;
    
    // Update reverb parameters
    void updateParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};

} // namespace UndergroundBeats
