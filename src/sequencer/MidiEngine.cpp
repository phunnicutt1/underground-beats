/*
 * Underground Beats
 * MidiEngine.cpp
 * 
 * Implementation of MIDI input/output handling
 */

#include "MidiEngine.h"

namespace UndergroundBeats {

MidiEngine::MidiEngine()
{
    // Start a timer to handle MIDI processing
    startTimer(10); // 10ms timer interval for low latency
}

MidiEngine::~MidiEngine()
{
    stopTimer();
    
    // Clean up MIDI devices
    midiInput.reset();
    midiOutput.reset();
}

bool MidiEngine::initialize()
{
    // Refresh the list of available devices
    refreshDeviceLists();
    
    return true;
}

std::vector<std::string> MidiEngine::getAvailableInputDevices() const
{
    std::vector<std::string> devices;
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    
    for (auto& device : midiInputs)
    {
        devices.push_back(device.name.toStdString());
    }
    
    return devices;
}

std::vector<std::string> MidiEngine::getAvailableOutputDevices() const
{
    std::vector<std::string> devices;
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    
    for (auto& device : midiOutputs)
    {
        devices.push_back(device.name.toStdString());
    }
    
    return devices;
}

bool MidiEngine::setInputDevice(const std::string& deviceName)
{
    // Close any existing device
    midiInput.reset();
    
    // Find the device with the given name
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    
    for (auto& device : midiInputs)
    {
        if (device.name.toStdString() == deviceName)
        {
            // Open the device
            midiInput = juce::MidiInput::openDevice(device.identifier, this);
            
            if (midiInput != nullptr)
            {
                // Start the device
                midiInput->start();
                currentInputDeviceName = deviceName;
                return true;
            }
        }
    }
    
    return false;
}

bool MidiEngine::setOutputDevice(const std::string& deviceName)
{
    // Close any existing device
    midiOutput.reset();
    
    // Find the device with the given name
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    
    for (auto& device : midiOutputs)
    {
        if (device.name.toStdString() == deviceName)
        {
            // Open the device
            midiOutput = juce::MidiOutput::openDevice(device.identifier);
            
            if (midiOutput != nullptr)
            {
                currentOutputDeviceName = deviceName;
                return true;
            }
        }
    }
    
    return false;
}

std::string MidiEngine::getInputDeviceName() const
{
    return currentInputDeviceName;
}

std::string MidiEngine::getOutputDeviceName() const
{
    return currentOutputDeviceName;
}

void MidiEngine::sendMessageNow(const juce::MidiMessage& message)
{
    if (midiOutput != nullptr)
    {
        midiOutput->sendMessageNow(message);
    }
}

void MidiEngine::processMidiBuffer(const juce::MidiBuffer& midiBuffer)
{
    // Add to outgoing messages
    outgoingMessages.addEvents(midiBuffer, 0, -1, 0);
}

void MidiEngine::setMidiInputCallback(std::function<void(const juce::MidiMessage&)> callback)
{
    juce::ScopedLock lock(midiCallbackLock);
    midiInputCallback = callback;
}

std::unique_ptr<juce::XmlElement> MidiEngine::createStateXml() const
{
    auto xml = std::make_unique<juce::XmlElement>("MidiEngine");
    
    // Add MIDI device information
    xml->setAttribute("inputDevice", currentInputDeviceName);
    xml->setAttribute("outputDevice", currentOutputDeviceName);
    
    return xml;
}

bool MidiEngine::restoreStateFromXml(const juce::XmlElement* xml)
{
    if (xml == nullptr || xml->getTagName() != "MidiEngine")
    {
        return false;
    }
    
    // Restore MIDI devices
    if (xml->hasAttribute("inputDevice"))
    {
        std::string inputDevice = xml->getStringAttribute("inputDevice").toStdString();
        setInputDevice(inputDevice);
    }
    
    if (xml->hasAttribute("outputDevice"))
    {
        std::string outputDevice = xml->getStringAttribute("outputDevice").toStdString();
        setOutputDevice(outputDevice);
    }
    
    return true;
}

void MidiEngine::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    // Add the message to the incoming queue
    juce::MidiBuffer buffer;
    buffer.addEvent(message, 0);
    
    {
        juce::ScopedLock lock(midiCallbackLock);
        incomingMessages.addEvents(buffer, 0, -1, 0);
    }
    
    // Call the callback if set
    if (midiInputCallback)
    {
        midiInputCallback(message);
    }
}

void MidiEngine::timerCallback()
{
    // Send any outgoing messages
    if (midiOutput != nullptr && !outgoingMessages.isEmpty())
    {
        for (const auto metadata : outgoingMessages)
        {
            const auto message = metadata.getMessage();
            midiOutput->sendMessageNow(message);
        }
        
        outgoingMessages.clear();
    }
    
    // Clear incoming messages buffer as it's been processed
    {
        juce::ScopedLock lock(midiCallbackLock);
        incomingMessages.clear();
    }
}

void MidiEngine::refreshDeviceLists()
{
    // Nothing to do here - the lists are always refreshed when requested
}

} // namespace UndergroundBeats
