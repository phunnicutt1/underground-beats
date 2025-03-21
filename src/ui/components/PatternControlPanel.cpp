/*
 * Underground Beats
 * PatternControlPanel.cpp
 * 
 * Implementation of control panel for pattern editing
 */

#include "PatternControlPanel.h"

namespace UndergroundBeats {

PatternControlPanel::PatternControlPanel()
    : sequencer(nullptr)
    , gridResolution(0.25)  // 16th note grid by default
    , snapToGrid(true)
    , editMode(1)  // Draw mode by default
{
    createControls();
}

PatternControlPanel::~PatternControlPanel()
{
}

void PatternControlPanel::setPattern(std::shared_ptr<Pattern> pattern)
{
    this->pattern = pattern;
    updateControls();
}

void PatternControlPanel::setSequencer(Sequencer* sequencer)
{
    this->sequencer = sequencer;
    updateControls();
}

void PatternControlPanel::setSettingsChangeCallback(std::function<void()> callback)
{
    settingsChangeCallback = callback;
}

double PatternControlPanel::getGridResolution() const
{
    return gridResolution;
}

bool PatternControlPanel::isSnapToGrid() const
{
    return snapToGrid;
}

int PatternControlPanel::getEditMode() const
{
    return editMode;
}

void PatternControlPanel::resized()
{
    // Layout the controls
    int width = getWidth();
    int height = getHeight();
    int margin = 5;
    int buttonWidth = 80;
    int buttonHeight = 30;
    int comboWidth = 120;
    int sliderWidth = 180;
    
    // Grid controls
    gridResolutionCombo.setBounds(margin, margin, comboWidth, buttonHeight);
    snapToGridToggle.setBounds(margin + comboWidth + margin, margin, buttonWidth, buttonHeight);
    
    // Edit mode buttons
    selectModeButton.setBounds(margin, margin + buttonHeight + margin, buttonWidth, buttonHeight);
    drawModeButton.setBounds(margin + buttonWidth + margin, margin + buttonHeight + margin, buttonWidth, buttonHeight);
    eraseModeButton.setBounds(margin + buttonWidth * 2 + margin * 2, margin + buttonHeight + margin, buttonWidth, buttonHeight);
    
    // Pattern length slider
    patternLengthSlider.setBounds(width - sliderWidth - margin, margin, sliderWidth, buttonHeight);
    
    // Transport controls
    int transportY = height - buttonHeight - margin;
    playButton.setBounds(margin, transportY, buttonWidth, buttonHeight);
    stopButton.setBounds(margin + buttonWidth + margin, transportY, buttonWidth, buttonHeight);
    recButton.setBounds(margin + buttonWidth * 2 + margin * 2, transportY, buttonWidth, buttonHeight);
}

void PatternControlPanel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey);
}

void PatternControlPanel::buttonClicked(juce::Button* button)
{
    if (button == &snapToGridToggle)
    {
        snapToGrid = snapToGridToggle.getToggleState();
        
        if (settingsChangeCallback)
        {
            settingsChangeCallback();
        }
    }
    else if (button == &selectModeButton)
    {
        editMode = 0;
        updateControls();
        
        if (settingsChangeCallback)
        {
            settingsChangeCallback();
        }
    }
    else if (button == &drawModeButton)
    {
        editMode = 1;
        updateControls();
        
        if (settingsChangeCallback)
        {
            settingsChangeCallback();
        }
    }
    else if (button == &eraseModeButton)
    {
        editMode = 2;
        updateControls();
        
        if (settingsChangeCallback)
        {
            settingsChangeCallback();
        }
    }
    else if (button == &playButton)
    {
        if (sequencer != nullptr)
        {
            sequencer->play();
        }
    }
    else if (button == &stopButton)
    {
        if (sequencer != nullptr)
        {
            sequencer->stop();
        }
    }
    else if (button == &recButton)
    {
        // TODO: Implement recording mode
    }
}

