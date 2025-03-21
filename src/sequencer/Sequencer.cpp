/*
 * Underground Beats
 * Sequencer.cpp
 * 
 * Implementation of sequencer for playback and MIDI handling
 */

#include "Sequencer.h"

namespace UndergroundBeats {

Sequencer::Sequencer()
    : currentPosition(0.0)
    , tempo(120.0)
    , timeSignatureNumerator(4)
    , timeSignatureDenominator(4)
    , playing(false)
    , looping(false)
    , loopStart(0.0)
    , loopEnd(4.0)
    , quantizationGrid(0.25) // 16th notes by default
    , currentSampleRate(44100.0)
    , currentBlockSize(512)
    , lastEventPosition(0.0)
{
}

Sequencer::~Sequencer()
{
    stop();
}

void Sequencer::setTimeline(std::shared_ptr<Timeline> timeline)
{
    this->timeline = timeline;
}

std::shared_ptr<Timeline> Sequencer::getTimeline() const
{
    return timeline;
}

void Sequencer::play()
{
    if (!playing)
    {
        // Start playback
        playing = true;
        lastEventPosition = currentPosition;
        
        // Start the timer for advancing the position
        startTimer(timerIntervalMs);
    }
}

void Sequencer::stop()
{
    if (playing)
    {
        // Stop playback
        playing = false;
        
        // Stop the timer
        stopTimer();
        
        // Turn off any active notes
        juce::MidiBuffer noteOffs;
        for (const auto& note : activeNotes)
        {
            noteOffs.addEvent(juce::MidiMessage::noteOff(1, note.note), 0);
        }
        
        // Clear active notes
        activeNotes.clear();
        
        // Call note event callback for note-offs
        if (noteEventCallback)
        {
            for (const auto& note : activeNotes)
            {
                NoteEvent noteEvent(note.note, 0, currentPosition, 0.0);
                noteEventCallback(noteEvent);
            }
        }
    }
}

void Sequencer::togglePlayStop()
{
    if (playing)
    {
        stop();
    }
    else
    {
        play();
    }
}

void Sequencer::setPosition(double positionInBeats)
{
    if (positionInBeats < 0.0)
    {
        positionInBeats = 0.0;
    }
    
    // If playing, update the last event position to avoid duplicate events
    if (playing)
    {
        lastEventPosition = positionInBeats;
    }
    
    currentPosition = positionInBeats;
}

double Sequencer::getPosition() const
{
    return currentPosition;
}

bool Sequencer::isPlaying() const
{
    return playing;
}

void Sequencer::setTempo(double bpm)
{
    if (bpm > 0.0)
    {
        tempo = bpm;
    }
}

double Sequencer::getTempo() const
{
    return tempo;
}

void Sequencer::setTimeSignature(int numerator, int denominator)
{
    if (numerator > 0 && denominator > 0)
    {
        timeSignatureNumerator = numerator;
        timeSignatureDenominator = denominator;
    }
}

int Sequencer::getTimeSignatureNumerator() const
{
    return timeSignatureNumerator;
}

int Sequencer::getTimeSignatureDenominator() const
{
    return timeSignatureDenominator;
}

void Sequencer::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
}

bool Sequencer::isLooping() const
{
    return looping;
}

void Sequencer::setLoopStart(double startInBeats)
{
    if (startInBeats >= 0.0 && startInBeats < loopEnd)
    {
        loopStart = startInBeats;
    }
}

double Sequencer::getLoopStart() const
{
    return loopStart;
}

void Sequencer::setLoopEnd(double endInBeats)
{
    if (endInBeats > loopStart)
    {
        loopEnd = endInBeats;
    }
}

double Sequencer::getLoopEnd() const
{
    return loopEnd;
}

void Sequencer::setQuantizationGrid(double gridSize)
{
    if (gridSize > 0.0)
    {
        quantizationGrid = gridSize;
    }
}

double Sequencer::getQuantizationGrid() const
{
    return quantizationGrid;
}

