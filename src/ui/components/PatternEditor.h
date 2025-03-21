/*
 * Underground Beats
 * PatternEditor.h
 * 
 * Pattern editor component for editing MIDI patterns
 */

#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"
#include <memory>

namespace UndergroundBeats {

/**
 * @class PatternEditor
 * @brief Component for editing MIDI patterns
 * 
 * The PatternEditor class provides a grid-based interface for editing MIDI
 * note patterns. It allows adding, editing, and removing notes, as well as
 * editing note properties like velocity and duration.
 */
class PatternEditor : public juce::Component,
                     public juce::MouseListener {
public:
    PatternEditor();
    ~PatternEditor() override;
    
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
     * @brief Set the grid resolution
     * 
     * @param beatsPerGrid Number of beats per grid cell
     */
    void setGridResolution(double beatsPerGrid);
    
    /**
     * @brief Get the current grid resolution
     * 
     * @return Beats per grid cell
     */
    double getGridResolution() const;
    
    /**
     * @brief Set whether to snap to grid
     * 
     * @param shouldSnap Whether to snap to grid
     */
    void setSnapToGrid(bool shouldSnap);
    
    /**
     * @brief Check if snap to grid is enabled
     * 
     * @return true if snap to grid is enabled
     */
    bool isSnapToGrid() const;
    
    /**
     * @brief Set the current edit mode
     * 
     * @param mode The edit mode (0 = select, 1 = draw, 2 = erase)
     */
    void setEditMode(int mode);
    
    /**
     * @brief Get the current edit mode
     * 
     * @return The current edit mode
     */
    int getEditMode() const;
    
    /**
     * @brief Set the playback position (for display)
     * 
     * @param positionInBeats Position in beats
     */
    void setPlaybackPosition(double positionInBeats);
    
    /**
     * @brief Get the current playback position
     * 
     * @return Position in beats
     */
    double getPlaybackPosition() const;
    
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
     * @brief Mouse down callback
     * 
     * @param event Mouse event information
     */
    void mouseDown(const juce::MouseEvent& event) override;
    
    /**
     * @brief Mouse drag callback
     * 
     * @param event Mouse event information
     */
    void mouseDrag(const juce::MouseEvent& event) override;
    
    /**
     * @brief Mouse up callback
     * 
     * @param event Mouse event information
     */
    void mouseUp(const juce::MouseEvent& event) override;
    
    /**
     * @brief Mouse double-click callback
     * 
     * @param event Mouse event information
     */
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    
private:
    std::shared_ptr<Pattern> pattern;
    double gridResolution;
    bool snapToGrid;
    int editMode;
    double playbackPosition;
    
    // Currently selected note (for dragging/editing)
    int selectedNoteIndex;
    
    // Mouse drag start position
    juce::Point<int> dragStartPosition;
    
    // Convert between pixel coordinates and musical values
    int noteToY(int note) const;
    int yToNote(int y) const;
    double xToTime(int x) const;
    int timeToX(double time) const;
    
    // Find a note at the given pixel coordinates
    int findNoteAt(int x, int y) const;
    
    // Grid-related methods
    double snapTimeToGrid(double time) const;
    void drawGrid(juce::Graphics& g);
    
    // Drawing methods
    void drawNotes(juce::Graphics& g);
    void drawPlaybackPosition(juce::Graphics& g);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternEditor)
};

} // namespace UndergroundBeats
