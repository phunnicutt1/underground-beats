/*
 * Underground Beats
 * EffectsChain.cpp
 * 
 * Implementation of effects chain manager
 */

#include "EffectsChain.h"
#include "Delay.h"
#include "Reverb.h"

namespace UndergroundBeats {

EffectsChain::EffectsChain()
    : currentSampleRate(44100.0)
    , currentBlockSize(512)
{
}

EffectsChain::~EffectsChain()
{
}

int EffectsChain::addEffect(std::unique_ptr<Effect> effect)
{
    if (effect == nullptr)
    {
        return -1;
    }
    
    // Prepare the effect with the current sample rate and block size
    effect->prepare(currentSampleRate, currentBlockSize);
    
    // Add the effect to the chain
    effects.push_back(std::move(effect));
    
    // Return the index of the added effect
    return static_cast<int>(effects.size() - 1);
}

bool EffectsChain::removeEffect(int index)
{
    if (index < 0 || index >= static_cast<int>(effects.size()))
    {
        return false;
    }
    
    // Remove the effect from the chain
    effects.erase(effects.begin() + index);
    
    return true;
}

Effect* EffectsChain::getEffect(int index)
{
    if (index < 0 || index >= static_cast<int>(effects.size()))
    {
        return nullptr;
    }
    
    return effects[index].get();
}

Effect* EffectsChain::getEffectByName(const std::string& name)
{
    // Search for an effect with the specified name
    for (auto& effect : effects)
    {
        if (effect->getName() == name)
        {
            return effect.get();
        }
    }
    
    return nullptr;
}

bool EffectsChain::moveEffect(int currentIndex, int newIndex)
{
    if (currentIndex < 0 || currentIndex >= static_cast<int>(effects.size()) ||
        newIndex < 0 || newIndex >= static_cast<int>(effects.size()) ||
        currentIndex == newIndex)
    {
        return false;
    }
    
    // Move the effect to the new position
    auto effect = std::move(effects[currentIndex]);
    effects.erase(effects.begin() + currentIndex);
    effects.insert(effects.begin() + newIndex, std::move(effect));
    
    return true;
}

int EffectsChain::getNumEffects() const
{
    return static_cast<int>(effects.size());
}

void EffectsChain::process(float* buffer, int numSamples)
{
    // Process the buffer through each effect in the chain
    for (auto& effect : effects)
    {
        if (effect->isEnabled())
        {
            effect->process(buffer, numSamples);
        }
    }
}

void EffectsChain::processStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Process the buffer through each effect in the chain
    for (auto& effect : effects)
    {
        if (effect->isEnabled())
        {
            effect->processStereo(leftBuffer, rightBuffer, numSamples);
        }
    }
}

void EffectsChain::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
    
    // Prepare all effects in the chain
    for (auto& effect : effects)
    {
        effect->prepare(sampleRate, blockSize);
    }
}

void EffectsChain::reset()
{
    // Reset all effects in the chain
    for (auto& effect : effects)
    {
        effect->reset();
    }
}

std::unique_ptr<juce::XmlElement> EffectsChain::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("EffectsChain");
    
    // Add each effect's state
    for (size_t i = 0; i < effects.size(); ++i)
    {
        auto effectXml = effects[i]->createStateXml();
        effectXml->setAttribute("index", static_cast<int>(i));
        xml->addChildElement(effectXml.release());
    }
    
    return xml;
}

bool EffectsChain::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "EffectsChain")
    {
        return false;
    }
    
    // Clear the current effects chain
    effects.clear();
    
    // Get the number of effects to restore
    int numEffects = xml->getNumChildElements();
    
    // Resize the effects vector
    effects.reserve(numEffects);
    
    // Temporary vector to hold effects before sorting
    std::vector<std::pair<int, std::unique_ptr<Effect>>> tempEffects;
    tempEffects.reserve(numEffects);
    
    // Restore each effect
    for (int i = 0; i < numEffects; ++i)
    {
        auto effectXml = xml->getChildElement(i);
        
        if (effectXml == nullptr)
        {
            continue;
        }
        
        // Get the effect index
        int index = effectXml->getIntAttribute("index", -1);
        
        if (index < 0)
        {
            continue;
        }
        
        // Create a new effect based on the type
        std::unique_ptr<Effect> effect;
        
        if (effectXml->getTagName() == "Effect")
        {
            // Get the effect name
            juce::String name = effectXml->getStringAttribute("name", "");
            
            if (name == "Delay")
            {
                effect = std::make_unique<Delay>();
            }
            else if (name == "Reverb")
            {
                effect = std::make_unique<Reverb>();
            }
            // Add other effect types here
        }
        
        if (effect != nullptr)
        {
            // Restore the effect state
            effect->restoreStateFromXml(effectXml);
            
            // Prepare the effect
            effect->prepare(currentSampleRate, currentBlockSize);
            
            // Add to temporary vector with index
            tempEffects.emplace_back(index, std::move(effect));
        }
    }
    
    // Sort effects by index
    std::sort(tempEffects.begin(), tempEffects.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    // Add sorted effects to the chain
    for (auto& [index, effect] : tempEffects)
    {
        effects.push_back(std::move(effect));
    }
    
    return true;
}

} // namespace UndergroundBeats
