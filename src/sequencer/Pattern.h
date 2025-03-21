/*
 * Underground Beats
 * Pattern.h
 * 
 * Pattern data structure for storing MIDI notes and automation
 */

#pragma once

#include <JuceHeader.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace UndergroundBeats {

/**
 * @brief Data structure to represent a MIDI note event
 */
struct NoteEvent {
    int note;          // MIDI note number (0-127)
    int velocity;      // Note velocity (0-127)
    double startTime;  // Start time in beats
    double duration;   // Duration in beats
    
    NoteEvent(int note = 60, int velocity = 100, double startTime = 0.0, double duration = 1.0)
        : note(note), velocity(velocity), startTime(startTime), duration(duration)
    {
    }
};

/**
 * @brief Data structure to represent a parameter automation point
 */
struct AutomationPoint {
    double time;       // Time in beats
    float value;       // Parameter value (0-1)
    int curveType;     // Curve type for interpolation
    
    AutomationPoint(double time = 0.0, float value = 0.0, int curveType = 0)
        : time(time), value(value), curveType(curveType)
    {
    }
};

/**
 * @class Pattern
 * @brief Pattern data structure for storing MIDI notes and automation
 * 
 * The Pattern class represents a musical pattern containing MIDI notes
 * and parameter automation data. It provides methods for adding, editing,
 * and removing notes and automation points, as well as for serialization.
 */
class Pattern {
public:
    Pattern(const std::string& name = "Untitled Pattern", double lengthInBeats = 4.0);
    ~Pattern();
    
    /**
     * @brief Set the pattern name
     * 
     * @param name The pattern name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Get the pattern name
     * 
     * @return The pattern name
     */
    std::string getName() const;
    
    /**
     * @brief Set the pattern length in beats
     * 
     * @param lengthInBeats The length in beats
     */
    void setLength(double lengthInBeats);
    
    /**
     * @brief Get the pattern length in beats
     * 
     * @return The pattern length
     */
    double getLength() const;
    
    /**
     * @brief Add a note to the pattern
     * 
     * @param note The MIDI note number
     * @param velocity The note velocity
     * @param startTime The start time in beats
     * @param duration The duration in beats
     * @return Index of the added note
     */
    int addNote(int note, int velocity, double startTime, double duration);
    
    /**
     * @brief Add a note event to the pattern
     * 
     * @param noteEvent The note event to add
     * @return Index of the added note
     */
    int addNote(const NoteEvent& noteEvent);
    
    /**
     * @brief Edit a note in the pattern
     * 
     * @param index The note index
     * @param note The new MIDI note number
     * @param velocity The new velocity
     * @param startTime The new start time
     * @param duration The new duration
     * @return true if the note was edited
     */
    bool editNote(int index, int note, int velocity, double startTime, double duration);
    
    /**
     * @brief Remove a note from the pattern
     * 
     * @param index The index of the note to remove
     * @return true if the note was removed
     */
    bool removeNote(int index);
    
    /**
     * @brief Get a note event by index
     * 
     * @param index The note index
     * @return The note event, or nullptr if not found
     */
    const NoteEvent* getNote(int index) const;
    
    /**
     * @brief Get the number of notes in the pattern
     * 
     * @return The number of notes
     */
    int getNumNotes() const;
    
    /**
     * @brief Get all notes in the pattern
     * 
     * @return Vector of note events
     */
    const std::vector<NoteEvent>& getNotes() const;
    
    /**
     * @brief Add an automation point for a parameter
     * 
     * @param paramId The parameter ID
     * @param time The time in beats
     * @param value The parameter value
     * @param curveType The curve type for interpolation
     * @return Index of the added automation point
     */
    int addAutomationPoint(const std::string& paramId, double time, float value, int curveType = 0);
    
    /**
     * @brief Edit an automation point
     * 
     * @param paramId The parameter ID
     * @param index The point index
     * @param time The new time
     * @param value The new value
     * @param curveType The new curve type
     * @return true if the point was edited
     */
    bool editAutomationPoint(const std::string& paramId, int index, double time, float value, int curveType = 0);
    
    /**
     * @brief Remove an automation point
     * 
     * @param paramId The parameter ID
     * @param index The point index
     * @return true if the point was removed
     */
    bool removeAutomationPoint(const std::string& paramId, int index);
    
    /**
     * @brief Get an automation point
     * 
     * @param paramId The parameter ID
     * @param index The point index
     * @return The automation point, or nullptr if not found
     */
    const AutomationPoint* getAutomationPoint(const std::string& paramId, int index) const;
    
    /**
     * @brief Get the number of automation points for a parameter
     * 
     * @param paramId The parameter ID
     * @return The number of automation points
     */
    int getNumAutomationPoints(const std::string& paramId) const;
    
    /**
     * @brief Get all automation points for a parameter
     * 
     * @param paramId The parameter ID
     * @return Vector of automation points, or empty vector if parameter not found
     */
    const std::vector<AutomationPoint>& getAutomationPoints(const std::string& paramId) const;
    
    /**
     * @brief Get the value of a parameter at a specific time
     * 
     * @param paramId The parameter ID
     * @param time The time in beats
     * @param defaultValue The default value to return if no automation exists
     * @return The parameter value
     */
    float getParameterValueAtTime(const std::string& paramId, double time, float defaultValue = 0.0f) const;
    
    /**
     * @brief Get all parameter IDs that have automation
     * 
     * @return Vector of parameter IDs
     */
    std::vector<std::string> getAutomatedParameters() const;
    
    /**
     * @brief Clear all notes and automation
     */
    void clear();
    
    /**
     * @brief Clear notes only
     */
    void clearNotes();
    
    /**
     * @brief Clear automation only
     */
    void clearAutomation();
    
    /**
     * @brief Create an XML element containing the pattern's state
     * 
     * @return XML element containing pattern state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore pattern state from an XML element
     * 
     * @param xml XML element containing pattern state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
private:
    std::string name;
    double length; // Length in beats
    std::vector<NoteEvent> notes;
    std::unordered_map<std::string, std::vector<AutomationPoint>> automation;
    
    // Empty vector for returning when parameter not found
    static const std::vector<AutomationPoint> emptyAutomationPoints;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern)
};

} // namespace UndergroundBeats
