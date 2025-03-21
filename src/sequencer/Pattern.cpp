/*
 * Underground Beats
 * Pattern.cpp
 * 
 * Implementation of pattern data structure
 */

#include "Pattern.h"
#include <algorithm>

namespace UndergroundBeats {

// Initialize static member
const std::vector<AutomationPoint> Pattern::emptyAutomationPoints;

Pattern::Pattern(const std::string& name, double lengthInBeats)
    : name(name)
    , length(lengthInBeats)
{
}

Pattern::~Pattern()
{
}

void Pattern::setName(const std::string& name)
{
    this->name = name;
}

std::string Pattern::getName() const
{
    return name;
}

void Pattern::setLength(double lengthInBeats)
{
    if (lengthInBeats > 0.0)
    {
        length = lengthInBeats;
    }
}

double Pattern::getLength() const
{
    return length;
}

int Pattern::addNote(int note, int velocity, double startTime, double duration)
{
    // Clamp MIDI note values
    note = juce::jlimit(0, 127, note);
    velocity = juce::jlimit(1, 127, velocity);
    
    // Ensure positive duration
    duration = std::max(0.001, duration);
    
    // Add the note
    notes.push_back({note, velocity, startTime, duration});
    
    // Return the index of the added note
    return static_cast<int>(notes.size() - 1);
}

int Pattern::addNote(const NoteEvent& noteEvent)
{
    // Copy the note event to ensure proper clamping
    return addNote(noteEvent.note, noteEvent.velocity, noteEvent.startTime, noteEvent.duration);
}

bool Pattern::editNote(int index, int note, int velocity, double startTime, double duration)
{
    if (index < 0 || index >= static_cast<int>(notes.size()))
    {
        return false;
    }
    
    // Clamp MIDI note values
    note = juce::jlimit(0, 127, note);
    velocity = juce::jlimit(1, 127, velocity);
    
    // Ensure positive duration
    duration = std::max(0.001, duration);
    
    // Update the note
    notes[index].note = note;
    notes[index].velocity = velocity;
    notes[index].startTime = startTime;
    notes[index].duration = duration;
    
    return true;
}

bool Pattern::removeNote(int index)
{
    if (index < 0 || index >= static_cast<int>(notes.size()))
    {
        return false;
    }
    
    // Remove the note
    notes.erase(notes.begin() + index);
    
    return true;
}

const NoteEvent* Pattern::getNote(int index) const
{
    if (index < 0 || index >= static_cast<int>(notes.size()))
    {
        return nullptr;
    }
    
    return &notes[index];
}

int Pattern::getNumNotes() const
{
    return static_cast<int>(notes.size());
}

const std::vector<NoteEvent>& Pattern::getNotes() const
{
    return notes;
}

int Pattern::addAutomationPoint(const std::string& paramId, double time, float value, int curveType)
{
    // Clamp value to 0-1 range
    value = juce::jlimit(0.0f, 1.0f, value);
    
    // Create the automation point
    AutomationPoint point(time, value, curveType);
    
    // Add the point to the parameter's automation
    auto& points = automation[paramId];
    points.push_back(point);
    
    // Sort by time
    std::sort(points.begin(), points.end(), 
              [](const AutomationPoint& a, const AutomationPoint& b) {
                  return a.time < b.time;
              });
    
    // Find the index of the added point
    auto it = std::find_if(points.begin(), points.end(),
                          [time](const AutomationPoint& p) {
                              return p.time == time;
                          });
    
    return static_cast<int>(std::distance(points.begin(), it));
}

bool Pattern::editAutomationPoint(const std::string& paramId, int index, double time, float value, int curveType)
{
    auto it = automation.find(paramId);
    if (it == automation.end())
    {
        return false;
    }
    
    auto& points = it->second;
    
    if (index < 0 || index >= static_cast<int>(points.size()))
    {
        return false;
    }
    
    // Clamp value to 0-1 range
    value = juce::jlimit(0.0f, 1.0f, value);
    
    // Update the point
    points[index].time = time;
    points[index].value = value;
    points[index].curveType = curveType;
    
    // Sort by time
    std::sort(points.begin(), points.end(), 
              [](const AutomationPoint& a, const AutomationPoint& b) {
                  return a.time < b.time;
              });
    
    return true;
}

bool Pattern::removeAutomationPoint(const std::string& paramId, int index)
{
    auto it = automation.find(paramId);
    if (it == automation.end())
    {
        return false;
    }
    
    auto& points = it->second;
    
    if (index < 0 || index >= static_cast<int>(points.size()))
    {
        return false;
    }
    
    // Remove the point
    points.erase(points.begin() + index);
    
    // If no more points, remove the parameter from the map
    if (points.empty())
    {
        automation.erase(it);
    }
    
    return true;
}

const AutomationPoint* Pattern::getAutomationPoint(const std::string& paramId, int index) const
{
    auto it = automation.find(paramId);
    if (it == automation.end())
    {
        return nullptr;
    }
    
    const auto& points = it->second;
    
    if (index < 0 || index >= static_cast<int>(points.size()))
    {
        return nullptr;
    }
    
    return &points[index];
}

int Pattern::getNumAutomationPoints(const std::string& paramId) const
{
    auto it = automation.find(paramId);
    if (it == automation.end())
    {
        return 0;
    }
    
    return static_cast<int>(it->second.size());
}

const std::vector<AutomationPoint>& Pattern::getAutomationPoints(const std::string& paramId) const
{
    auto it = automation.find(paramId);
    if (it == automation.end())
    {
        return emptyAutomationPoints;
    }
    
    return it->second;
}

float Pattern::getParameterValueAtTime(const std::string& paramId, double time, float defaultValue) const
{
    auto it = automation.find(paramId);
    if (it == automation.end() || it->second.empty())
    {
        return defaultValue;
    }
    
    const auto& points = it->second;
    
    // If time is before first point, return first point value
    if (time <= points.front().time)
    {
        return points.front().value;
    }
    
    // If time is after last point, return last point value
    if (time >= points.back().time)
    {
        return points.back().value;
    }
    
    // Find the points that surround the requested time
    auto nextPoint = std::find_if(points.begin(), points.end(),
                                 [time](const AutomationPoint& p) {
                                     return p.time > time;
                                 });
    
    auto prevPoint = nextPoint - 1;
    
    // Calculate time position between points (0 to 1)
    double timeFraction = (time - prevPoint->time) / (nextPoint->time - prevPoint->time);
    
    // Linear interpolation by default
    float result = prevPoint->value + (nextPoint->value - prevPoint->value) * static_cast<float>(timeFraction);
    
    // Apply curve type if needed
    // (Basic implementation - could be extended with more curve types)
    switch (prevPoint->curveType)
    {
        case 1: // Exponential
            result = prevPoint->value + (nextPoint->value - prevPoint->value) * static_cast<float>(timeFraction * timeFraction);
            break;
        case 2: // Logarithmic
            result = prevPoint->value + (nextPoint->value - prevPoint->value) * static_cast<float>(std::sqrt(timeFraction));
            break;
        case 3: // S-curve
        {
            float s = static_cast<float>(timeFraction);
            s = s * s * (3.0f - 2.0f * s); // Smooth step
            result = prevPoint->value + (nextPoint->value - prevPoint->value) * s;
            break;
        }
        default: // Linear (0)
            // Already calculated above
            break;
    }
    
    return result;
}

std::vector<std::string> Pattern::getAutomatedParameters() const
{
    std::vector<std::string> paramIds;
    paramIds.reserve(automation.size());
    
    for (const auto& pair : automation)
    {
        paramIds.push_back(pair.first);
    }
    
    return paramIds;
}

void Pattern::clear()
{
    notes.clear();
    automation.clear();
}

void Pattern::clearNotes()
{
    notes.clear();
}

void Pattern::clearAutomation()
{
    automation.clear();
}

std::unique_ptr<juce::XmlElement> Pattern::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("Pattern");
    
