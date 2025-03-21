/*
 * Underground Beats
 * Sequencer.h
 * 
 * Manages sequencer playback and MIDI handling
 */

#pragma once

#include "Timeline.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <functional>

namespace UndergroundBeats {

/**
 * @class Sequencer
 * @brief Manages sequencer playback and MIDI handling
 * 
 * The Sequencer class manages playback of a timeline, generating MIDI events
 * based on the notes and automation in the timeline. It handles transport
 * controls, tempo, time signature, and quantization.
 */
class Sequencer : public juce::MidiKeyboardStateListener,
                 public juce::Timer {
public:
    Sequencer();
    ~Sequencer();
    
    /**
     * @brief Set the timeline to be played back
     * 
     * @param timeline The timeline to play
     */
    void setTimeline(std::shared_ptr<Timeline> timeline);
    
    /**
     * @brief Get the current timeline
     * 
     * @return The current timeline, or nullptr if none is set
     */
    std::shared_ptr<Timeline> getTimeline() const;
    
    /**
     * @brief Start playback
     */
    void play();
    
    /**
     * @brief Stop playback
     */
    void stop();
    
    /**
     * @brief Toggle between play and stop
     */
    void togglePlayStop();
    
    /**
     * @brief Set the playback position
     * 
     * @param positionInBeats The position in beats
     */
    void setPosition(double positionInBeats);
    
    /**
     * @brief Get the current playback position
     * 
     * @return The current position in beats
     */
    double getPosition() const;
    
    /**
     * @brief Check if the sequencer is currently playing
     * 
     * @return true if playing, false if stopped
     */
    bool isPlaying() const;
    
    /**
     * @brief Set the tempo in beats per minute
     * 
     * @param bpm Tempo in BPM
     */
    void setTempo(double bpm);
    
    /**
     * @brief Get the current tempo
     * 
     * @return The current tempo in BPM
     */
    double getTempo() const;
    
    /**
     * @brief Set the time signature
     * 
     * @param numerator Time signature numerator
     * @param denominator Time signature denominator
     */
    void setTimeSignature(int numerator, int denominator);
    
    /**
     * @brief Get the current time signature numerator
     * 
     * @return The time signature numerator
     */
    int getTimeSignatureNumerator() const;
    
    /**
     * @brief Get the current time signature denominator
     * 
     * @return The time signature denominator
     */
    int getTimeSignatureDenominator() const;
    
    /**
     * @brief Enable or disable looping
     * 
     * @param shouldLoop Whether to loop
     */
    void setLooping(bool shouldLoop);
    
    /**
     * @brief Check if looping is enabled
     * 
     * @return true if looping is enabled
     */
    bool isLooping() const;
    
    /**
     * @brief Set the loop start position
     * 
     * @param startInBeats Loop start position in beats
     */
    void setLoopStart(double startInBeats);
    
    /**
     * @brief Get the loop start position
     * 
     * @return The loop start position in beats
     */
    double getLoopStart() const;
    
    /**
     * @brief Set the loop end position
     * 
     * @param endInBeats Loop end position in beats
     */
    void setLoopEnd(double endInBeats);
    
    /**
     * @brief Get the loop end position
     * 
     * @return The loop end position in beats
     */
    double getLoopEnd() const;
    
    /**
     * @brief Set the quantization grid size
     * 
     * @param gridSize Grid size in beats (e.g., 0.25 for 16th notes)
     */
    void setQuantizationGrid(double gridSize);
    
    /**
     * @brief Get the current quantization grid size
     * 
     * @return The grid size in beats
     */
    double getQuantizationGrid() const;
    
    /**
     * @brief Process incoming MIDI messages
     * 
     * @param midiInput The MIDI messages to process
     * @param midiOutput Buffer to write output MIDI messages to
     */
    void processMidi(const juce::MidiBuffer& midiInput, juce::MidiBuffer& midiOutput);
    
    /**
     * @brief Set a callback function for note events
     * 
     * @param callback Function to call when note events occur
     */
    void setNoteEventCallback(std::function<void(const NoteEvent&)> callback);
    
    /**
     * @brief Set a callback function for parameter automation events
     * 
     * @param callback Function to call when parameter changes occur
     */
    void setParameterCallback(std::function<void(const std::string&, float)> callback);
    
    /**
     * @brief Quantize a time value to the current grid
     * 
     * @param time The time to quantize
     * @return The quantized time
     */
    double quantizeTime(double time) const;
    
    /**
     * @brief Handle note pressed events from a MIDI keyboard
     * 
     * @param keyboardState The keyboard state
     * @param midiChannel The MIDI channel
     * @param midiNoteNumber The MIDI note number
     * @param velocity The note velocity
     */
    void handleNoteOn(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity) override;
    
    /**
     * @brief Handle note released events from a MIDI keyboard
     * 
     * @param keyboardState The keyboard state
     * @param midiChannel The MIDI channel
     * @param midiNoteNumber The MIDI note number
     * @param velocity The release velocity
     */
    void handleNoteOff(juce::MidiKeyboardState* keyboardState, int midiChannel, int midiNoteNumber, float velocity) override;
    
    /**
     * @brief Timer callback (to advance playback position)
     */
    void timerCallback() override;
    
    /**
     * @brief Prepare the sequencer for playback
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    void prepare(double sampleRate, int blockSize);
    
    /**
     * @brief Create an XML element containing the sequencer's state
     * 
     * @return XML element containing sequencer state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore sequencer state from an XML element
     * 
     * @param xml XML element containing sequencer state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
private:
    std::shared_ptr<Timeline> timeline;
    double currentPosition; // Position in beats
    double tempo; // Tempo in BPM
    int timeSignatureNumerator;
    int timeSignatureDenominator;
    bool playing;
    bool looping;
    double loopStart; // Loop start in beats
    double loopEnd; // Loop end in beats
    double quantizationGrid; // Grid size in beats
    
    juce::MidiBuffer tempMidiBuffer;
    
    std::function<void(const NoteEvent&)> noteEventCallback;
    std::function<void(const std::string&, float)> parameterCallback;
    
    double currentSampleRate;
    int currentBlockSize;
    
    // Timer interval in milliseconds (higher values = lower CPU usage, but less accuracy)
    static constexpr int timerIntervalMs = 10;
    
    // Last time we generated events (to avoid duplicates)
    double lastEventPosition;
    
    // Currently playing notes (to handle note-offs correctly)
    struct ActiveNote {
        int note;
        int velocity;
        double startTime;
        double endTime;
    };
    
    std::vector<ActiveNote> activeNotes;
    
    // Convert beats to time in seconds
    double beatsToSeconds(double beats) const;
    
    // Convert time in seconds to beats
    double secondsToBeats(double seconds) const;
    
    // Convert time in seconds to samples
    int secondsToSamples(double seconds) const;
    
    // Generate events for the current position
    void generateEvents(double startPosition, double endPosition, juce::MidiBuffer& midiBuffer);
    
    // Check for note-offs that should happen
    void checkNoteOffs(double currentTime, juce::MidiBuffer& midiBuffer);
    
    // Generate parameter automation events
    void generateParameterEvents(double currentTime);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};

} // namespace UndergroundBeats
