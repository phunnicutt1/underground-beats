/*
 * Underground Beats
 * MixerChannel.cpp
 * 
 * Implementation of mixer channel component
 */

#include "MixerChannel.h"

namespace UndergroundBeats {

MixerChannel::MixerChannel(const juce::String& name)
    : channelName(name)
{
    createControls();
}

MixerChannel::~MixerChannel()
{
}

void MixerChannel::setChannelName(const juce::String& name)
{
    channelName = name;
    nameLabel.setText(name, juce::dontSendNotification);
}

juce::String MixerChannel::getChannelName() const
{
    return channelName;
}

void MixerChannel::setLevel(float level, juce::NotificationType notification)
{
    level = juce::jlimit(0.0f, 1.0f, level);
    levelSlider.setValue(level, notification);
}

float MixerChannel::getLevel() const
{
    return static_cast<float>(levelSlider.getValue());
}

void MixerChannel::setPan(float pan, juce::NotificationType notification)
{
    pan = juce::jlimit(-1.0f, 1.0f, pan);
    panSlider.setValue(pan, notification);
}

float MixerChannel::getPan() const
{
    return static_cast<float>(panSlider.getValue());
}

void MixerChannel::setMute(bool mute, juce::NotificationType notification)
{
    muteButton.setToggleState(mute, notification);
}

bool MixerChannel::isMuted() const
{
    return muteButton.getToggleState();
}

void MixerChannel::setSolo(bool solo, juce::NotificationType notification)
{
    soloButton.setToggleState(solo, notification);
}

bool MixerChannel::isSoloed() const
{
    return soloButton.getToggleState();
}

void MixerChannel::setSendLevel(int sendIndex, float level, juce::NotificationType notification)
{
    if (sendIndex >= 0 && sendIndex < sends.size())
    {
        level = juce::jlimit(0.0f, 1.0f, level);
        sends[sendIndex].slider.setValue(level, notification);
    }
}

float MixerChannel::getSendLevel(int sendIndex) const
{
    if (sendIndex >= 0 && sendIndex < sends.size())
    {
        return static_cast<float>(sends[sendIndex].slider.getValue());
    }
    
    return 0.0f;
}

void MixerChannel::setLevelChangeCallback(std::function<void(float)> callback)
{
    levelChangeCallback = callback;
}

void MixerChannel::setPanChangeCallback(std::function<void(float)> callback)
{
    panChangeCallback = callback;
}

void MixerChannel::setMuteChangeCallback(std::function<void(bool)> callback)
{
    muteChangeCallback = callback;
}

void MixerChannel::setSoloChangeCallback(std::function<void(bool)> callback)
{
    soloChangeCallback = callback;
}

void MixerChannel::setSendLevelChangeCallback(int sendIndex, std::function<void(float)> callback)
{
    if (sendIndex >= 0 && sendIndex < sends.size())
    {
        sends[sendIndex].callback = callback;
    }
}

void MixerChannel::setNumSends(int numSends)
{
    numSends = juce::jmax(0, numSends);
    
    if (numSends == sends.size())
    {
        return;
    }
    
    if (numSends > sends.size())
    {
        // Add more sends
        while (sends.size() < numSends)
        {
            SendControl send;
            send.slider.setSliderStyle(juce::Slider::LinearVertical);
            send.slider.setRange(0.0, 1.0);
            send.slider.setValue(0.0);
            send.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
            send.slider.addListener(this);
            
            send.label.setText("Send " + juce::String(sends.size() + 1), juce::dontSendNotification);
            send.label.setJustificationType(juce::Justification::centred);
            
            addAndMakeVisible(send.slider);
            addAndMakeVisible(send.label);
            
            sends.push_back(send);
        }
    }
    else
    {
        // Remove excess sends
        while (sends.size() > numSends)
        {
            removeChildComponent(&sends.back().slider);
            removeChildComponent(&sends.back().label);
            sends.pop_back();
        }
    }
    
    updateLayout();
}

int MixerChannel::getNumSends() const
{
    return static_cast<int>(sends.size());
}

void MixerChannel::resized()
{
    updateLayout();
}

void MixerChannel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey.darker(0.2f));
    
    // Draw border
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
}

