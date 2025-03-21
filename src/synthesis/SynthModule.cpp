/*
 * Underground Beats
 * SynthModule.cpp
 * 
 * Implementation of synthesizer module
 */

#include "SynthModule.h"

namespace UndergroundBeats {

//==============================================================================
// SynthVoice Implementation
//==============================================================================

SynthVoice::SynthVoice()
    : active(false)
    , currentNote(-1)
    , currentVelocity(0.0f)
    , currentSampleRate(44100.0)
    , oscillatorLevels({0.5f, 0.5f})
    , oscillatorDetuneCents({0.0f, 0.0f})
    , velocitySensitivity(0.7f)
    , filterEnvelopeAmount(0.5f)
{
    // Create oscillators
    for (auto& osc : oscillators)
    {
        osc = std::make_unique<Oscillator>();
    }
    
    // Set second oscillator's default detune
    oscillatorDetuneCents[1] = 5.0f;
    
    // Create envelopes
    ampEnvelope = std::make_unique<Envelope>();
    filterEnvelope = std::make_unique<Envelope>();
    
    // Set default envelope parameters
    ampEnvelope->setAttackTime(10.0f);
    ampEnvelope->setDecayTime(100.0f);
    ampEnvelope->setSustainLevel(0.7f);
    ampEnvelope->setReleaseTime(200.0f);
    
    filterEnvelope->setAttackTime(50.0f);
    filterEnvelope->setDecayTime(500.0f);
    filterEnvelope->setSustainLevel(0.5f);
    filterEnvelope->setReleaseTime(500.0f);
    
    // Create filter
    filter = std::make_unique<Filter>();
    filter->setCutoff(1000.0f);
    filter->setResonance(0.5f);
    
    // Allocate temp buffer
    tempBuffer.setSize(1, 512);
}

SynthVoice::~SynthVoice()
{
}

bool SynthVoice::isActive() const
{
    return active;
}

void SynthVoice::noteOn(int midiNoteNumber, float velocity)
{
    currentNote = midiNoteNumber;
    currentVelocity = velocity;
    active = true;
    
    // Set oscillator frequencies based on the MIDI note
    oscillators[0]->setFrequency(midiNoteToFrequency(midiNoteNumber, oscillatorDetuneCents[0]));
    oscillators[1]->setFrequency(midiNoteToFrequency(midiNoteNumber, oscillatorDetuneCents[1]));
    
    // Reset oscillator phases to avoid clicks
    oscillators[0]->resetPhase();
    oscillators[1]->resetPhase();
    
    // Trigger envelopes
    ampEnvelope->noteOn();
    filterEnvelope->noteOn();
}

void SynthVoice::noteOff(bool allowTailOff)
{
    if (allowTailOff)
    {
        // Start envelope release phase
        ampEnvelope->noteOff();
        filterEnvelope->noteOff();
    }
    else
    {
        // Stop immediately
        active = false;
        ampEnvelope->reset();
        filterEnvelope->reset();
        currentNote = -1;
    }
}

int SynthVoice::getCurrentNote() const
{
    return currentNote;
}

void SynthVoice::renderNextBlock(float* outputBuffer, int numSamples)
{
    if (!active)
        return;
    
    // Ensure temp buffer is large enough
    if (tempBuffer.getNumSamples() < numSamples)
    {
        tempBuffer.setSize(1, numSamples, false, true, true);
    }
    
    // Clear the buffer
    tempBuffer.clear();
    float* tempData = tempBuffer.getWritePointer(0);
    
    // Generate audio from oscillators
    for (size_t i = 0; i < oscillators.size(); ++i)
    {
        // Generate samples from this oscillator
        oscillators[i]->process(tempData, numSamples);
        
        // Apply oscillator level
        juce::FloatVectorOperations::multiply(tempData, oscillatorLevels[i], numSamples);
        
        // Mix into output buffer
        juce::FloatVectorOperations::add(outputBuffer, tempData, numSamples);
    }
    
    // Process filter envelope
    filterEnvelope->process(tempData, numSamples);
    
    // Apply filter envelope to cutoff frequency
    float baseCutoff = filter->getCutoff();
    for (int i = 0; i < numSamples; ++i)
    {
        // Scale filter cutoff based on envelope
        float envelopeAmount = tempData[i];
        float cutoffMod = baseCutoff * (1.0f + filterEnvelopeAmount * envelopeAmount);
        filter->setCutoff(cutoffMod);
        
        // Apply filter to the sample
        outputBuffer[i] = filter->processSample(outputBuffer[i]);
    }
    
    // Reset filter cutoff
    filter->setCutoff(baseCutoff);
    
    // Apply amplitude envelope
    ampEnvelope->process(outputBuffer, numSamples);
    
    // Apply velocity sensitivity
    if (velocitySensitivity > 0.0f)
    {
        // Calculate velocity amount (mix of velocity sensitivity and full volume)
        float velocityAmount = velocitySensitivity * currentVelocity + (1.0f - velocitySensitivity);
        juce::FloatVectorOperations::multiply(outputBuffer, velocityAmount, numSamples);
    }
    
    // Check if voice is still active after processing
    if (!ampEnvelope->isActive())
    {
        active = false;
        currentNote = -1;
    }
}

void SynthVoice::setOscillatorWaveform(int oscillatorIndex, WaveformType type)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < static_cast<int>(oscillators.size()))
    {
        oscillators[oscillatorIndex]->setWaveform(type);
    }
}

