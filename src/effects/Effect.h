/*
 * Underground Beats
 * Effect.h
 * 
 * Base class for all audio effects
 */

#pragma once

#include <JuceHeader.h>
#include <string>
#include <memory>

namespace UndergroundBeats {

/**
 * @class Effect
 * @brief Base class for all audio effects
 * 
 * The Effect class provides a common interface for all audio effects
 * in the Underground Beats application. It defines methods for processing
 * audio, parameter control, and state management.
 */
class Effect {
public:
    Effect(const std::string& name);
    virtual ~Effect();
    
    /**
     * @brief Get the effect name
     * 
     * @return The effect name
     */
    std::string getName() const;
    
    /**
     * @brief Set whether the effect is enabled
     * 
     * @param enabled true to enable, false to bypass
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if the effect is enabled
     * 
     * @return true if enabled, false if bypassed
     */
    bool isEnabled() const;
    
    /**
     * @brief Set the effect's wet/dry mix
     * 
     * @param mix The wet/dry mix (0 = dry, 1 = wet)
     */
    void setMix(float mix);
    
    /**
     * @brief Get the current wet/dry mix
     * 
     * @return The current wet/dry mix
     */
    float getMix() const;
    
    /**
     * @brief Process a mono buffer of samples
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the effect for processing
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    virtual void prepare(double sampleRate, int blockSize);
    
    /**
     * @brief Reset the effect state
     */
    virtual void reset();
    
    /**
     * @brief Create an XML element containing the effect's state
     * 
     * @return XML element containing effect state
     */
    virtual std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore effect state from an XML element
     * 
     * @param xml XML element containing effect state
     * @return true if state was successfully restored
     */
    virtual bool restoreStateFromXml(const juce::XmlElement* xml);
    
protected:
    /**
     * @brief Process a single sample (mono)
     * 
     * Derived classes must implement this method to process
     * a single sample.
     * 
     * @param sample The input sample
     * @return The processed sample
     */
    virtual float processSample(float sample) = 0;
    
    /**
     * @brief Process a single sample (stereo)
     * 
     * Derived classes must implement this method to process
     * a single stereo sample.
     * 
     * @param leftSample The left channel input sample
     * @param rightSample The right channel input sample
     * @param leftOutput Pointer to store left channel output
     * @param rightOutput Pointer to store right channel output
     */
    virtual void processSampleStereo(float leftSample, float rightSample, float* leftOutput, float* rightOutput) = 0;
    
    /**
     * @brief Process a buffer of samples with custom implementation
     * 
     * Derived classes can optionally override this method to
     * implement more efficient buffer processing.
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    virtual void processBuffer(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer with custom implementation
     * 
     * Derived classes can optionally override this method to
     * implement more efficient stereo buffer processing.
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    virtual void processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    std::string effectName;
    bool enabled;
    float mixLevel;
    double currentSampleRate;
    int currentBlockSize;
    
    // Temporary buffer for wet/dry mixing
    juce::AudioBuffer<float> tempBuffer;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)
};

} // namespace UndergroundBeats
