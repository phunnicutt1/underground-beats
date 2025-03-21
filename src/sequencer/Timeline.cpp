/*
 * Underground Beats
 * Timeline.cpp
 * 
 * Implementation of timeline for pattern arrangement
 */

#include "Timeline.h"
#include <algorithm>

namespace UndergroundBeats {

Timeline::Timeline()
    : nextPatternId(0)
{
}

Timeline::~Timeline()
{
}

int Timeline::addPattern(std::unique_ptr<Pattern> pattern)
{
    if (pattern == nullptr)
    {
        return -1;
    }
    
    int patternId = nextPatternId++;
    patterns[patternId] = std::move(pattern);
    
    // Update any pattern instances that use this pattern
    updateAllPatternInstanceEndTimes();
    
    return patternId;
}

bool Timeline::removePattern(int patternId)
{
    auto it = patterns.find(patternId);
    if (it == patterns.end())
    {
        return false;
    }
    
    // Remove all instances of this pattern from the timeline
    auto newEnd = std::remove_if(patternInstances.begin(), patternInstances.end(),
                               [patternId](const PatternInstance& instance) {
                                   return instance.patternId == patternId;
                               });
    
    patternInstances.erase(newEnd, patternInstances.end());
    
    // Remove the pattern from the library
    patterns.erase(it);
    
    return true;
}

Pattern* Timeline::getPattern(int patternId)
{
    auto it = patterns.find(patternId);
    if (it == patterns.end())
    {
        return nullptr;
    }
    
    return it->second.get();
}

const std::unordered_map<int, std::unique_ptr<Pattern>>& Timeline::getPatterns() const
{
    return patterns;
}

int Timeline::addPatternInstance(int patternId, double startTime, bool muted)
{
    auto it = patterns.find(patternId);
    if (it == patterns.end())
    {
        return -1;
    }
    
    // Create the pattern instance
    PatternInstance instance(patternId, startTime, muted);
    
    // Calculate end time based on pattern length
    instance.endTime = startTime + it->second->getLength();
    
    // Add the instance to the timeline
    patternInstances.push_back(instance);
    
    // Sort by start time
    std::sort(patternInstances.begin(), patternInstances.end(),
             [](const PatternInstance& a, const PatternInstance& b) {
                 return a.startTime < b.startTime;
             });
    
    // Find the index of the added instance
    auto instanceIt = std::find_if(patternInstances.begin(), patternInstances.end(),
                                 [patternId, startTime](const PatternInstance& p) {
                                     return p.patternId == patternId && p.startTime == startTime;
                                 });
    
    return static_cast<int>(std::distance(patternInstances.begin(), instanceIt));
}

bool Timeline::removePatternInstance(int index)
{
    if (index < 0 || index >= static_cast<int>(patternInstances.size()))
    {
        return false;
    }
    
    patternInstances.erase(patternInstances.begin() + index);
    
    return true;
}

bool Timeline::movePatternInstance(int index, double newStartTime)
{
    if (index < 0 || index >= static_cast<int>(patternInstances.size()))
    {
        return false;
    }
    
    // Update start time
    patternInstances[index].startTime = newStartTime;
    
    // Update end time
    updatePatternInstanceEndTime(index);
    
    // Sort by start time
    std::sort(patternInstances.begin(), patternInstances.end(),
             [](const PatternInstance& a, const PatternInstance& b) {
                 return a.startTime < b.startTime;
             });
    
    return true;
}

bool Timeline::setPatternInstanceMuted(int index, bool muted)
{
    if (index < 0 || index >= static_cast<int>(patternInstances.size()))
    {
        return false;
    }
    
    patternInstances[index].muted = muted;
    
    return true;
}

const PatternInstance* Timeline::getPatternInstance(int index) const
{
    if (index < 0 || index >= static_cast<int>(patternInstances.size()))
    {
        return nullptr;
    }
    
    return &patternInstances[index];
}

int Timeline::getNumPatternInstances() const
{
    return static_cast<int>(patternInstances.size());
}

const std::vector<PatternInstance>& Timeline::getPatternInstances() const
{
    return patternInstances;
}

std::vector<NoteEvent> Timeline::getNotesInRange(double startTime, double endTime) const
{
    std::vector<NoteEvent> result;
    
    // For each pattern instance that overlaps with the range
    for (const auto& instance : patternInstances)
    {
        // Skip muted patterns
        if (instance.muted)
        {
            continue;
        }
        
        // Check if the pattern instance overlaps with the requested range
        if (instance.endTime > startTime && instance.startTime < endTime)
        {
            auto patternIt = patterns.find(instance.patternId);
            if (patternIt == patterns.end())
            {
                continue;
            }
            
            const Pattern* pattern = patternIt->second.get();
            
            // Calculate the intersection of the pattern time range and the requested range
            double patternStartOffset = std::max(0.0, startTime - instance.startTime);
            double patternEndOffset = std::min(pattern->getLength(), endTime - instance.startTime);
            
            // Get all notes in the pattern
            const auto& notes = pattern->getNotes();
            
            for (const auto& note : notes)
            {
                // Check if the note falls within the intersection
                if (note.startTime >= patternStartOffset && note.startTime < patternEndOffset)
                {
                    // Add the note with adjusted start time
                    NoteEvent adjustedNote = note;
                    adjustedNote.startTime += instance.startTime - startTime;
                    result.push_back(adjustedNote);
                }
            }
        }
    }
    
    return result;
}

float Timeline::getParameterValueAtTime(const std::string& paramId, double time, float defaultValue) const
{
    // Find the pattern instance that contains the requested time
    for (const auto& instance : patternInstances)
    {
        // Skip muted patterns
        if (instance.muted)
        {
            continue;
        }
        
        // Check if the time falls within this pattern instance
        if (time >= instance.startTime && time < instance.endTime)
        {
            auto patternIt = patterns.find(instance.patternId);
            if (patternIt == patterns.end())
            {
                continue;
            }
            
            const Pattern* pattern = patternIt->second.get();
            
            // Get the parameter value from the pattern
            double patternTime = time - instance.startTime;
            return pattern->getParameterValueAtTime(paramId, patternTime, defaultValue);
        }
    }
    
    return defaultValue;
}

double Timeline::getLength() const
{
    // Find the end time of the last pattern instance
    double maxEndTime = 0.0;
    
    for (const auto& instance : patternInstances)
    {
        maxEndTime = std::max(maxEndTime, instance.endTime);
    }
    
    return maxEndTime;
}

void Timeline::clear()
{
    patternInstances.clear();
}

std::unique_ptr<juce::XmlElement> Timeline::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("Timeline");
    
