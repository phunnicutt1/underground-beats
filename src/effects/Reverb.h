#pragma once

#include <JuceHeader.h>
#include "../audio-engine/ProcessorNode.h"

/**
 * Reverb effect processor with room size, damping, and mix controls.
 */
class Reverb : public ProcessorNode
{
public:
    /** Parameter indices */
    enum ParameterIndex
    {
        RoomSizeParam,
        DampingParam,
        WidthParam,
        DryWetParam,
        NumParams
    };
    
public:
    Reverb();
    ~Reverb() override;
    
    const juce::String getName() const override { return "Reverb"; }
    
    /** Set the room size (0.0 - 1.0) */
    void setRoomSize(float size);
    
    /** Set the damping amount (0.0 - 1.0) */
    void setDamping(float damping);
    
    /** Set the stereo width (0.0 - 1.0) */
    void setWidth(float width);
    
    /** Set the dry/wet mix (0.0 = dry, 1.0 = wet) */
    void setDryWet(float dryWet);
    
    //==============================================================================
    // AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    //==============================================================================
    // Custom SIMD processing
    void processBlockSIMD(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
private:
    /** Update reverb parameters when settings change */
    void updateParameters();
    
    //==============================================================================
    // Effect parameters
    float roomSize;
    float dampingAmount;
    float stereoWidth;
    float dryWetMix;
    
    // JUCE reverb processor
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;
    
    // Process spec for DSP modules
    juce::dsp::ProcessSpec processSpec;
    
    // Parameter smoothing
    juce::LinearSmoothedValue<float> roomSizeSmoothed;
    juce::LinearSmoothedValue<float> dampingSmoothed;
    juce::LinearSmoothedValue<float> widthSmoothed;
    juce::LinearSmoothedValue<float> dryWetSmoothed;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Reverb)
};
