/*
 * Underground Beats
 * AppComponent.cpp
 * 
 * Implementation of main application component
 */

#include "AppComponent.h"
#include "views/PatternEditorView.h"
#include "views/MixerView.h"

namespace UndergroundBeats {

AppComponent::AppComponent()
    : mainTabs(juce::TabbedButtonBar::TabsAtTop)
{
    // Set up command manager
    commandManager.registerAllCommandsForTarget(this);
    
    // Start the timer for UI updates
    startTimer(50); // 50ms timer interval (20 fps)
}

AppComponent::~AppComponent()
{
    stopTimer();
}

bool AppComponent::initialize()
{
    // Initialize all components
    if (!initializeAudio() ||
        !initializeSynthesis() ||
        !initializeEffects() ||
        !initializeSequencer() ||
        !initializeMidi())
    {
        return false;
    }
    
    // Create the UI components
    createComponents();
    
    return true;
}

void AppComponent::createComponents()
{
    // Add the tabs component to the main window
    addAndMakeVisible(mainTabs);
    
    // Create tabs
    createPatternEditorTab();
    createMixerTab();
    createSynthTab();
    createEffectsTab();
    createSettingsTab();
}

void AppComponent::resized()
{
    // Set the main tabs to fill the entire window
    mainTabs.setBounds(getLocalBounds());
}

void AppComponent::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AppComponent::timerCallback()
{
    // Update UI based on sequencer position, etc.
    if (sequencer != nullptr && sequencer->isPlaying())
    {
        // Trigger a repaint to update any UI that shows playback position
        repaint();
    }
}

void AppComponent::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    commands.add(CommandIDs::PlayStop);
    commands.add(CommandIDs::Save);
    commands.add(CommandIDs::Open);
    commands.add(CommandIDs::New);
    commands.add(CommandIDs::Preferences);
}

void AppComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case CommandIDs::PlayStop:
            result.setInfo("Play/Stop", "Start or stop playback", "Transport", 0);
            result.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
            break;
        
        case CommandIDs::Save:
            result.setInfo("Save", "Save the current project", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
            break;
        
        case CommandIDs::Open:
            result.setInfo("Open", "Open a project", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
            break;
        
        case CommandIDs::New:
            result.setInfo("New", "Create a new project", "File", 0);
            result.addDefaultKeypress('n', juce::ModifierKeys::commandModifier);
            break;
        
        case CommandIDs::Preferences:
            result.setInfo("Preferences", "Open preferences dialog", "Edit", 0);
            result.addDefaultKeypress(',', juce::ModifierKeys::commandModifier);
            break;
            
        default:
            break;
    }
}

bool AppComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::PlayStop:
            if (sequencer != nullptr)
            {
                sequencer->togglePlayStop();
                return true;
            }
            break;
        
        case CommandIDs::Save:
            // TODO: Implement save functionality
            return true;
        
        case CommandIDs::Open:
            // TODO: Implement open functionality
            return true;
        
        case CommandIDs::New:
            // TODO: Implement new project functionality
            return true;
        
        case CommandIDs::Preferences:
            // TODO: Implement preferences dialog
            return true;
            
        default:
            break;
    }
    
    return false;
}

juce::ApplicationCommandTarget* AppComponent::getNextCommandTarget()
{
    return nullptr;
}

bool AppComponent::initializeAudio()
{
    // Create the audio engine
    audioEngine = std::make_unique<AudioEngine>();
    
    // Initialize with default settings
    return audioEngine->initialize();
}

bool AppComponent::initializeSynthesis()
{
    // Create the synth module with 8 voices
    synthModule = std::make_unique<SynthModule>(8);
    
    // Initialize with default settings
    synthModule->prepare(audioEngine->getSampleRate());
    
    return true;
}

bool AppComponent::initializeEffects()
{
    // Create the effects chain
    effectsChain = std::make_unique<EffectsChain>();
    
    // Initialize with default settings
    effectsChain->prepare(audioEngine->getSampleRate(), audioEngine->getBufferSize());
    
    return true;
}

bool AppComponent::initializeSequencer()
{
    // Create the timeline
    timeline = std::make_shared<Timeline>();
    
    // Create the sequencer
    sequencer = std::make_unique<Sequencer>();
    sequencer->setTimeline(timeline);
    
    // Initialize with default settings
    sequencer->prepare(audioEngine->getSampleRate(), audioEngine->getBufferSize());
    
    // Create a default pattern
    auto pattern = std::make_shared<Pattern>("Default Pattern", 4.0);
    int patternId = timeline->addPattern(std::move(pattern));
    timeline->addPatternInstance(patternId, 0.0);
    
    return true;
}

bool AppComponent::initializeMidi()
{
    // Create the MIDI engine
    midiEngine = std::make_unique<MidiEngine>();
    
    // Initialize with default settings
    return midiEngine->initialize();
}

void AppComponent::createPatternEditorTab()
{
    // Create the pattern editor view
    auto patternEditorView = new PatternEditorView();
    
    // Set up the pattern editor with the current pattern and sequencer
    if (timeline != nullptr && sequencer != nullptr)
    {
        // Get the first pattern from the timeline
        if (timeline->getNumPatternInstances() > 0)
        {
            const auto* instance = timeline->getPatternInstance(0);
            if (instance != nullptr)
            {
                auto pattern = timeline->getPattern(instance->patternId);
                if (pattern != nullptr)
                {
                    patternEditorView->setPattern(std::shared_ptr<Pattern>(pattern, [](Pattern*){}));
                }
            }
        }
        
        patternEditorView->setSequencer(sequencer.get());
    }
    
    // Add to tabs
    mainTabs.addTab("Pattern Editor", juce::Colours::darkgrey, patternEditorView, true);
}

void AppComponent::createMixerTab()
{
    // Create the mixer view
    auto mixerView = new MixerView();
    
    // Set up the mixer with the audio engine
    if (audioEngine != nullptr)
    {
        mixerView->setAudioEngine(audioEngine.get());
    }
    
    // Add to tabs
    mainTabs.addTab("Mixer", juce::Colours::darkgrey, mixerView, true);
}

void AppComponent::createSynthTab()
{
    // Create the synth component (placeholder for now)
    auto synth = new juce::Component();
    synth->setName("Synth");
    
    // Add to tabs
    mainTabs.addTab("Synth", juce::Colours::darkgrey, synth, true);
}

void AppComponent::createEffectsTab()
{
    // Create the effects component (placeholder for now)
    auto effects = new juce::Component();
    effects->setName("Effects");
    
    // Add to tabs
    mainTabs.addTab("Effects", juce::Colours::darkgrey, effects, true);
}

void AppComponent::createSettingsTab()
{
    // Create the settings component (placeholder for now)
    auto settings = new juce::Component();
    settings->setName("Settings");
    
    // Add to tabs
    mainTabs.addTab("Settings", juce::Colours::darkgrey, settings, true);
}

} // namespace UndergroundBeats