    // Add patterns
    auto patternsXml = xml->createNewChildElement("Patterns");
    
    for (const auto& pair : patterns)
    {
        auto patternXml = pair.second->createStateXml();
        patternXml->setAttribute("id", pair.first);
        patternsXml->addChildElement(patternXml.release());
    }
    
    // Add pattern instances
    auto instancesXml = xml->createNewChildElement("PatternInstances");
    
    for (const auto& instance : patternInstances)
    {
        auto instanceXml = instancesXml->createNewChildElement("Instance");
        instanceXml->setAttribute("patternId", instance.patternId);
        instanceXml->setAttribute("startTime", instance.startTime);
        instanceXml->setAttribute("muted", instance.muted);
    }
    
    // Add next pattern ID
    xml->setAttribute("nextPatternId", nextPatternId);
    
    return xml;
}

bool Timeline::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "Timeline")
    {
        return false;
    }
    
    // Clear existing data
    patterns.clear();
    patternInstances.clear();
    
    // Restore next pattern ID
    nextPatternId = xml->getIntAttribute("nextPatternId", 0);
    
    // Restore patterns
    auto patternsXml = xml->getChildByName("Patterns");
    if (patternsXml != nullptr)
    {
        for (auto* patternXml : patternsXml->getChildIterator())
        {
            auto pattern = std::make_unique<Pattern>();
            if (pattern->restoreStateFromXml(patternXml))
            {
                int patternId = patternXml->getIntAttribute("id", -1);
                if (patternId >= 0)
                {
                    patterns[patternId] = std::move(pattern);
                }
            }
        }
    }
    
    // Restore pattern instances
    auto instancesXml = xml->getChildByName("PatternInstances");
    if (instancesXml != nullptr)
    {
        for (auto* instanceXml : instancesXml->getChildWithTagNameIterator("Instance"))
        {
            int patternId = instanceXml->getIntAttribute("patternId", -1);
            double startTime = instanceXml->getDoubleAttribute("startTime", 0.0);
            bool muted = instanceXml->getBoolAttribute("muted", false);
            
            if (patternId >= 0)
            {
                addPatternInstance(patternId, startTime, muted);
            }
        }
    }
    
    return true;
}

void Timeline::updatePatternInstanceEndTime(int index)
{
    if (index < 0 || index >= static_cast<int>(patternInstances.size()))
    {
        return;
    }
    
    auto& instance = patternInstances[index];
    
    auto patternIt = patterns.find(instance.patternId);
    if (patternIt == patterns.end())
    {
        return;
    }
    
    instance.endTime = instance.startTime + patternIt->second->getLength();
}

void Timeline::updateAllPatternInstanceEndTimes()
{
    for (size_t i = 0; i < patternInstances.size(); ++i)
    {
        updatePatternInstanceEndTime(static_cast<int>(i));
    }
}

} // namespace UndergroundBeats
