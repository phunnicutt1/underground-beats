/*
 * Underground Beats
 * MixerView.h
 * 
 * Complete mixer view with multiple channels
 */

#pragma once

#include <JuceHeader.h>
#include "../components/MixerChannel.h"
#include "../../audio-engine/AudioEngine.h"
#include <vector>
#include <memory>

namespace UndergroundBeats {

/**
 * @class MixerView
 * @brief Complete mixer view with multiple channels
 * 
 * The MixerView class provides a complete mixing interface with
 * multiple channels, master channel, and controls for routing.
 */
class MixerView : public juce::Component,
                 public juce::Button::Listener {
public:
    MixerView();
    ~MixerView() override;
    
    /**
     * @brief Set the audio engine for level monitoring
     * 
     * @param engine The audio engine
     */
    void setAudioEngine(AudioEngine* engine);
    
    /**
     * @brief Set the number of input channels
     * 
     * @param numChannels The number of channels
     */
    void setNumInputChannels(int numChannels);
    
    /**
     * @brief Set the number of effect send channels
     * 
     * @param numSends The number of effect sends
     */
    void setNumEffectSends(int numSends);
    
    /**
     * @brief Component resized callback
     */
    void resized() override;
    
    /**
     * @brief Component paint callback
     * 
     * @param g Graphics context to paint to
     */
    void paint(juce::Graphics& g) override;
    
    /**
     * @brief Button clicked callback
     * 
     * @param button The button that was clicked
     */
    void buttonClicked(juce::Button* button) override;
    
private:
    AudioEngine* audioEngine;
    
    // Scrollable view for channels
    juce::Viewport channelsViewport;
    juce::Component channelsContainer;
    
    // Mixer channels
    std::vector<std::unique_ptr<MixerChannel>> inputChannels;
    std::vector<std::unique_ptr<MixerChannel>> effectReturnChannels;
    std::unique_ptr<MixerChannel> masterChannel;
    
    // Control buttons
    juce::TextButton addChannelButton;
    juce::TextButton removeChannelButton;
    juce::TextButton addSendButton;
    juce::TextButton removeSendButton;
    
    // Number of effect sends
    int numEffectSends;
    
    // Create default channels
    void createDefaultChannels();
    
    // Update the channel layout
    void updateChannelLayout();
    
    // Handler for level changes
    void handleLevelChange(int channelIndex, float level);
    
    // Handler for pan changes
    void handlePanChange(int channelIndex, float pan);
    
    // Handler for mute changes
    void handleMuteChange(int channelIndex, bool muted);
    
    // Handler for solo changes
    void handleSoloChange(int channelIndex, bool soloed);
    
    // Handler for send level changes
    void handleSendLevelChange(int channelIndex, int sendIndex, float level);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerView)
};

} // namespace UndergroundBeats