void PatternControlPanel::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &gridResolutionCombo)
    {
        int selectedId = gridResolutionCombo.getSelectedId();
        
        switch (selectedId)
        {
            case 1: gridResolution = 1.0; break;      // Whole note
            case 2: gridResolution = 0.5; break;      // Half note
            case 3: gridResolution = 0.25; break;     // Quarter note
            case 4: gridResolution = 0.125; break;    // Eighth note
            case 5: gridResolution = 0.0625; break;   // Sixteenth note
            case 6: gridResolution = 0.03125; break;  // Thirty-second note
            default: gridResolution = 0.25; break;    // Default to quarter note
        }
        
        if (settingsChangeCallback)
        {
            settingsChangeCallback();
        }
    }
}

void PatternControlPanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &patternLengthSlider)
    {
        double length = patternLengthSlider.getValue();
        
        if (pattern != nullptr)
        {
            pattern->setLength(length);
            
            if (settingsChangeCallback)
            {
                settingsChangeCallback();
            }
        }
    }
}

void PatternControlPanel::createControls()
{
    // Grid resolution combo box
    gridResolutionCombo.addItem("Whole", 1);
    gridResolutionCombo.addItem("Half", 2);
    gridResolutionCombo.addItem("Quarter", 3);
    gridResolutionCombo.addItem("Eighth", 4);
    gridResolutionCombo.addItem("Sixteenth", 5);
    gridResolutionCombo.addItem("Thirty-second", 6);
    gridResolutionCombo.setSelectedId(5);  // Default to 16th note
    gridResolutionCombo.addListener(this);
    addAndMakeVisible(gridResolutionCombo);
    
    // Snap to grid toggle
    snapToGridToggle.setButtonText("Snap to Grid");
    snapToGridToggle.setToggleState(true, juce::dontSendNotification);
    snapToGridToggle.addListener(this);
    addAndMakeVisible(snapToGridToggle);
    
    // Edit mode buttons
    selectModeButton.setButtonText("Select");
    selectModeButton.setClickingTogglesState(true);
    selectModeButton.setRadioGroupId(1);
    selectModeButton.addListener(this);
    addAndMakeVisible(selectModeButton);
    
    drawModeButton.setButtonText("Draw");
    drawModeButton.setClickingTogglesState(true);
    drawModeButton.setRadioGroupId(1);
    drawModeButton.setToggleState(true, juce::dontSendNotification);
    drawModeButton.addListener(this);
    addAndMakeVisible(drawModeButton);
    
    eraseModeButton.setButtonText("Erase");
    eraseModeButton.setClickingTogglesState(true);
    eraseModeButton.setRadioGroupId(1);
    eraseModeButton.addListener(this);
    addAndMakeVisible(eraseModeButton);
    
    // Pattern length slider
    patternLengthSlider.setRange(1.0, 16.0, 1.0);
    patternLengthSlider.setValue(4.0);
    patternLengthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    patternLengthSlider.setTextValueSuffix(" bars");
    patternLengthSlider.addListener(this);
    addAndMakeVisible(patternLengthSlider);
    
    // Transport controls
    playButton.setButtonText("Play");
    playButton.addListener(this);
    addAndMakeVisible(playButton);
    
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    addAndMakeVisible(stopButton);
    
    recButton.setButtonText("Record");
    recButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    recButton.addListener(this);
    addAndMakeVisible(recButton);
}

void PatternControlPanel::updateControls()
{
    // Update pattern length slider
    if (pattern != nullptr)
    {
        patternLengthSlider.setValue(pattern->getLength(), juce::dontSendNotification);
    }
    
    // Update edit mode buttons
    selectModeButton.setToggleState(editMode == 0, juce::dontSendNotification);
    drawModeButton.setToggleState(editMode == 1, juce::dontSendNotification);
    eraseModeButton.setToggleState(editMode == 2, juce::dontSendNotification);
    
    // Update transport button states
    if (sequencer != nullptr)
    {
        playButton.setEnabled(true);
        stopButton.setEnabled(true);
        recButton.setEnabled(true);
    }
    else
    {
        playButton.setEnabled(false);
        stopButton.setEnabled(false);
        recButton.setEnabled(false);
    }
}

} // namespace UndergroundBeats