void Sequencer::processMidi(const juce::MidiBuffer& midiInput, juce::MidiBuffer& midiOutput)
{
    if (!playing || timeline == nullptr)
    {
        // If not playing, just pass through MIDI
        midiOutput.addEvents(midiInput, 0, -1, 0);
        return;
    }
    
    // Clear the output buffer
    midiOutput.clear();
    
    // Calculate how many beats this block represents
    double blockTimeInSeconds = currentBlockSize / currentSampleRate;
    double blockTimeInBeats = secondsToBeats(blockTimeInSeconds);
    
    // Generate events for this block
    double startPosition = currentPosition;
    double endPosition = startPosition + blockTimeInBeats;
    
    // Advance the position
    currentPosition = endPosition;
    
    // Check for loop end
    if (looping && currentPosition >= loopEnd)
    {
        // Wrap around to loop start
        double overflow = currentPosition - loopEnd;
        currentPosition = loopStart + overflow;
        
        // Generate events for the end of the loop
        generateEvents(startPosition, loopEnd, midiOutput);
        
        // Generate events for the start of the loop
        generateEvents(loopStart, currentPosition, midiOutput);
    }
    else
    {
        // Generate events for the current block
        generateEvents(startPosition, endPosition, midiOutput);
    }
    
    // Also process any incoming MIDI
    midiOutput.addEvents(midiInput, 0, -1, 0);
}

void Sequencer::setNoteEventCallback(std::function<void(const NoteEvent&)> callback)
{
    noteEventCallback = callback;
}

void Sequencer::setParameterCallback(std::function<void(const std::string&, float)> callback)
{
    parameterCallback = callback;
}

double Sequencer::quantizeTime(double time) const
{
    // Quantize to the nearest grid point
    return std::round(time / quantizationGrid) * quantizationGrid;
}

void Sequencer::handleNoteOn(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity)
{
    // Add to MIDI buffer for the next process block
    tempMidiBuffer.addEvent(juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity), 0);
    
    // Call note event callback
    if (noteEventCallback)
    {
        NoteEvent noteEvent(midiNoteNumber, static_cast<int>(velocity * 127.0f), currentPosition, 0.0);
        noteEventCallback(noteEvent);
    }
}

void Sequencer::handleNoteOff(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity)
{
    // Add to MIDI buffer for the next process block
    tempMidiBuffer.addEvent(juce::MidiMessage::noteOff(midiChannel, midiNoteNumber), 0);
    
    // Call note event callback
    if (noteEventCallback)
    {
        NoteEvent noteEvent(midiNoteNumber, 0, currentPosition, 0.0);
        noteEventCallback(noteEvent);
    }
}

void Sequencer::timerCallback()
{
    if (playing)
    {
        // Calculate how many beats have passed since the last timer callback
        double timeInSeconds = timerIntervalMs / 1000.0;
        double timeInBeats = secondsToBeats(timeInSeconds);
        
        // Advance the position
        double startPosition = currentPosition;
        currentPosition += timeInBeats;
        
        // Check for loop end
        if (looping && currentPosition >= loopEnd)
        {
            // Wrap around to loop start
            double overflow = currentPosition - loopEnd;
            currentPosition = loopStart + overflow;
            
            // Generate parameter events for the end of the loop
            generateParameterEvents(loopEnd);
            
            // Generate parameter events for the start of the loop
            generateParameterEvents(loopStart);
        }
        else
        {
            // Generate parameter events
            generateParameterEvents(currentPosition);
        }
    }
}

void Sequencer::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;
}

std::unique_ptr<juce::XmlElement> Sequencer::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("Sequencer");
    
    // Add sequencer attributes
    xml->setAttribute("position", currentPosition);
    xml->setAttribute("tempo", tempo);
    xml->setAttribute("timeSignatureNumerator", timeSignatureNumerator);
    xml->setAttribute("timeSignatureDenominator", timeSignatureDenominator);
    xml->setAttribute("looping", looping);
    xml->setAttribute("loopStart", loopStart);
    xml->setAttribute("loopEnd", loopEnd);
    xml->setAttribute("quantizationGrid", quantizationGrid);
    
    return xml;
}

