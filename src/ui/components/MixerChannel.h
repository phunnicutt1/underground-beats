/*
 * Underground Beats
 * MixerChannel.h
 * 
 * Mixer channel component for level control and routing
 */

#pragma once

#include <JuceHeader.h>
#include <functional>

namespace UndergroundBeats {

/**
 * @class MixerChannel
 * @brief Mixer channel component for level control and routing
 * 
 * The MixerChannel class represents a single channel in the mixer,
 * with controls for level, pan, mute, solo, and effect sends.
 */
class MixerChannel : public juce::Component,
                     public juce::Slider::Listener,
                     public juce::Button::Listener {
public:
    MixerChannel(const juce::String& name = "Channel");
    ~MixerChannel() override;
    
    /**
     * @brief Set the channel name
     * 
     * @param name The channel name
     */
    void setChannelName(const juce::String& name);
    
    /**
     * @brief Get the channel name
     * 
     * @return The channel name
     */
    juce::String getChannelName() const;
    
    /**
     * @brief Set the channel level
     * 
     * @param level The level (0.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setLevel(float level, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the channel level
     * 
     * @return The level (0.0 to 1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Set the channel pan position
     * 
     * @param pan The pan position (-1.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setPan(float pan, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the channel pan position
     * 
     * @return The pan position (-1.0 to 1.0)
     */
    float getPan() const;
    
    /**
     * @brief Set whether the channel is muted
     * 
     * @param mute Whether the channel is muted
     * @param notification Whether to send a notification
     */
    void setMute(bool mute, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Check if the channel is muted
     * 
     * @return true if the channel is muted
     */
    bool isMuted() const;
    
    /**
     * @brief Set whether the channel is soloed
     * 
     * @param solo Whether the channel is soloed
     * @param notification Whether to send a notification
     */
    void setSolo(bool solo, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Check if the channel is soloed
     * 
     * @return true if the channel is soloed
     */
    bool isSoloed() const;
    
    /**
     * @brief Set a send level
     * 
     * @param sendIndex The send index
     * @param level The send level (0.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setSendLevel(int sendIndex, float level, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get a send level
     * 
     * @param sendIndex The send index
     * @return The send level (0.0 to 1.0)
     */
    float getSendLevel(int sendIndex) const;
    
    /**
     * @brief Set a callback for when the level changes
     * 
     * @param callback The callback function
     */
    void setLevelChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the pan changes
     * 
     * @param callback The callback function
     */
    void setPanChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the mute state changes
     * 
     * @param callback The callback function
     */
    void setMuteChangeCallback(std::function<void(bool)> callback);
    
    /**
     * @brief Set a callback for when the solo state changes
     * 
     * @param callback The callback function
     */
    void setSoloChangeCallback(std::function<void(bool)> callback);
    
    /**
     * @brief Set a callback for when a send level changes
     * 
     * @param sendIndex The send index
     * @param callback The callback function
     */
    void setSendLevelChangeCallback(int sendIndex, std::function<void(float)> callback);
    
    /**
     * @brief Set the number of sends
     * 
     * @param numSends The number of sends
     */
    void setNumSends(int numSends);
    
    /**
     * @brief Get the number of sends
     * 
     * @return The number of sends
     */
    int getNumSends() const;
    
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
     * @brief Slider value changed callback
     * 
     * @param slider The slider that changed
     */
    void sliderValueChanged(juce::Slider* slider) override;
    
    /**
     * @brief Button clicked callback
     * 
     * @param button The button that was clicked
     */
    void buttonClicked(juce::Button* button) override;
    
private:
    juce::String channelName;
    
    // Level controls
    juce::Slider levelSlider;
    juce::Label levelLabel;
    
    // Pan control
    juce::Slider panSlider;
    juce::Label panLabel;
    
    // Mute/solo buttons
    juce::TextButton muteButton;
    juce::TextButton soloButton;
    
    // Channel name label
    juce::Label nameLabel;
    
    // Send controls
    struct SendControl {
        juce::Slider slider;
        juce::Label label;
        std::function<void(float)> callback;
    };
    
    std::vector<SendControl> sends;
    
    // Callbacks
    std::function<void(float)> levelChangeCallback;
    std::function<void(float)> panChangeCallback;
    std::function<void(bool)> muteChangeCallback;
    std::function<void(bool)> soloChangeCallback;
    
    // Create and set up the controls
    void createControls();
    
    // Update the layout based on the number of sends
    void updateLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerChannel)
};

} // namespace UndergroundBeats
