#pragma once

#include <JuceHeader.h>
#include "audio-engine/Engine.h"
#include "synthesis/Oscillator.h"
#include "synthesis/Envelope.h"
#include "synthesis/Filter.h"
#include "effects/Delay.h"
#include "effects/Reverb.h"

//==============================================================================
class MainComponent  : public juce::AudioAppComponent,
                       private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback for handling envelope trigger
    void timerCallback() override;

private:
    //==============================================================================
    Engine audioEngine;
    std::unique_ptr<Oscillator> oscillator;
    std::unique_ptr<Envelope> envelope;
    std::unique_ptr<Filter> filter;
    std::unique_ptr<Delay> delay;
    std::unique_ptr<Reverb> reverb;
    
    // UI components
    juce::TextButton startButton { "Start Engine" };
    
    // Oscillator controls
    juce::Slider frequencySlider;
    juce::Label frequencyLabel { {}, "Frequency (Hz)" };
    juce::ComboBox waveformSelector;
    juce::Label waveformLabel { {}, "Waveform" };
    juce::Slider pulseWidthSlider;
    juce::Label pulseWidthLabel { {}, "Pulse Width" };
    juce::Slider detuneSlider;
    juce::Label detuneLabel { {}, "Detune (cents)" };
    juce::Slider gainSlider;
    juce::Label gainLabel { {}, "Gain" };
    
    // Envelope controls
    juce::Slider attackSlider;
    juce::Label attackLabel { {}, "Attack (ms)" };
    juce::Slider decaySlider;
    juce::Label decayLabel { {}, "Decay (ms)" };
    juce::Slider sustainSlider;
    juce::Label sustainLabel { {}, "Sustain" };
    juce::Slider releaseSlider;
    juce::Label releaseLabel { {}, "Release (ms)" };
    juce::TextButton triggerButton { "Trigger" };
    
    // Filter controls
    juce::Slider filterFreqSlider;
    juce::Label filterFreqLabel { {}, "Filter Freq (Hz)" };
    juce::Slider resonanceSlider;
    juce::Label resonanceLabel { {}, "Resonance" };
    juce::ComboBox filterTypeSelector;
    juce::Label filterTypeLabel { {}, "Filter Type" };
    
    // Effect selector
    juce::ComboBox effectSelector;
    juce::Label effectLabel { {}, "Effect" };
    
    // Delay controls
    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel { {}, "Delay Time (ms)" };
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel { {}, "Feedback" };
    juce::Slider delayMixSlider;
    juce::Label delayMixLabel { {}, "Mix" };
    
    // Reverb controls
    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel { {}, "Room Size" };
    juce::Slider dampingSlider;
    juce::Label dampingLabel { {}, "Damping" };
    juce::Slider widthSlider;
    juce::Label widthLabel { {}, "Width" };
    juce::Slider reverbMixSlider;
    juce::Label reverbMixLabel { {}, "Mix" };
    
    // Tab component for different sections
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    
    // Container components for each tab
    std::unique_ptr<juce::Component> oscillatorTab;
    std::unique_ptr<juce::Component> envelopeTab;
    std::unique_ptr<juce::Component> filterTab;
    std::unique_ptr<juce::Component> effectsTab;
    
    // Node IDs for the processor graph
    NodeID oscillatorNodeId;
    NodeID envelopeNodeId;
    NodeID filterNodeId;
    NodeID delayNodeId;
    NodeID reverbNodeId;
    
    // Create processors
    void createOscillator();
    void createEnvelope();
    void createFilter();
    void createDelay();
    void createReverb();
    
    // Connect processors in the graph
    void connectProcessors();
    
    // Update UI based on current effect
    void updateEffectsUI();
    
    // Active effect type
    enum EffectType
    {
        NoEffect,
        DelayEffect,
        ReverbEffect
    };
    
    EffectType currentEffect = NoEffect;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