void MixerChannel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        if (levelChangeCallback)
        {
            levelChangeCallback(static_cast<float>(levelSlider.getValue()));
        }
    }
    else if (slider == &panSlider)
    {
        if (panChangeCallback)
        {
            panChangeCallback(static_cast<float>(panSlider.getValue()));
        }
    }
    else
    {
        // Check if it's a send slider
        for (size_t i = 0; i < sends.size(); ++i)
        {
            if (slider == &sends[i].slider)
            {
                if (sends[i].callback)
                {
                    sends[i].callback(static_cast<float>(slider->getValue()));
                }
                break;
            }
        }
    }
}

void MixerChannel::buttonClicked(juce::Button* button)
{
    if (button == &muteButton)
    {
        if (muteChangeCallback)
        {
            muteChangeCallback(muteButton.getToggleState());
        }
    }
    else if (button == &soloButton)
    {
        if (soloChangeCallback)
        {
            soloChangeCallback(soloButton.getToggleState());
        }
    }
}

void MixerChannel::createControls()
{
    // Channel name label
    nameLabel.setText(channelName, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(nameLabel);
    
    // Level slider
    levelSlider.setSliderStyle(juce::Slider::LinearVertical);
    levelSlider.setRange(0.0, 1.0);
    levelSlider.setValue(0.75);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    levelSlider.addListener(this);
    addAndMakeVisible(levelSlider);
    
    levelLabel.setText("Level", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(levelLabel);
    
    // Pan slider
    panSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setRange(-1.0, 1.0);
    panSlider.setValue(0.0);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    panSlider.addListener(this);
    addAndMakeVisible(panSlider);
    
    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);
    
    // Mute/solo buttons
    muteButton.setButtonText("M");
    muteButton.setClickingTogglesState(true);
    muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    muteButton.addListener(this);
    addAndMakeVisible(muteButton);
    
    soloButton.setButtonText("S");
    soloButton.setClickingTogglesState(true);
    soloButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::yellow);
    soloButton.addListener(this);
    addAndMakeVisible(soloButton);
    
    // Start with no sends
    sends.clear();
}

void MixerChannel::updateLayout()
{
    int width = getWidth();
    int height = getHeight();
    int margin = 5;
    int buttonWidth = 30;
    int buttonHeight = 20;
    int labelHeight = 20;
    
    // Channel name at the top
    nameLabel.setBounds(margin, margin, width - margin * 2, labelHeight);
    
    // Mute/solo buttons below name
    int buttonY = margin + labelHeight + margin;
    muteButton.setBounds(margin, buttonY, buttonWidth, buttonHeight);
    soloButton.setBounds(width - margin - buttonWidth, buttonY, buttonWidth, buttonHeight);
    
    // Pan control below buttons
    int panY = buttonY + buttonHeight + margin;
    int panHeight = 60;
    panSlider.setBounds(margin, panY, width - margin * 2, panHeight);
    panLabel.setBounds(margin, panY + panHeight, width - margin * 2, labelHeight);
    
    // Level slider at the bottom
    int levelY = height - margin - 100 - labelHeight;
    levelSlider.setBounds(margin, levelY, width - margin * 2, 100);
    levelLabel.setBounds(margin, height - margin - labelHeight, width - margin * 2, labelHeight);
    
    // Send controls in the middle
    if (!sends.empty())
    {
        int sendY = panY + panHeight + labelHeight + margin;
        int sendHeight = levelY - sendY - margin;
        int sendWidth = (width - margin * 2) / static_cast<int>(sends.size());
        
        for (size_t i = 0; i < sends.size(); ++i)
        {
            int x = margin + static_cast<int>(i) * sendWidth;
            sends[i].slider.setBounds(x, sendY, sendWidth, sendHeight - labelHeight);
            sends[i].label.setBounds(x, sendY + sendHeight - labelHeight, sendWidth, labelHeight);
        }
    }
}

} // namespace UndergroundBeats
