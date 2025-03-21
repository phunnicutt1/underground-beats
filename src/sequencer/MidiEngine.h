/*
 * Underground Beats
 * MidiEngine.h
 * 
 * Handles MIDI input/output and device management
 */

#pragma once

#include <JuceHeader.h>
#include <string>
#include <vector>
#include <functional>

namespace UndergroundBeats {

/**
 * @class MidiEngine
 * @brief Handles MIDI input/output and device management
 * 
 * The MidiEngine class manages MIDI devices, handling input and output
 * of MIDI messages, device selection, and MIDI routing.
 */
class MidiEngine : private juce::MidiInputCallback,
                  private juce::Timer {
public:
    MidiEngine();
    ~MidiEngine();
    
    /**
     * @brief Initialize the MIDI engine
     * 
     * @return true if initialization was successful
     */
    bool initialize();
    
    /**
     * @brief Get a list of available MIDI input devices
     * 
     * @return A vector of device names
     */
    std::vector<std::string> getAvailableInputDevices() const;
    
    /**
     * @brief Get a list of available MIDI output devices
     * 
     * @return A vector of device names
     */
    std::vector<std::string> getAvailableOutputDevices() const;
    
    /**
     * @brief Set the active MIDI input device
     * 
     * @param deviceName The name of the device to use
     * @return true if the device was set successfully
     */
    bool setInputDevice(const std::string& deviceName);
    
    /**
     * @brief Set the active MIDI output device
     * 
     * @param deviceName The name of the device to use
     * @return true if the device was set successfully
     */
    bool setOutputDevice(const std::string& deviceName);
    
    /**
     * @brief Get the name of the current input device
     * 
     * @return The device name, or empty string if none
     */
    std::string getInputDeviceName() const;
    
    /**
     * @brief Get the name of the current output device
     * 
     * @return The device name, or empty string if none
     */
    std::string getOutputDeviceName() const;
    
    /**
     * @brief Send a MIDI message to the output device
     * 
     * @param message The MIDI message to send
     */
    void sendMessageNow(const juce::MidiMessage& message);
    
    /**
     * @brief Process MIDI messages
     * 
     * @param midiBuffer The buffer of MIDI messages to process
     */
    void processMidiBuffer(const juce::MidiBuffer& midiBuffer);
    
    /**
     * @brief Set a callback function for incoming MIDI messages
     * 
     * @param callback Function to call when MIDI messages are received
     */
    void setMidiInputCallback(std::function<void(const juce::MidiMessage&)> callback);
    
    /**
     * @brief Create an XML element containing the MIDI engine's state
     * 
     * @return XML element containing MIDI engine state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore MIDI engine state from an XML element
     * 
     * @param xml XML element containing MIDI engine state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
private:
    // MidiInputCallback implementation
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    
    // Timer implementation
    void timerCallback() override;
    
    // Refresh the list of available devices
    void refreshDeviceLists();
    
    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
    
    std::string currentInputDeviceName;
    std::string currentOutputDeviceName;
    
    juce::CriticalSection midiCallbackLock;
    std::function<void(const juce::MidiMessage&)> midiInputCallback;
    
    juce::MidiBuffer incomingMessages;
    juce::MidiBuffer outgoingMessages;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEngine)
};

} // namespace UndergroundBeats
