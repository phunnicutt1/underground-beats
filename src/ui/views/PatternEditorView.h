/*
 * Underground Beats
 * PatternEditorView.h
 * 
 * Complete pattern editor view with editor and controls
 */

#pragma once

#include <JuceHeader.h>
#include "../components/PatternEditor.h"
#include "../components/PatternControlPanel.h"
#include "../../sequencer/Pattern.h"
#include "../../sequencer/Sequencer.h"
#include <memory>

namespace UndergroundBeats {

/**
 * @class PatternEditorView
 * @brief Complete pattern editor view with editor and controls
 * 
 * The PatternEditorView class combines a PatternEditor and PatternControlPanel
 * to provide a complete pattern editing interface.
 */
class PatternEditorView : public juce::Component,
                         public juce::Timer {
public:
    PatternEditorView();
    ~PatternEditorView() override;
    
    /**
     * @brief Set the pattern to edit
     * 
     * @param pattern The pattern to edit
     */
    void setPattern(std::shared_ptr<Pattern> pattern);
    
    /**
     * @brief Get the current pattern
     * 
     * @return The current pattern
     */
    std::shared_ptr<Pattern> getPattern() const;
    
    /**
     * @brief Set the sequencer for transport controls
     * 
     * @param sequencer The sequencer
     */
    void setSequencer(Sequencer* sequencer);
    
    /**
     * @brief Component resized callback
     */
    void resized() override;
    
    /**
     * @brief Timer callback for playback position updates
     */
    void timerCallback() override;
    
private:
    PatternEditor patternEditor;
    PatternControlPanel controlPanel;
    
    std::shared_ptr<Pattern> pattern;
    Sequencer* sequencer;
    
    // Handler for control panel settings changes
    void onSettingsChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternEditorView)
};

} // namespace UndergroundBeats
