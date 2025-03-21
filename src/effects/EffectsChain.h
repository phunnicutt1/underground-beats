/*
 * Underground Beats
 * EffectsChain.h
 * 
 * Manages a chain of audio effects
 */

#pragma once

#include "Effect.h"
#include <vector>
#include <memory>
#include <string>

namespace UndergroundBeats {

/**
 * @class EffectsChain
 * @brief Manages a chain of audio effects
 * 
 * The EffectsChain class manages a chain of audio effects, handling the
 * routing of audio through each effect in sequence and providing methods
 * to add, remove, and reorder effects.
 */
class EffectsChain {
public:
    EffectsChain();
    ~EffectsChain();
    
    /**
     * @brief Add an effect to the chain
     * 
     * @param effect The effect to add
     * @return The index of the added effect
     */
    int addEffect(std::unique_ptr<Effect> effect);
    
    /**
     * @brief Remove an effect from the chain
     * 
     * @param index The index of the effect to remove
     * @return true if the effect was removed
     */
    bool removeEffect(int index);
    
    /**
     * @brief Get an effect from the chain
     * 
     * @param index The index of the effect to get
     * @return Pointer to the effect, or nullptr if not found
     */
    Effect* getEffect(int index);
    
    /**
     * @brief Get an effect from the chain by name
     * 
     * @param name The name of the effect to get
     * @return Pointer to the effect, or nullptr if not found
     */
    Effect* getEffectByName(const std::string& name);
    
    /**
     * @brief Move an effect to a new position in the chain
     * 
     * @param currentIndex The current index of the effect
     * @param newIndex The new index for the effect
     * @return true if the effect was moved
     */
    bool moveEffect(int currentIndex, int newIndex);
    
    /**
     * @brief Get the number of effects in the chain
     * 
     * @return The number of effects
     */
    int getNumEffects() const;
    
    /**
     * @brief Process a mono buffer of samples through the effect chain
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples through the effect chain
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the effect chain for processing
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    void prepare(double sampleRate, int blockSize);
    
    /**
     * @brief Reset all effects in the chain
     */
    void reset();
    
    /**
     * @brief Create an XML element containing the effect chain's state
     * 
     * @return XML element containing effect chain state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore effect chain state from an XML element
     * 
     * @param xml XML element containing effect chain state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
private:
    std::vector<std::unique_ptr<Effect>> effects;
    double currentSampleRate;
    int currentBlockSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsChain)
};

} // namespace UndergroundBeats
