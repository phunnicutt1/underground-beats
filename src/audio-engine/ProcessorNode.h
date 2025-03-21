#pragma once

#include <JuceHeader.h>

// Custom node ID class for graph nodes
class NodeID
{
public:
    explicit NodeID(uint32_t id) : nodeID(id) {}
    uint32_t get() const { return nodeID; }
    bool operator==(const NodeID& other) const { return nodeID == other.nodeID; }
    bool operator!=(const NodeID& other) const { return !(*this == other); }
private:
    uint32_t nodeID;
};

// Maximum number of parameters per node
constexpr int MAX_PARAMETERS = 128;

//==============================================================================
class ProcessorNode : public juce::AudioProcessor
{
public:
    ProcessorNode();
    ~ProcessorNode() override;
    
    // Parameter management
    void setParameter(int index, float newValue);
    float getParameter(int index) const;
    
    // AudioProcessor methods (minimal implementations for now)
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    // SIMD-optimized processing (will be implemented in future)
    virtual void processBlockSIMD(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    // Metadata methods
    const juce::String getName() const override { return "Processor Node"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    
    // Editor methods
    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    
    // Program and preset methods
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    
    // State methods
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

protected:
    // Parameter storage (atomic for thread safety)
    std::array<std::atomic<float>, MAX_PARAMETERS> parameters;
    
    // Processing buffer
    juce::AudioBuffer<float> processingBuffer;
    
    // Processing state
    double currentSampleRate = 44100.0;
    int currentBlockSize = 256;
    bool isPrepared = false;
};
