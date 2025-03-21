/*
 * Underground Beats
 * MixerView.cpp
 * 
 * Implementation of complete mixer view
 */

#include "MixerView.h"

namespace UndergroundBeats {

MixerView::MixerView()
    : audioEngine(nullptr)
    , numEffectSends(0)
{
    // Add the channels viewport
    addAndMakeVisible(channelsViewport);
    channelsViewport.setViewedComponent(&channelsContainer, false);
    channelsContainer.setInterceptsMouseClicks(false, true);
    
    // Create control buttons
    addChannelButton.setButtonText("Add Channel");
    addChannelButton.addListener(this);
    addAndMakeVisible(addChannelButton);
    
    removeChannelButton.setButtonText("Remove Channel");
    removeChannelButton.addListener(this);
    addAndMakeVisible(removeChannelButton);
    
    addSendButton.setButtonText("Add Send");
    addSendButton.addListener(this);
    addAndMakeVisible(addSendButton);
    
    removeSendButton.setButtonText("Remove Send");
    removeSendButton.addListener(this);
    addAndMakeVisible(removeSendButton);
    
    // Create default channels
    createDefaultChannels();
}

MixerView::~MixerView()
{
}

void MixerView::setAudioEngine(AudioEngine* engine)
{
    audioEngine = engine;
    
    // TODO: Connect to audio engine for level monitoring
}

void MixerView::setNumInputChannels(int numChannels)
{
    numChannels = juce::jmax(1, numChannels);
    
    // Adjust the number of input channels
    if (numChannels > inputChannels.size())
    {
        // Add more channels
        while (inputChannels.size() < numChannels)
        {
            auto channel = std::make_unique<MixerChannel>("Ch " + juce::String(inputChannels.size() + 1));
            channel->setNumSends(numEffectSends);
            
            // Set up callbacks
            int channelIndex = static_cast<int>(inputChannels.size());
            channel->setLevelChangeCallback([this, channelIndex](float level) {
                handleLevelChange(channelIndex, level);
            });
            channel->setPanChangeCallback([this, channelIndex](float pan) {
                handlePanChange(channelIndex, pan);
            });
            channel->setMuteChangeCallback([this, channelIndex](bool muted) {
                handleMuteChange(channelIndex, muted);
            });
            channel->setSoloChangeCallback([this, channelIndex](bool soloed) {
                handleSoloChange(channelIndex, soloed);
            });
            
            // Set up send callbacks
            for (int sendIndex = 0; sendIndex < numEffectSends; ++sendIndex)
            {
                int capturedSendIndex = sendIndex;
                channel->setSendLevelChangeCallback(sendIndex, [this, channelIndex, capturedSendIndex](float level) {
                    handleSendLevelChange(channelIndex, capturedSendIndex, level);
                });
            }
            
            channelsContainer.addAndMakeVisible(channel.get());
            inputChannels.push_back(std::move(channel));
        }
    }
    else if (numChannels < inputChannels.size())
    {
        // Remove excess channels
        while (inputChannels.size() > numChannels)
        {
            channelsContainer.removeChildComponent(inputChannels.back().get());
            inputChannels.pop_back();
        }
    }
    
    updateChannelLayout();
}

void MixerView::setNumEffectSends(int numSends)
{
    numSends = juce::jmax(0, numSends);
    
    if (numSends == numEffectSends)
    {
        return;
    }
    
    numEffectSends = numSends;
    
    // Update the number of sends on all channels
    for (auto& channel : inputChannels)
    {
        channel->setNumSends(numSends);
    }
    
    // Update the number of effect return channels
    if (numSends > effectReturnChannels.size())
    {
        // Add more effect return channels
        while (effectReturnChannels.size() < numSends)
        {
            auto channel = std::make_unique<MixerChannel>("FX " + juce::String(effectReturnChannels.size() + 1));
            
            // Set up callbacks
            int channelIndex = static_cast<int>(effectReturnChannels.size());
            channel->setLevelChangeCallback([this, channelIndex](float level) {
                // Handle effect return level change
            });
            channel->setPanChangeCallback([this, channelIndex](float pan) {
                // Handle effect return pan change
            });
            channel->setMuteChangeCallback([this, channelIndex](bool muted) {
                // Handle effect return mute change
            });
            channel->setSoloChangeCallback([this, channelIndex](bool soloed) {
                // Handle effect return solo change
            });
            
            channelsContainer.addAndMakeVisible(channel.get());
            effectReturnChannels.push_back(std::move(channel));
        }
    }
    else if (numSends < effectReturnChannels.size())
    {
        // Remove excess effect return channels
        while (effectReturnChannels.size() > numSends)
        {
            channelsContainer.removeChildComponent(effectReturnChannels.back().get());
            effectReturnChannels.pop_back();
        }
    }
    
    updateChannelLayout();
}

void MixerView::resized()
{
    // Layout components
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int buttonHeight = 30;
    int buttonWidth = 120;
    
    // Place control buttons at the bottom
    int buttonY = height - margin - buttonHeight;
    addChannelButton.setBounds(margin, buttonY, buttonWidth, buttonHeight);
    removeChannelButton.setBounds(margin + buttonWidth + margin, buttonY, buttonWidth, buttonHeight);
    addSendButton.setBounds(width - margin - buttonWidth * 2 - margin, buttonY, buttonWidth, buttonHeight);
    removeSendButton.setBounds(width - margin - buttonWidth, buttonY, buttonWidth, buttonHeight);
    
    // Channels viewport fills the rest
    channelsViewport.setBounds(0, 0, width, buttonY - margin);
    
    // Update channel layout
    updateChannelLayout();
}

void MixerView::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey);
}

