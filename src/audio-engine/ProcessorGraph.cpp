/*
 * Underground Beats
 * ProcessorGraph.cpp
 * 
 * Implementation of the audio processing graph and node routing
 */

#include "ProcessorGraph.h"

namespace UndergroundBeats {

ProcessorGraph::ProcessorGraph()
{
    initializeDefaultNodes();
}

ProcessorGraph::~ProcessorGraph()
{
    // Clear all connections and nodes
    clear();
}

void ProcessorGraph::initializeDefaultNodes()
{
    // Create the default nodes (input, output, midi input, midi output)
    audioInputNodeID = addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
                              juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode))->nodeID;
    
    audioOutputNodeID = addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
                               juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode))->nodeID;
    
    midiInputNodeID = addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
                             juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode))->nodeID;
    
    midiOutputNodeID = addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
                              juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode))->nodeID;
    
    // Add to the node map for easier lookup
    nodeMap["audio_input"] = audioInputNodeID;
    nodeMap["audio_output"] = audioOutputNodeID;
    nodeMap["midi_input"] = midiInputNodeID;
    nodeMap["midi_output"] = midiOutputNodeID;
}

juce::AudioProcessorGraph::NodeID ProcessorGraph::addProcessor(std::unique_ptr<juce::AudioProcessor> processor, 
                                                             const std::string& nodeID)
{
    // Check if the nodeID already exists
    if (nodeMap.find(nodeID) != nodeMap.end())
    {
        // Node ID already exists, return empty ID
        return juce::AudioProcessorGraph::NodeID();
    }
    
    // Add the processor to the graph
    auto node = addNode(std::move(processor));
    
    if (node != nullptr)
    {
        // Store the mapping and return the ID
        nodeMap[nodeID] = node->nodeID;
        return node->nodeID;
    }
    
    // Failed to add node
    return juce::AudioProcessorGraph::NodeID();
}

bool ProcessorGraph::connectNodes(const std::string& sourceNodeID, int sourceChannelIndex, 
                                const std::string& destNodeID, int destChannelIndex)
{
    // Check if both nodes exist
    if (nodeMap.find(sourceNodeID) == nodeMap.end() || nodeMap.find(destNodeID) == nodeMap.end())
    {
        return false;
    }
    
    // Get the node IDs
    auto sourceID = nodeMap[sourceNodeID];
    auto destID = nodeMap[destNodeID];
    
    // Create the connection
    auto result = addConnection({ { sourceID, sourceChannelIndex }, { destID, destChannelIndex } });
    
    return result;
}

bool ProcessorGraph::disconnectNodes(const std::string& sourceNodeID, int sourceChannelIndex, 
                                   const std::string& destNodeID, int destChannelIndex)
{
    // Check if both nodes exist
    if (nodeMap.find(sourceNodeID) == nodeMap.end() || nodeMap.find(destNodeID) == nodeMap.end())
    {
        return false;
    }
    
    // Get the node IDs
    auto sourceID = nodeMap[sourceNodeID];
    auto destID = nodeMap[destNodeID];
    
    // Remove the connection
    auto result = removeConnection({ { sourceID, sourceChannelIndex }, { destID, destChannelIndex } });
    
    return result;
}

bool ProcessorGraph::removeProcessor(const std::string& nodeID)
{
    // Check if the node exists
    if (nodeMap.find(nodeID) == nodeMap.end())
    {
        return false;
    }
    
    // Get the node ID
    auto id = nodeMap[nodeID];
    
    // Remove the node
    auto result = removeNode(id);
    
    if (result)
    {
        // Remove from the node map
        nodeMap.erase(nodeID);
    }
    
    return result;
}

juce::AudioProcessorGraph::Node::Ptr ProcessorGraph::getNodeForID(const std::string& nodeID)
{
    // Check if the node exists
    if (nodeMap.find(nodeID) == nodeMap.end())
    {
        return nullptr;
    }
    
    // Get the node ID
    auto id = nodeMap[nodeID];
    
    // Return the node
    return getNodeForId(id);
}

} // namespace UndergroundBeats
