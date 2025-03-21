/*
 * Underground Beats
 * SynthModule.h
 * 
 * Synthesizer module that combines oscillators, envelopes, and filters
 */

#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"
#include <vector>
#include <memory>

namespace UndergroundBeats {

/**
 * @class SynthVoice
 * @brief A single voice for polyphonic synthesis
 * 
 * Represents a single voice in a polyphonic synthesizer, containing
 * oscillators, envelopes, and filters.
 */
class SynthVoice {
public:
    SynthVoice();
    ~SynthVoice();
    
    /**
     * @brief Check if the voice is currently active
     * 
     * @return true if the voice is playing a note
     */
    bool isActive() const;
    
    /**
     * @brief Start playing a note with this voice
     * 
     * @param midiNoteNumber The MIDI note number to play
     * @param velocity The velocity of the note (0 to 1)
     */
    void noteOn(int midiNoteNumber, float velocity);
    
    /**
     * @brief Stop playing the current note
     * 
     * @param allowTailOff Whether to allow envelope release phase
     */
    void noteOff(bool allowTailOff = true);
    
    /**
     * @brief Get the current MIDI note number
     * 
     * @return The MIDI note number currently playing, or -1 if not active
     */
    int getCurrentNote() const;
    
    /**
     * @brief Render audio for this voice
     * 
     * @param outputBuffer Buffer to write output to
     * @param numSamples Number of samples to generate
     */
    void renderNextBlock(float* outputBuffer, int numSamples);
    
    /**
     * @brief Set the oscillator waveform
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param type The waveform type
     */
    void setOscillatorWaveform(int oscillatorIndex, WaveformType type);
    
    /**
     * @brief Set the oscillator detune amount
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param cents Detune amount in cents
     */
    void setOscillatorDetune(int oscillatorIndex, float cents);
    
    /**
     * @brief Set the oscillator level
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param level Oscillator level (0 to 1)
     */
    void setOscillatorLevel(int oscillatorIndex, float level);
    
    /**
     * @brief Set the filter type
     * 
     * @param type The filter type
     */
    void setFilterType(FilterType type);
    
    /**
     * @brief Set the filter cutoff frequency
     * 
     * @param frequencyHz Cutoff frequency in Hertz
     */
    void setFilterCutoff(float frequencyHz);
    
    /**
     * @brief Set the filter resonance
     * 
     * @param amount Resonance amount (0 to 1)
     */
    void setFilterResonance(float amount);
    
    /**
     * @brief Set the ADSR envelope parameters
     * 
     * @param attackMs Attack time in milliseconds
     * @param decayMs Decay time in milliseconds
     * @param sustainLevel Sustain level (0 to 1)
     * @param releaseMs Release time in milliseconds
     */
    void setEnvelopeParameters(float attackMs, float decayMs, float sustainLevel, float releaseMs);
    
    /**
     * @brief Set the velocity sensitivity
     * 
     * @param sensitivity How much velocity affects output (0 to 1)
     */
    void setVelocitySensitivity(float sensitivity);
    
    /**
     * @brief Prepare the voice for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
private:
    // Voice state
    bool active;
    int currentNote;
    float currentVelocity;
    double currentSampleRate;
    
    // Synthesis components
    std::array<std::unique_ptr<Oscillator>, 2> oscillators;
    std::array<float, 2> oscillatorLevels;
    std::array<float, 2> oscillatorDetuneCents;
    
    std::unique_ptr<Envelope> ampEnvelope;
    std::unique_ptr<Envelope> filterEnvelope;
    std::unique_ptr<Filter> filter;
    
    // Parameters
    float velocitySensitivity;
    float filterEnvelopeAmount;
    
    // Temp buffer for processing
    juce::AudioBuffer<float> tempBuffer;
    
    // Convert MIDI note to frequency
    float midiNoteToFrequency(int midiNote, float cents = 0.0f) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};

/**
 * @class SynthModule
 * @brief Complete polyphonic synthesizer module
 * 
 * The SynthModule class implements a complete polyphonic synthesizer with
 * multiple voices, oscillators, filters, and envelopes.
 */
class SynthModule {
public:
    SynthModule(int numVoices = 8);
    ~SynthModule();
    
    /**
     * @brief Process incoming MIDI messages and generate audio
     * 
     * @param midiMessages MIDI messages to process
     * @param outputBuffer Buffer to write output to
     * @param numSamples Number of samples to generate
     */
    void processBlock(const juce::MidiBuffer& midiMessages, float* outputBuffer, int numSamples);
    
    /**
     * @brief Process MIDI messages and generate audio for stereo output
     * 
     * @param midiMessages MIDI messages to process
     * @param leftBuffer Left channel output buffer
     * @param rightBuffer Right channel output buffer
     * @param numSamples Number of samples to generate
     */
    void processStereoBlock(const juce::MidiBuffer& midiMessages, float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the synthesizer for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Set the oscillator waveform for all voices
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param type The waveform type
     */
    void setOscillatorWaveform(int oscillatorIndex, WaveformType type);
    
    /**
     * @brief Set the oscillator detune amount for all voices
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param cents Detune amount in cents
     */
    void setOscillatorDetune(int oscillatorIndex, float cents);
    
    /**
     * @brief Set the oscillator level for all voices
     * 
     * @param oscillatorIndex The oscillator to set (0 or 1)
     * @param level Oscillator level (0 to 1)
     */
    void setOscillatorLevel(int oscillatorIndex, float level);
    
    /**
     * @brief Set the filter type for all voices
     * 
     * @param type The filter type
     */
    void setFilterType(FilterType type);
    
    /**
     * @brief Set the filter cutoff frequency for all voices
     * 
     * @param frequencyHz Cutoff frequency in Hertz
     */
    void setFilterCutoff(float frequencyHz);
    
    /**
     * @brief Set the filter resonance for all voices
     * 
     * @param amount Resonance amount (0 to 1)
     */
    void setFilterResonance(float amount);
    
    /**
     * @brief Set the ADSR envelope parameters for all voices
     * 
     * @param attackMs Attack time in milliseconds
     * @param decayMs Decay time in milliseconds
     * @param sustainLevel Sustain level (0 to 1)
     * @param releaseMs Release time in milliseconds
     */
    void setEnvelopeParameters(float attackMs, float decayMs, float sustainLevel, float releaseMs);
    
    /**
     * @brief Set the velocity sensitivity for all voices
     * 
     * @param sensitivity How much velocity affects output (0 to 1)
     */
    void setVelocitySensitivity(float sensitivity);
    
private:
    std::vector<std::unique_ptr<SynthVoice>> voices;
    double currentSampleRate;
    
    // Find a free voice or steal one if needed
    SynthVoice* findFreeVoice(int midiNoteNumber, float velocity) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthModule)
};

} // namespace UndergroundBeats