bool Sequencer::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "Sequencer")
    {
        return false;
    }
    
    // Restore sequencer attributes
    currentPosition = xml->getDoubleAttribute("position", 0.0);
    tempo = xml->getDoubleAttribute("tempo", 120.0);
    timeSignatureNumerator = xml->getIntAttribute("timeSignatureNumerator", 4);
    timeSignatureDenominator = xml->getIntAttribute("timeSignatureDenominator", 4);
    looping = xml->getBoolAttribute("looping", false);
    loopStart = xml->getDoubleAttribute("loopStart", 0.0);
    loopEnd = xml->getDoubleAttribute("loopEnd", 4.0);
    quantizationGrid = xml->getDoubleAttribute("quantizationGrid", 0.25);
    
    return true;
}

double Sequencer::beatsToSeconds(double beats) const
{
    // Convert beats to seconds based on the current tempo
    return (beats * 60.0) / tempo;
}

double Sequencer::secondsToBeats(double seconds) const
{
    // Convert seconds to beats based on the current tempo
    return (seconds * tempo) / 60.0;
}

int Sequencer::secondsToSamples(double seconds) const
{
    // Convert seconds to samples based on the current sample rate
    return static_cast<int>(seconds * currentSampleRate);
}

void Sequencer::generateEvents(double startPosition, double endPosition, juce::MidiBuffer& midiBuffer)
{
    if (timeline == nullptr)
    {
        return;
    }
    
    // Get notes that fall within the time range
    auto notes = timeline->getNotesInRange(startPosition, endPosition);
    
    // Process each note
    for (const auto& note : notes)
    {
        // Calculate sample position relative to the start of the buffer
        double noteStartTimeInSeconds = beatsToSeconds(note.startTime);
        int samplePosition = secondsToSamples(noteStartTimeInSeconds);
        
        // Add note-on event
        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, note.note, static_cast<float>(note.velocity) / 127.0f), samplePosition);
        
        // Add an active note so we can generate note-off later
        ActiveNote activeNote;
        activeNote.note = note.note;
        activeNote.velocity = note.velocity;
        activeNote.startTime = startPosition + note.startTime;
        activeNote.endTime = activeNote.startTime + note.duration;
        activeNotes.push_back(activeNote);
        
        // Call note event callback
        if (noteEventCallback)
        {
            noteEventCallback(note);
        }
    }
    
    // Check for any note-offs that should happen in this block
    checkNoteOffs(endPosition, midiBuffer);
    
    // Generate parameter automation events
    generateParameterEvents(endPosition);
    
    // Update last event position
    lastEventPosition = endPosition;
}

void Sequencer::checkNoteOffs(double currentTime, juce::MidiBuffer& midiBuffer)
{
    // Check for any note-offs that should happen
    auto it = activeNotes.begin();
    while (it != activeNotes.end())
    {
        if (it->endTime <= currentTime)
        {
            // Calculate sample position relative to the start of the buffer
            double noteEndTimeInSeconds = beatsToSeconds(it->endTime - lastEventPosition);
            int samplePosition = secondsToSamples(noteEndTimeInSeconds);
            
            // Add note-off event
            midiBuffer.addEvent(juce::MidiMessage::noteOff(1, it->note), samplePosition);
            
            // Call note event callback
            if (noteEventCallback)
            {
                NoteEvent noteEvent(it->note, 0, it->endTime, 0.0);
                noteEventCallback(noteEvent);
            }
            
            // Remove the note from active notes
            it = activeNotes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Sequencer::generateParameterEvents(double currentTime)
{
    if (timeline == nullptr || !parameterCallback)
    {
        return;
    }
    
    // Get all parameters that have automation
    if (timeline->getTimeline() != nullptr)
    {
        auto parameterIds = timeline->getTimeline()->getAutomatedParameters();
        
        // Generate parameter events for each automated parameter
        for (const auto& paramId : parameterIds)
        {
            float value = timeline->getParameterValueAtTime(paramId, currentTime);
            parameterCallback(paramId, value);
        }
    }
}

} // namespace UndergroundBeats