void SynthVoice::setOscillatorDetune(int oscillatorIndex, float cents)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < static_cast<int>(oscillators.size()))
    {
        oscillatorDetuneCents[oscillatorIndex] = cents;
        
        // If the voice is active, update the frequency
        if (active && currentNote >= 0)
        {
            oscillators[oscillatorIndex]->setFrequency(midiNoteToFrequency(currentNote, cents));
        }
    }
}

void SynthVoice::setOscillatorLevel(int oscillatorIndex, float level)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < static_cast<int>(oscillators.size()))
    {
        oscillatorLevels[oscillatorIndex] = juce::jlimit(0.0f, 1.0f, level);
    }
}

void SynthVoice::setFilterType(FilterType type)
{
    filter->setType(type);
}

void SynthVoice::setFilterCutoff(float frequencyHz)
{
    filter->setCutoff(frequencyHz);
}

void SynthVoice::setFilterResonance(float amount)
{
    filter->setResonance(amount);
}

void SynthVoice::setEnvelopeParameters(float attackMs, float decayMs, float sustainLevel, float releaseMs)
{
    ampEnvelope->setAttackTime(attackMs);
    ampEnvelope->setDecayTime(decayMs);
    ampEnvelope->setSustainLevel(sustainLevel);
    ampEnvelope->setReleaseTime(releaseMs);
}

void SynthVoice::setVelocitySensitivity(float sensitivity)
{
    velocitySensitivity = juce::jlimit(0.0f, 1.0f, sensitivity);
}

void SynthVoice::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    
    // Prepare all components with the new sample rate
    for (auto& osc : oscillators)
    {
        osc->prepare(sampleRate);
    }
    
    ampEnvelope->prepare(sampleRate);
    filterEnvelope->prepare(sampleRate);
    filter->prepare(sampleRate);
}

float SynthVoice::midiNoteToFrequency(int midiNote, float cents) const
{
    // Convert MIDI note to frequency using standard formula: freq = 440 * 2^((note - 69) / 12)
    float note = static_cast<float>(midiNote) + cents / 100.0f;
    return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f);
}

//==============================================================================
// SynthModule Implementation
//==============================================================================

SynthModule::SynthModule(int numVoices)
    : currentSampleRate(44100.0)
{
    // Create the requested number of voices
    voices.reserve(numVoices);
    for (int i = 0; i < numVoices; ++i)
    {
        voices.push_back(std::make_unique<SynthVoice>());
    }
}

SynthModule::~SynthModule()
{
}

