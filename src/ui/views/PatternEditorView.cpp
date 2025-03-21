/*
 * Underground Beats
 * PatternEditorView.cpp
 * 
 * Implementation of complete pattern editor view
 */

#include "PatternEditorView.h"

namespace UndergroundBeats {

PatternEditorView::PatternEditorView()
    : sequencer(nullptr)
{
    // Add the pattern editor and control panel
    addAndMakeVisible(patternEditor);
    addAndMakeVisible(controlPanel);
    
    // Set up callback for control panel settings changes
    controlPanel.setSettingsChangeCallback([this]() { onSettingsChanged(); });
    
    // Start the timer for playback position updates
    startTimer(50); // 50ms timer interval (20 fps)
}

PatternEditorView::~PatternEditorView()
{
    stopTimer();
}

void PatternEditorView::setPattern(std::shared_ptr<Pattern> pattern)
{
    this->pattern = pattern;
    
    // Update the pattern in the editor and control panel
    patternEditor.setPattern(pattern);
    controlPanel.setPattern(pattern);
}

std::shared_ptr<Pattern> PatternEditorView::getPattern() const
{
    return pattern;
}

void PatternEditorView::setSequencer(Sequencer* sequencer)
{
    this->sequencer = sequencer;
    
    // Update the sequencer in the control panel
    controlPanel.setSequencer(sequencer);
}

void PatternEditorView::resized()
{
    // Layout components
    int width = getWidth();
    int height = getHeight();
    int controlPanelHeight = 100;
    
    // Control panel at the top
    controlPanel.setBounds(0, 0, width, controlPanelHeight);
    
    // Pattern editor fills the rest
    patternEditor.setBounds(0, controlPanelHeight, width, height - controlPanelHeight);
}

void PatternEditorView::timerCallback()
{
    // Update the playback position in the pattern editor
    if (sequencer != nullptr && pattern != nullptr)
    {
        double position = sequencer->getPosition();
        patternEditor.setPlaybackPosition(position);
    }
}

void PatternEditorView::onSettingsChanged()
{
    // Update pattern editor settings based on control panel
    patternEditor.setGridResolution(controlPanel.getGridResolution());
    patternEditor.setSnapToGrid(controlPanel.isSnapToGrid());
    patternEditor.setEditMode(controlPanel.getEditMode());
    
    // Trigger a repaint
    patternEditor.repaint();
}

} // namespace UndergroundBeats
