/*
 * Underground Beats
 * Timeline.h
 * 
 * Manages the arrangement of patterns in a timeline
 */

#pragma once

#include "Pattern.h"
#include <JuceHeader.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace UndergroundBeats {

/**
 * @brief Structure to represent a pattern instance in the timeline
 */
struct PatternInstance {
    int patternId;     // ID of the pattern
    double startTime;  // Start time in beats
    double endTime;    // End time in beats (calculated from pattern length)
    bool muted;        // Whether the pattern is muted
    
    PatternInstance(int patternId = -1, double startTime = 0.0, bool muted = false)
        : patternId(patternId), startTime(startTime), endTime(0.0), muted(muted)
    {
    }
};

/**
 * @class Timeline
 * @brief Manages the arrangement of patterns in a timeline
 * 
 * The Timeline class manages the arrangement of patterns in a timeline,
 * providing methods for adding, removing, and moving pattern instances,
 * as well as querying the timeline for notes and automation data.
 */
class Timeline {
public:
    Timeline();
    ~Timeline();
    
    /**
     * @brief Add a pattern to the timeline's pattern library
     * 
     * @param pattern The pattern to add
     * @return The ID of the added pattern
     */
    int addPattern(std::unique_ptr<Pattern> pattern);
    
    /**
     * @brief Remove a pattern from the timeline's pattern library
     * 
     * @param patternId The ID of the pattern to remove
     * @return true if the pattern was removed
     */
    bool removePattern(int patternId);
    
    /**
     * @brief Get a pattern from the timeline's pattern library
     * 
     * @param patternId The ID of the pattern to get
     * @return Pointer to the pattern, or nullptr if not found
     */
    Pattern* getPattern(int patternId);
    
    /**
     * @brief Get all patterns in the timeline's pattern library
     * 
     * @return Map of pattern IDs to patterns
     */
    const std::unordered_map<int, std::unique_ptr<Pattern>>& getPatterns() const;
    
    /**
     * @brief Add a pattern instance to the timeline
     * 
     * @param patternId The ID of the pattern to add
     * @param startTime The start time in beats
     * @param muted Whether the pattern instance is muted
     * @return The index of the added pattern instance
     */
    int addPatternInstance(int patternId, double startTime, bool muted = false);
    
    /**
     * @brief Remove a pattern instance from the timeline
     * 
     * @param index The index of the pattern instance to remove
     * @return true if the pattern instance was removed
     */
    bool removePatternInstance(int index);
    
    /**
     * @brief Move a pattern instance to a new start time
     * 
     * @param index The index of the pattern instance to move
     * @param newStartTime The new start time in beats
     * @return true if the pattern instance was moved
     */
    bool movePatternInstance(int index, double newStartTime);
    
    /**
     * @brief Set whether a pattern instance is muted
     * 
     * @param index The index of the pattern instance
     * @param muted Whether the pattern instance is muted
     * @return true if the mute state was set
     */
    bool setPatternInstanceMuted(int index, bool muted);
    
    /**
     * @brief Get a pattern instance from the timeline
     * 
     * @param index The index of the pattern instance to get
     * @return Pointer to the pattern instance, or nullptr if not found
     */
    const PatternInstance* getPatternInstance(int index) const;
    
    /**
     * @brief Get the number of pattern instances in the timeline
     * 
     * @return The number of pattern instances
     */
    int getNumPatternInstances() const;
    
    /**
     * @brief Get all pattern instances in the timeline
     * 
     * @return Vector of pattern instances
     */
    const std::vector<PatternInstance>& getPatternInstances() const;
    
    /**
     * @brief Get all note events that occur within a time range
     * 
     * @param startTime Start time in beats
     * @param endTime End time in beats
     * @return Vector of note events with adjusted start times
     */
    std::vector<NoteEvent> getNotesInRange(double startTime, double endTime) const;
    
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
     * @brief Get the length of the timeline in beats
     * 
     * @return The timeline length
     */
    double getLength() const;
    
    /**
     * @brief Clear the timeline (remove all pattern instances)
     */
    void clear();
    
    /**
     * @brief Create an XML element containing the timeline's state
     * 
     * @return XML element containing timeline state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore timeline state from an XML element
     * 
     * @param xml XML element containing timeline state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
private:
    std::unordered_map<int, std::unique_ptr<Pattern>> patterns;
    std::vector<PatternInstance> patternInstances;
    int nextPatternId;
    
    // Update the end time of a pattern instance based on its pattern
    void updatePatternInstanceEndTime(int index);
    
    // Update all pattern instance end times
    void updateAllPatternInstanceEndTimes();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Timeline)
};

} // namespace UndergroundBeats
