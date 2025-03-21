/*
 * Underground Beats
 * EnvelopePanel.h
 * 
 * UI panel for envelope controls
 */

#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Envelope.h"
#include <functional>

namespace UndergroundBeats {

/**
 * @class EnvelopePanel
 * @brief UI panel for envelope controls
 * 
 * The EnvelopePanel class provides a user interface for controlling
 * the parameters of an ADSR envelope, including attack, decay, sustain,
 * and release times.
 */
class EnvelopePanel : public juce::Component,
                     public juce::Slider::Listener {
public:
    EnvelopePanel(const juce::String& name = "Envelope");
    ~EnvelopePanel() override;
    
    /**
     * @brief Set the attack time
     * 
     * @param timeMs Attack time in milliseconds
     * @param notification Whether to send a notification
     */
    void setAttackTime(float timeMs, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current attack time
     * 
     * @return The current attack time in milliseconds
     */
    float getAttackTime() const;
    
    /**
     * @brief Set the decay time
     * 
     * @param timeMs Decay time in milliseconds
     * @param notification Whether to send a notification
     */
    void setDecayTime(float timeMs, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current decay time
     * 
     * @return The current decay time in milliseconds
     */
    float getDecayTime() const;
    
    /**
     * @brief Set the sustain level
     * 
     * @param level Sustain level (0.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setSustainLevel(float level, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current sustain level
     * 
     * @return The current sustain level (0.0 to 1.0)
     */
    float getSustainLevel() const;
    
    /**
     * @brief Set the release time
     * 
     * @param timeMs Release time in milliseconds
     * @param notification Whether to send a notification
     */
    void setReleaseTime(float timeMs, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current release time
     * 
     * @return The current release time in milliseconds
     */
    float getReleaseTime() const;
    
    /**
     * @brief Set a callback for when the attack time changes
     * 
     * @param callback The callback function
     */
    void setAttackTimeChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the decay time changes
     * 
     * @param callback The callback function
     */
    void setDecayTimeChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the sustain level changes
     * 
     * @param callback The callback function
     */
    void setSustainLevelChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the release time changes
     * 
     * @param callback The callback function
     */
    void setReleaseTimeChangeCallback(std::function<void(float)> callback);
    
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
    
private:
    juce::String envelopeName;
    
    // ADSR controls
    juce::Slider attackSlider;
    juce::Label attackLabel;
    
    juce::Slider decaySlider;
    juce::Label decayLabel;
    
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    
    // Current values
    float currentAttackTime;
    float currentDecayTime;
    float currentSustainLevel;
    float currentReleaseTime;
    
    // Callbacks
    std::function<void(float)> attackTimeChangeCallback;
    std::function<void(float)> decayTimeChangeCallback;
    std::function<void(float)> sustainLevelChangeCallback;
    std::function<void(float)> releaseTimeChangeCallback;
    
    // ADSR visualization component
    class EnvelopeVisualizer : public juce::Component {
    public:
        EnvelopeVisualizer()
            : attackTime(10.0f), decayTime(100.0f), sustainLevel(0.7f), releaseTime(200.0f)
        {
        }
        
        void setParameters(float attack, float decay, float sustain, float release)
        {
            attackTime = attack;
            decayTime = decay;
            sustainLevel = sustain;
            releaseTime = release;
            repaint();
        }
        
        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colours::black);
            
            g.setColour(juce::Colours::white);
            
            // Draw the envelope curve
            int width = getWidth();
            int height = getHeight();
            
            // Calculate the total envelope duration for scaling
            float totalDuration = attackTime + decayTime + 500.0f + releaseTime; // Add some sustain time for display
            
            // Calculate the x positions for each stage
            int attackX = (int)((attackTime / totalDuration) * width);
            int decayX = attackX + (int)((decayTime / totalDuration) * width);
            int releaseX = decayX + (int)((500.0f / totalDuration) * width); // Some arbitrary sustain time
            
            // Calculate the y positions for each stage
            int zeroY = height;
            int peakY = 0;
            int sustainY = (int)((1.0f - sustainLevel) * height);
            
            // Create a path for the envelope curve
            juce::Path path;
            path.startNewSubPath(0, zeroY);
            path.lineTo(attackX, peakY);
            path.lineTo(decayX, sustainY);
            path.lineTo(releaseX, sustainY);
            path.lineTo(width, zeroY);
            
            g.strokePath(path, juce::PathStrokeType(1.5f));
        }
        
    private:
        float attackTime;
        float decayTime;
        float sustainLevel;
        float releaseTime;
    };
    
    EnvelopeVisualizer visualizer;
    
    // Create and set up the controls
    void createControls();
    
    // Update the visualizer
    void updateVisualizer();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopePanel)
};

} // namespace UndergroundBeats