void MixerView::buttonClicked(juce::Button* button)
{
    if (button == &addChannelButton)
    {
        setNumInputChannels(static_cast<int>(inputChannels.size()) + 1);
    }
    else if (button == &removeChannelButton)
    {
        setNumInputChannels(static_cast<int>(inputChannels.size()) - 1);
    }
    else if (button == &addSendButton)
    {
        setNumEffectSends(numEffectSends + 1);
    }
    else if (button == &removeSendButton)
    {
        setNumEffectSends(numEffectSends - 1);
    }
}

void MixerView::createDefaultChannels()
{
    // Create master channel
    masterChannel = std::make_unique<MixerChannel>("Master");
    masterChannel->setLevelChangeCallback([this](float level) {
        // Handle master level change
        if (audioEngine != nullptr)
        {
            // TODO: Set master level on audio engine
        }
    });
    channelsContainer.addAndMakeVisible(masterChannel.get());
    
    // Create some default input channels
    setNumInputChannels(4);
    
    // Create some default effect sends
    setNumEffectSends(2);
}

void MixerView::updateChannelLayout()
{
    int totalWidth = 0;
    int channelWidth = 120;
    int channelSpacing = 10;
    int margin = 10;
    int channelHeight = channelsViewport.getHeight() - margin * 2;
    
    // Calculate total width needed
    int totalChannels = static_cast<int>(inputChannels.size() + effectReturnChannels.size() + 1); // +1 for master
    totalWidth = totalChannels * channelWidth + (totalChannels - 1) * channelSpacing + margin * 2;
    
    // Resize the container component
    channelsContainer.setBounds(0, 0, totalWidth, channelsViewport.getHeight());
    
    // Position the input channels
    int x = margin;
    for (auto& channel : inputChannels)
    {
        channel->setBounds(x, margin, channelWidth, channelHeight);
        x += channelWidth + channelSpacing;
    }
    
    // Position the effect return channels
    for (auto& channel : effectReturnChannels)
    {
        channel->setBounds(x, margin, channelWidth, channelHeight);
        x += channelWidth + channelSpacing;
    }
    
    // Position the master channel at the end
    masterChannel->setBounds(x, margin, channelWidth, channelHeight);
    
    // Ensure the viewport position is valid
    channelsViewport.setViewPosition(channelsViewport.getViewPosition().x, 0);
}

void MixerView::handleLevelChange(int channelIndex, float level)
{
    // Apply the level change to the audio engine
    if (audioEngine != nullptr)
    {
        // TODO: Set channel level on audio engine
    }
}

void MixerView::handlePanChange(int channelIndex, float pan)
{
    // Apply the pan change to the audio engine
    if (audioEngine != nullptr)
    {
        // TODO: Set channel pan on audio engine
    }
}

void MixerView::handleMuteChange(int channelIndex, bool muted)
{
    // Apply the mute change to the audio engine
    if (audioEngine != nullptr)
    {
        // TODO: Set channel mute on audio engine
    }
}

void MixerView::handleSoloChange(int channelIndex, bool soloed)
{
    // Apply the solo change to the audio engine
    if (audioEngine != nullptr)
    {
        // TODO: Set channel solo on audio engine
    }
    
    // Update the mute state of other channels based on solo
    if (soloed)
    {
        // Find all soloed channels
        std::vector<int> soloedChannels;
        for (size_t i = 0; i < inputChannels.size(); ++i)
        {
            if (inputChannels[i]->isSoloed())
            {
                soloedChannels.push_back(static_cast<int>(i));
            }
        }
        
        // Mute all non-soloed channels
        for (size_t i = 0; i < inputChannels.size(); ++i)
        {
            bool shouldBeMuted = std::find(soloedChannels.begin(), soloedChannels.end(), static_cast<int>(i)) == soloedChannels.end();
            if (shouldBeMuted != inputChannels[i]->isMuted())
            {
                inputChannels[i]->setMute(shouldBeMuted, juce::dontSendNotification);
                handleMuteChange(static_cast<int>(i), shouldBeMuted);
            }
        }
    }
    else
    {
        // If no channels are soloed, unmute all channels
        bool anySoloed = false;
        for (auto& channel : inputChannels)
        {
            if (channel->isSoloed())
            {
                anySoloed = true;
                break;
            }
        }
        
        if (!anySoloed)
        {
            for (size_t i = 0; i < inputChannels.size(); ++i)
            {
                if (inputChannels[i]->isMuted())
                {
                    inputChannels[i]->setMute(false, juce::dontSendNotification);
                    handleMuteChange(static_cast<int>(i), false);
                }
            }
        }
    }
}

void MixerView::handleSendLevelChange(int channelIndex, int sendIndex, float level)
{
    // Apply the send level change to the audio engine
    if (audioEngine != nullptr)
    {
        // TODO: Set send level on audio engine
    }
}

} // namespace UndergroundBeats
