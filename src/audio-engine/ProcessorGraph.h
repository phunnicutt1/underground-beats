/*
 * Underground Beats
 * ProcessorGraph.h
 * 
 * Manages the audio processing graph and node routing
 */

#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <string>

namespace UndergroundBeats {

/**
 * @class ProcessorGraph
 * @brief Manages the audio processing graph and node routing
 * 
 * This class extends JUCE's AudioProcessorGraph to provide a higher-level
 * interface for managing audio processors, connections, and signal flow.
 * It handles the creation, connection, and removal of audio processing nodes.
 */
class ProcessorGraph : public juce::AudioProcessorGraph {
public:
    ProcessorGraph();
    ~ProcessorGraph() override;
    
    /**
     * @brief Add a processor to the graph with a unique ID
     * 
     * @param processor The audio processor to add
     * @param nodeID A unique string ID for the node
     * @return The node ID if successful, or an empty string if failed
     */
    juce::AudioProcessorGraph::NodeID addProcessor(std::unique_ptr<juce::AudioProcessor> processor, const std::string& nodeID);
    
    /**
     * @brief Connect two nodes in the graph
     * 
     * @param sourceNodeID The source node ID
     * @param sourceChannelIndex The source channel index
     * @param destNodeID The destination node ID
     * @param destChannelIndex The destination channel index
     * @return true if connection was made, false otherwise
     */
    bool connectNodes(const std::string& sourceNodeID, int sourceChannelIndex, 
                      const std::string& destNodeID, int destChannelIndex);
    
    /**
     * @brief Disconnect a specific connection between nodes
     * 
     * @param sourceNodeID The source node ID
     * @param sourceChannelIndex The source channel index
     * @param destNodeID The destination node ID
     * @param destChannelIndex The destination channel index
     * @return true if disconnection was made, false otherwise
     */
    bool disconnectNodes(const std::string& sourceNodeID, int sourceChannelIndex, 
                         const std::string& destNodeID, int destChannelIndex);
    
    /**
     * @brief Remove a processor node from the graph
     * 
     * @param nodeID The ID of the node to remove
     * @return true if the node was removed, false otherwise
     */
    bool removeProcessor(const std::string& nodeID);
    
    /**
     * @brief Get a node by its string ID
     * 
     * @param nodeID The string ID of the node
     * @return The node if found, nullptr otherwise
     */
    juce::AudioProcessorGraph::Node::Ptr getNodeForID(const std::string& nodeID);
    
    /**
     * @brief Initialize the standard nodes (input, output, etc.)
     */
    void initializeDefaultNodes();
    
private:
    std::unordered_map<std::string, juce::AudioProcessorGraph::NodeID> nodeMap;
    
    // Default node IDs
    juce::AudioProcessorGraph::NodeID audioInputNodeID;
    juce::AudioProcessorGraph::NodeID audioOutputNodeID;
    juce::AudioProcessorGraph::NodeID midiInputNodeID;
    juce::AudioProcessorGraph::NodeID midiOutputNodeID;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorGraph)
};

} // namespace UndergroundBeats
