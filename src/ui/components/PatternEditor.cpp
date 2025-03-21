/*
 * Underground Beats
 * PatternEditor.cpp
 * 
 * Implementation of pattern editor component
 */

#include "PatternEditor.h"

namespace UndergroundBeats {

PatternEditor::PatternEditor()
    : gridResolution(0.25)  // 16th note grid by default (assuming 4/4 time)
    , snapToGrid(true)
    , editMode(1)  // Draw mode by default
    , playbackPosition(0.0)
    , selectedNoteIndex(-1)
{
    // Add mouse listener to receive mouse events
    addMouseListener(this, true);
    
    // Make sure the component receives keyboard focus
    setWantsKeyboardFocus(true);
}

PatternEditor::~PatternEditor()
{
}

void PatternEditor::setPattern(std::shared_ptr<Pattern> pattern)
{
    this->pattern = pattern;
    selectedNoteIndex = -1;  // Clear selection
    repaint();
}

std::shared_ptr<Pattern> PatternEditor::getPattern() const
{
    return pattern;
}

void PatternEditor::setGridResolution(double beatsPerGrid)
{
    gridResolution = beatsPerGrid;
    repaint();
}

double PatternEditor::getGridResolution() const
{
    return gridResolution;
}

void PatternEditor::setSnapToGrid(bool shouldSnap)
{
    snapToGrid = shouldSnap;
}

bool PatternEditor::isSnapToGrid() const
{
    return snapToGrid;
}

void PatternEditor::setEditMode(int mode)
{
    editMode = mode;
}

int PatternEditor::getEditMode() const
{
    return editMode;
}

void PatternEditor::setPlaybackPosition(double positionInBeats)
{
    playbackPosition = positionInBeats;
    repaint();
}

double PatternEditor::getPlaybackPosition() const
{
    return playbackPosition;
}

void PatternEditor::resized()
{
    // Layout is handled in paint()
}

void PatternEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::black);
    
    // Draw grid
    drawGrid(g);
    
    // Draw notes
    drawNotes(g);
    
    // Draw playback position
    drawPlaybackPosition(g);
}

void PatternEditor::mouseDown(const juce::MouseEvent& event)
{
    if (pattern == nullptr)
        return;
    
    dragStartPosition = event.getPosition();
    
    if (editMode == 0)  // Select mode
    {
        // Try to select a note
        selectedNoteIndex = findNoteAt(event.x, event.y);
    }
    else if (editMode == 1)  // Draw mode
    {
        // Convert mouse coordinates to note and time
        int note = yToNote(event.y);
        double time = xToTime(event.x);
        
        // Snap time to grid if enabled
        if (snapToGrid)
        {
            time = snapTimeToGrid(time);
        }
        
        // Add a new note
        selectedNoteIndex = pattern->addNote(note, 100, time, gridResolution);
        repaint();
    }
    else if (editMode == 2)  // Erase mode
    {
        // Find and remove a note
        int noteIndex = findNoteAt(event.x, event.y);
        if (noteIndex >= 0)
        {
            pattern->removeNote(noteIndex);
            selectedNoteIndex = -1;
            repaint();
        }
    }
}

void PatternEditor::mouseDrag(const juce::MouseEvent& event)
{
    if (pattern == nullptr || selectedNoteIndex < 0)
        return;
    
    if (editMode == 0)  // Select mode
    {
        // Get the note to edit
        const NoteEvent* note = pattern->getNote(selectedNoteIndex);
        if (note != nullptr)
        {
            // Calculate drag delta in time and note
            double timeDelta = xToTime(event.x) - xToTime(dragStartPosition.x);
            int noteDelta = yToNote(event.y) - yToNote(dragStartPosition.y);
            
            // Update the note
            double newTime = note->startTime + timeDelta;
            if (snapToGrid)
            {
                newTime = snapTimeToGrid(newTime);
            }
            
            int newNote = note->note + noteDelta;
            newNote = juce::jlimit(0, 127, newNote);
            
            pattern->editNote(selectedNoteIndex, newNote, note->velocity, newTime, note->duration);
            
            // Update drag start for relative movement
            dragStartPosition = event.getPosition();
            
            repaint();
        }
    }
    else if (editMode == 1)  // Draw mode
    {
        // Get the note to edit
        const NoteEvent* note = pattern->getNote(selectedNoteIndex);
        if (note != nullptr)
        {
            // Update the note duration based on drag
            double endTime = xToTime(event.x);
            if (snapToGrid)
            {
                endTime = snapTimeToGrid(endTime);
            }
            
            double duration = endTime - note->startTime;
            if (duration <= 0.0)
            {
                duration = gridResolution;  // Minimum duration
            }
            
            pattern->editNote(selectedNoteIndex, note->note, note->velocity, note->startTime, duration);
            
            repaint();
        }
    }
}

void PatternEditor::mouseUp(const juce::MouseEvent& event)
{
    // Nothing specific to do here
}

void PatternEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    if (pattern == nullptr)
        return;
    
    // Double click to edit note properties
    int noteIndex = findNoteAt(event.x, event.y);
    if (noteIndex >= 0)
    {
        // TODO: Show note properties dialog
    }
}