    // Add pattern attributes
    xml->setAttribute("name", name);
    xml->setAttribute("length", length);
    
    // Add notes
    auto notesXml = xml->createNewChildElement("Notes");
    
    for (const auto& note : notes)
    {
        auto noteXml = notesXml->createNewChildElement("Note");
        noteXml->setAttribute("note", note.note);
        noteXml->setAttribute("velocity", note.velocity);
        noteXml->setAttribute("startTime", note.startTime);
        noteXml->setAttribute("duration", note.duration);
    }
    
    // Add automation
    auto automationXml = xml->createNewChildElement("Automation");
    
    for (const auto& pair : automation)
    {
        auto paramXml = automationXml->createNewChildElement("Parameter");
        paramXml->setAttribute("id", pair.first);
        
        for (const auto& point : pair.second)
        {
            auto pointXml = paramXml->createNewChildElement("Point");
            pointXml->setAttribute("time", point.time);
            pointXml->setAttribute("value", point.value);
            pointXml->setAttribute("curveType", point.curveType);
        }
    }
    
    return xml;
}

bool Pattern::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "Pattern")
    {
        return false;
    }
    
    // Clear existing data
    clear();
    
    // Restore pattern attributes
    name = xml->getStringAttribute("name", "Untitled Pattern");
    length = xml->getDoubleAttribute("length", 4.0);
    
    // Restore notes
    auto notesXml = xml->getChildByName("Notes");
    if (notesXml != nullptr)
    {
        for (auto* noteXml : notesXml->getChildWithTagNameIterator("Note"))
        {
            int note = noteXml->getIntAttribute("note", 60);
            int velocity = noteXml->getIntAttribute("velocity", 100);
            double startTime = noteXml->getDoubleAttribute("startTime", 0.0);
            double duration = noteXml->getDoubleAttribute("duration", 1.0);
            
            addNote(note, velocity, startTime, duration);
        }
    }
    
    // Restore automation
    auto automationXml = xml->getChildByName("Automation");
    if (automationXml != nullptr)
    {
        for (auto* paramXml : automationXml->getChildWithTagNameIterator("Parameter"))
        {
            std::string paramId = paramXml->getStringAttribute("id", "").toStdString();
            
            if (!paramId.empty())
            {
                for (auto* pointXml : paramXml->getChildWithTagNameIterator("Point"))
                {
                    double time = pointXml->getDoubleAttribute("time", 0.0);
                    float value = pointXml->getFloatAttribute("value", 0.0f);
                    int curveType = pointXml->getIntAttribute("curveType", 0);
                    
                    addAutomationPoint(paramId, time, value, curveType);
                }
            }
        }
    }
    
    return true;
}

} // namespace UndergroundBeats
