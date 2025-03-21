/*
 * Underground Beats
 * OscillatorPanel.h
 * 
 * UI panel for oscillator controls
 */

#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Oscillator.h"
#include <functional>

namespace UndergroundBeats {

/**
 * @class OscillatorPanel
 * @brief UI panel for oscillator controls
 * 
 * The OscillatorPanel class provides a user interface for controlling
 * the parameters of an oscillator, including waveform type, frequency,
 * and other settings.
 */
class OscillatorPanel : public juce::Component,
                        public juce::ComboBox::Listener,
                        public juce::Slider::Listener {
public:
    OscillatorPanel(const juce::String& name = "Oscillator");
    ~OscillatorPanel() override;
    
    /**
     * @brief Set the oscillator waveform type
     * 
     * @param type The waveform type to set
     * @param notification Whether to send a notification
     */
    void setWaveform(WaveformType type, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current waveform type
     * 
     * @return The current waveform type
     */
    WaveformType getWaveform() const;
    
    /**
     * @brief Set the oscillator detune amount
     * 
     * @param cents Detune amount in cents
     * @param notification Whether to send a notification
     */
    void setDetune(float cents, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current detune amount
     * 
     * @return The current detune amount in cents
     */
    float getDetune() const;
    
    /**
     * @brief Set the oscillator level
     * 
     * @param level Level (0.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setLevel(float level, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current oscillator level
     * 
     * @return The current level (0.0 to 1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Set a callback for when the waveform changes
     * 
     * @param callback The callback function
     */
    void setWaveformChangeCallback(std::function<void(WaveformType)> callback);
    
    /**
     * @brief Set a callback for when the detune changes
     * 
     * @param callback The callback function
     */
    void setDetuneChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the level changes
     * 
     * @param callback The callback function
     */
    void setLevelChangeCallback(std::function<void(float)> callback);
    
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
     * @brief ComboBox changed callback
     * 
     * @param comboBox The combo box that changed
     */
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    /**
     * @brief Slider value changed callback
     * 
     * @param slider The slider that changed
     */
    void sliderValueChanged(juce::Slider* slider) override;
    
private:
    juce::String oscillatorName;
    
    // Waveform selector
    juce::ComboBox waveformCombo;
    juce::Label waveformLabel;
    
    // Detune control
    juce::Slider detuneSlider;
    juce::Label detuneLabel;
    
    // Level control
    juce::Slider levelSlider;
    juce::Label levelLabel;
    
    // Current values
    WaveformType currentWaveform;
    float currentDetune;
    float currentLevel;
    
    // Callbacks
    std::function<void(WaveformType)> waveformChangeCallback;
    std::function<void(float)> detuneChangeCallback;
    std::function<void(float)> levelChangeCallback;
    
    // Create and set up the controls
    void createControls();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};

} // namespace UndergroundBeats