void SynthModule::processBlock(const juce::MidiBuffer& midiMessages, float* outputBuffer, int numSamples)
{
    // Clear the output buffer
    std::fill(outputBuffer, outputBuffer + numSamples, 0.0f);
    
    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        const int samplePosition = metadata.samplePosition;
        
        if (message.isNoteOn())
        {
            // Handle note on
            const int midiNote = message.getNoteNumber();
            const float velocity = message.getVelocity() / 127.0f;
            
            // Find a free voice or steal one
            SynthVoice* voice = findFreeVoice(midiNote, velocity);
            if (voice != nullptr)
            {
                voice->noteOn(midiNote, velocity);
            }
        }
        else if (message.isNoteOff())
        {
            // Handle note off
            const int midiNote = message.getNoteNumber();
            
            // Find any voices playing this note
            for (auto& voice : voices)
            {
                if (voice->getCurrentNote() == midiNote)
                {
                    voice->noteOff(true);
                }
            }
        }
        else if (message.isAllNotesOff())
        {
            // Turn off all voices
            for (auto& voice : voices)
            {
                voice->noteOff(true);
            }
        }
    }
    
    // Render audio for all active voices
    for (auto& voice : voices)
    {
        if (voice->isActive())
        {
            voice->renderNextBlock(outputBuffer, numSamples);
        }
    }
}

void SynthModule::processStereoBlock(const juce::MidiBuffer& midiMessages, float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Clear the output buffers
    std::fill(leftBuffer, leftBuffer + numSamples, 0.0f);
    
    // Process mono first
    processBlock(midiMessages, leftBuffer, numSamples);
    
    // Copy to right channel
    std::copy(leftBuffer, leftBuffer + numSamples, rightBuffer);
}

void SynthModule::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    
    // Prepare all voices
    for (auto& voice : voices)
    {
        voice->prepare(sampleRate);
    }
}

void SynthModule::setOscillatorWaveform(int oscillatorIndex, WaveformType type)
{
    for (auto& voice : voices)
    {
        voice->setOscillatorWaveform(oscillatorIndex, type);
    }
}

void SynthModule::setOscillatorDetune(int oscillatorIndex, float cents)
{
    for (auto& voice : voices)
    {
        voice->setOscillatorDetune(oscillatorIndex, cents);
    }
}

void SynthModule::setOscillatorLevel(int oscillatorIndex, float level)
{
    for (auto& voice : voices)
    {
        voice->setOscillatorLevel(oscillatorIndex, level);
    }
}

void SynthModule::setFilterType(FilterType type)
{
    for (auto& voice : voices)
    {
        voice->setFilterType(type);
    }
}

void SynthModule::setFilterCutoff(float frequencyHz)
{
    for (auto& voice : voices)
    {
        voice->setFilterCutoff(frequencyHz);
    }
}

void SynthModule::setFilterResonance(float amount)
{
    for (auto& voice : voices)
    {
        voice->setFilterResonance(amount);
    }
}

void SynthModule::setEnvelopeParameters(float attackMs, float decayMs, float sustainLevel, float releaseMs)
{
    for (auto& voice : voices)
    {
        voice->setEnvelopeParameters(attackMs, decayMs, sustainLevel, releaseMs);
    }
}

void SynthModule::setVelocitySensitivity(float sensitivity)
{
    for (auto& voice : voices)
    {
        voice->setVelocitySensitivity(sensitivity);
    }
}

SynthVoice* SynthModule::findFreeVoice(int midiNoteNumber, float velocity) const
{
    // First, check if the note is already playing (voice stealing for legato)
    for (auto& voice : voices)
    {
        if (voice->getCurrentNote() == midiNoteNumber)
        {
            return voice.get();
        }
    }
    
    // Then look for an inactive voice
    for (auto& voice : voices)
    {
        if (!voice->isActive())
        {
            return voice.get();
        }
    }
    
    // If all voices are active, steal the oldest one
    // In a more sophisticated implementation, we would use a more
    // intelligent voice stealing algorithm based on note priority, etc.
    return voices[0].get();
}

} // namespace UndergroundBeats
