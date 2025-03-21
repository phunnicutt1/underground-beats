/*
 * Underground Beats
 * PatternControlPanel.h
 * 
 * Control panel for pattern editing
 */

#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"
#include "../../sequencer/Sequencer.h"
#include <memory>

namespace UndergroundBeats {

/**
 * @class PatternControlPanel
 * @brief Control panel for pattern editing
 * 
 * The PatternControlPanel class provides controls for pattern editing,
 * including grid settings, edit mode, pattern length, and playback controls.
 */
class PatternControlPanel : public juce::Component,
                           public juce::Button::Listener,
                           public juce::ComboBox::Listener,
                           public juce::Slider::Listener {
public:
    PatternControlPanel();
    ~PatternControlPanel() override;
    
    /**
     * @brief Set the pattern to edit
     * 
     * @param pattern The pattern to edit
     */
    void setPattern(std::shared_ptr<Pattern> pattern);
    
    /**
     * @brief Set the sequencer for transport controls
     * 
     * @param sequencer The sequencer
     */
    void setSequencer(Sequencer* sequencer);
    
    /**
     * @brief Set a callback for when settings change
     * 
     * @param callback The callback function
     */
    void setSettingsChangeCallback(std::function<void()> callback);
    
    /**
     * @brief Get the current grid resolution
     * 
     * @return Grid resolution in beats
     */
    double getGridResolution() const;
    
    /**
     * @brief Get whether snap to grid is enabled
     * 
     * @return true if snap to grid is enabled
     */
    bool isSnapToGrid() const;
    
    /**
     * @brief Get the current edit mode
     * 
     * @return Edit mode (0 = select, 1 = draw, 2 = erase)
     */
    int getEditMode() const;
    
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
     * @brief Button clicked callback
     * 
     * @param button The button that was clicked
     */
    void buttonClicked(juce::Button* button) override;
    
    /**
     * @brief ComboBox changed callback
     * 
     * @param comboBox The combo box that changed
     */
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    /**
     * @brief Slider value changed callback
     * 
     * @param slider The slider that changed
     */
    void sliderValueChanged(juce::Slider* slider) override;
    
private:
    std::shared_ptr<Pattern> pattern;
    Sequencer* sequencer;
    std::function<void()> settingsChangeCallback;
    
    // Controls
    juce::ComboBox gridResolutionCombo;
    juce::ToggleButton snapToGridToggle;
    juce::TextButton selectModeButton;
    juce::TextButton drawModeButton;
    juce::TextButton eraseModeButton;
    juce::Slider patternLengthSlider;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton recButton;
    
    // Current settings
    double gridResolution;
    bool snapToGrid;
    int editMode;
    
    // Create and set up the controls
    void createControls();
    
    // Update control states based on current settings
    void updateControls();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternControlPanel)
};

} // namespace UndergroundBeats