int PatternEditor::noteToY(int note) const
{
    // Map MIDI note (0-127) to vertical pixel position
    // Note: Higher notes at the top, lower notes at the bottom
    // Display 88 notes (piano range) centered in the view
    int visibleNotes = 88;
    int centerNote = 60; // Middle C
    
    // Calculate range
    int minNote = centerNote - visibleNotes / 2;
    int maxNote = minNote + visibleNotes;
    
    // Calculate note height
    float noteHeight = static_cast<float>(getHeight()) / visibleNotes;
    
    // Invert Y coordinate (0 at top, height at bottom)
    return getHeight() - static_cast<int>((note - minNote) * noteHeight);
}

int PatternEditor::yToNote(int y) const
{
    // Map vertical pixel position to MIDI note
    int visibleNotes = 88;
    int centerNote = 60; // Middle C
    
    // Calculate range
    int minNote = centerNote - visibleNotes / 2;
    int maxNote = minNote + visibleNotes;
    
    // Calculate note height
    float noteHeight = static_cast<float>(getHeight()) / visibleNotes;
    
    // Invert Y coordinate
    int invertedY = getHeight() - y;
    
    // Calculate note
    int note = minNote + static_cast<int>(invertedY / noteHeight);
    
    // Clamp to valid MIDI note range
    return juce::jlimit(0, 127, note);
}

double PatternEditor::xToTime(int x) const
{
    if (pattern == nullptr)
        return 0.0;
    
    // Map horizontal pixel position to time in beats
    double patternLength = pattern->getLength();
    return (static_cast<double>(x) / getWidth()) * patternLength;
}

int PatternEditor::timeToX(double time) const
{
    if (pattern == nullptr)
        return 0;
    
    // Map time in beats to horizontal pixel position
    double patternLength = pattern->getLength();
    return static_cast<int>((time / patternLength) * getWidth());
}

int PatternEditor::findNoteAt(int x, int y) const
{
    if (pattern == nullptr)
        return -1;
    
    // Convert pixel position to time and note
    double time = xToTime(x);
    int note = yToNote(y);
    
    // Define a note "hit area" (how close in note number and time)
    const int noteThreshold = 2;
    const double timeThreshold = gridResolution / 2.0;
    
    // Check all notes in the pattern
    for (int i = 0; i < pattern->getNumNotes(); ++i)
    {
        const NoteEvent* noteEvent = pattern->getNote(i);
        if (noteEvent != nullptr)
        {
            // Check if the position is within this note's area
            if (std::abs(noteEvent->note - note) <= noteThreshold &&
                time >= noteEvent->startTime &&
                time <= noteEvent->startTime + noteEvent->duration)
            {
                return i;
            }
        }
    }
    
    return -1;
}

double PatternEditor::snapTimeToGrid(double time) const
{
    return std::round(time / gridResolution) * gridResolution;
}

void PatternEditor::drawGrid(juce::Graphics& g)
{
    if (pattern == nullptr)
        return;
    
    // Set grid line color
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    
    // Draw horizontal lines (notes)
    for (int note = 0; note <= 127; note += 12)
    {
        int y = noteToY(note);
        g.drawLine(0, y, getWidth(), y, 1.0f);
        
        // Draw C notes in a highlight color
        if (note % 12 == 0)
        {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
            g.drawLine(0, y, getWidth(), y, 2.0f);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
    }
    
    // Draw vertical lines (beats)
    double patternLength = pattern->getLength();
    for (double beat = 0; beat <= patternLength; beat += gridResolution)
    {
        int x = timeToX(beat);
        
        // Draw whole beats in a highlight color
        if (std::abs(std::round(beat) - beat) < 0.001)
        {
            g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
            g.drawLine(x, 0, x, getHeight(), 2.0f);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
        else
        {
            g.drawLine(x, 0, x, getHeight(), 1.0f);
        }
    }
}

void PatternEditor::drawNotes(juce::Graphics& g)
{
    if (pattern == nullptr)
        return;
    
    // Get all notes in the pattern
    const auto& notes = pattern->getNotes();
    
    // Draw each note
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto& note = notes[i];
        
        // Calculate position and size
        int x = timeToX(note.startTime);
        int y = noteToY(note.note);
        int width = timeToX(note.startTime + note.duration) - x;
        int height = 10; // Fixed height for now
        
        // Draw the note
        juce::Colour noteColor = juce::Colour(41, 128, 185); // Nice blue color
        
        // Highlight selected note
        if (i == selectedNoteIndex)
        {
            noteColor = juce::Colour(231, 76, 60); // Highlight color
        }
        
        // Draw with velocity-based alpha
        float alpha = juce::jmap(static_cast<float>(note.velocity), 0.0f, 127.0f, 0.5f, 1.0f);
        g.setColour(noteColor.withAlpha(alpha));
        
        // Draw note rectangle
        g.fillRoundedRectangle(x, y - height / 2, width, height, 3.0f);
        
        // Draw outline
        g.setColour(noteColor.brighter().withAlpha(alpha));
        g.drawRoundedRectangle(x, y - height / 2, width, height, 3.0f, 1.0f);
    }
}

void PatternEditor::drawPlaybackPosition(juce::Graphics& g)
{
    if (pattern == nullptr)
        return;
    
    // Draw playback position line
    int x = timeToX(playbackPosition);
    
    g.setColour(juce::Colours::white);
    g.drawLine(x, 0, x, getHeight(), 2.0f);
}

} // namespace UndergroundBeats
