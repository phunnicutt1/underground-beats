/*
 * Underground Beats
 * AppComponent.h
 * 
 * Main application component that serves as the entry point for the UI
 */

#pragma once

#include <JuceHeader.h>
#include "../audio-engine/AudioEngine.h"
#include "../synthesis/SynthModule.h"
#include "../effects/EffectsChain.h"
#include "../sequencer/Sequencer.h"
#include "../sequencer/MidiEngine.h"
#include <memory>

namespace UndergroundBeats {

/**
 * @class AppComponent
 * @brief Main application component that serves as the entry point for the UI
 * 
 * The AppComponent class is the main component that contains all UI elements
 * and manages the application state. It integrates all components of the
 * application (audio, synthesis, effects, sequencer) and provides the main
 * interface for user interaction.
 */
class AppComponent : public juce::Component,
                    public juce::Timer,
                    public juce::ApplicationCommandTarget {
public:
    AppComponent();
    ~AppComponent() override;
    
    /**
     * @brief Initialize the application
     * 
     * @return true if initialization was successful
     */
    bool initialize();
    
    /**
     * @brief Create the UI components and layout
     */
    void createComponents();
    
    /**
     * @brief Component resized callback
     */
    void resized() override;
    
    /**
     * @brief Component paint callback
     * 
     * @param g Graphics context to paint to
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Timer callback for UI updates
     */
    void timerCallback() override;
    
    /**
     * @brief Get application commands that this target can perform
     * 
     * @param commands The command manager to register with
     */
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    
    /**
     * @brief Get command information for a specific command
     * 
     * @param commandID The command ID to get info for
     * @param result The result object to fill with info
     */
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    
    /**
     * @brief Perform a command
     * 
     * @param info Information about the command to perform
     * @return true if the command was handled
     */
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;
    
    /**
     * @brief Get the next command target in the chain
     * 
     * @return The next command target
     */
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    
private:
    // Core components
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<SynthModule> synthModule;
    std::unique_ptr<EffectsChain> effectsChain;
    std::shared_ptr<Timeline> timeline;
    std::unique_ptr<Sequencer> sequencer;
    std::unique_ptr<MidiEngine> midiEngine;
    
    // UI Components
    juce::TabbedComponent mainTabs;
    
    // Command manager
    juce::ApplicationCommandManager commandManager;
    
    // Command IDs
    enum CommandIDs {
        PlayStop = 1,
        Save,
        Open,
        New,
        Preferences
    };
    
    // Initialize the various components
    bool initializeAudio();
    bool initializeSynthesis();
    bool initializeEffects();
    bool initializeSequencer();
    bool initializeMidi();
    
    // Create the various UI tabs
    void createPatternEditorTab();
    void createMixerTab();
    void createSynthTab();
    void createEffectsTab();
    void createSettingsTab();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppComponent)
};

} // namespace UndergroundBeats
