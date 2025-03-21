/*
 * Underground Beats
 * FilterPanel.h
 * 
 * UI panel for filter controls
 */

#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Filter.h"
#include <functional>

namespace UndergroundBeats {

/**
 * @class FilterPanel
 * @brief UI panel for filter controls
 * 
 * The FilterPanel class provides a user interface for controlling
 * the parameters of a filter, including filter type, cutoff frequency,
 * resonance, and gain.
 */
class FilterPanel : public juce::Component,
                   public juce::ComboBox::Listener,
                   public juce::Slider::Listener {
public:
    FilterPanel(const juce::String& name = "Filter");
    ~FilterPanel() override;
    
    /**
     * @brief Set the filter type
     * 
     * @param type The filter type to set
     * @param notification Whether to send a notification
     */
    void setFilterType(FilterType type, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current filter type
     * 
     * @return The current filter type
     */
    FilterType getFilterType() const;
    
    /**
     * @brief Set the cutoff frequency
     * 
     * @param frequencyHz Cutoff frequency in Hertz
     * @param notification Whether to send a notification
     */
    void setCutoff(float frequencyHz, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current cutoff frequency
     * 
     * @return The current cutoff frequency in Hertz
     */
    float getCutoff() const;
    
    /**
     * @brief Set the resonance
     * 
     * @param amount Resonance amount (0.0 to 1.0)
     * @param notification Whether to send a notification
     */
    void setResonance(float amount, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current resonance
     * 
     * @return The current resonance amount (0.0 to 1.0)
     */
    float getResonance() const;
    
    /**
     * @brief Set the gain (for shelf and peak filters)
     * 
     * @param gainDb Gain in decibels
     * @param notification Whether to send a notification
     */
    void setGain(float gainDb, juce::NotificationType notification = juce::sendNotificationAsync);
    
    /**
     * @brief Get the current gain
     * 
     * @return The current gain in decibels
     */
    float getGain() const;
    
    /**
     * @brief Set a callback for when the filter type changes
     * 
     * @param callback The callback function
     */
    void setFilterTypeChangeCallback(std::function<void(FilterType)> callback);
    
    /**
     * @brief Set a callback for when the cutoff frequency changes
     * 
     * @param callback The callback function
     */
    void setCutoffChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the resonance changes
     * 
     * @param callback The callback function
     */
    void setResonanceChangeCallback(std::function<void(float)> callback);
    
    /**
     * @brief Set a callback for when the gain changes
     * 
     * @param callback The callback function
     */
    void setGainChangeCallback(std::function<void(float)> callback);
    
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
    juce::String filterName;
    
    // Filter type selector
    juce::ComboBox filterTypeCombo;
    juce::Label filterTypeLabel;
    
    // Cutoff control
    juce::Slider cutoffSlider;
    juce::Label cutoffLabel;
    
    // Resonance control
    juce::Slider resonanceSlider;
    juce::Label resonanceLabel;
    
    // Gain control (for shelf and peak filters)
    juce::Slider gainSlider;
    juce::Label gainLabel;
    
    // Current values
    FilterType currentFilterType;
    float currentCutoff;
    float currentResonance;
    float currentGain;
    
    // Callbacks
    std::function<void(FilterType)> filterTypeChangeCallback;
    std::function<void(float)> cutoffChangeCallback;
    std::function<void(float)> resonanceChangeCallback;
    std::function<void(float)> gainChangeCallback;
    
    // Filter response visualization component
    class FilterVisualizer : public juce::Component {
    public:
        FilterVisualizer()
            : filterType(FilterType::LowPass), cutoff(1000.0f), resonance(0.5f), gain(0.0f)
        {
        }
        
        void setParameters(FilterType type, float cutoffFreq, float reso, float gainDb)
        {
            filterType = type;
            cutoff = cutoffFreq;
            resonance = reso;
            gain = gainDb;
            repaint();
        }
        
        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colours::black);
            
            g.setColour(juce::Colours::white);
            
            // Draw the filter response curve
            juce::Path path;
            
            int width = getWidth();
            int height = getHeight();
            
            const int numPoints = 100;
            
            // Draw the frequency axis (log scale)
            const float minFreq = 20.0f;
            const float maxFreq = 20000.0f;
            
            // Draw the first point
            float x = 0;
            float y = getResponseAtFrequency(minFreq);
            y = juce::jmap(y, -24.0f, 24.0f, static_cast<float>(height), 0.0f);
            
            path.startNewSubPath(x, y);
            
            // Draw the rest of the curve
            for (int i = 1; i < numPoints; ++i)
            {
                x = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(numPoints - 1), 0.0f, static_cast<float>(width));
                float freq = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(numPoints - 1), std::log10(minFreq), std::log10(maxFreq));
                freq = std::pow(10.0f, freq);
                
                y = getResponseAtFrequency(freq);
                y = juce::jmap(y, -24.0f, 24.0f, static_cast<float>(height), 0.0f);
                
                path.lineTo(x, y);
            }
            
            g.strokePath(path, juce::PathStrokeType(1.5f));
        }
        
    private:
        FilterType filterType;
        float cutoff;
        float resonance;
        float gain;
        
        // Calculate the filter response at a given frequency
        float getResponseAtFrequency(float frequency)
        {
            // Simple approximation of filter response
            float response = 0.0f;
            
            // Resonance factor (Q)
            float q = 1.0f / (2.0f * resonance);
            
            // Relative frequency
            float f = frequency / cutoff;
            
            switch (filterType)
            {
                case FilterType::LowPass:
                    response = 20.0f * std::log10(1.0f / std::sqrt(1.0f + std::pow(f, 4)));
                    if (f < 1.0f && resonance > 0.7f)
                    {
                        response += (resonance - 0.7f) * 20.0f;
                    }
                    break;
                    
                case FilterType::HighPass:
                    response = 20.0f * std::log10(1.0f / std::sqrt(1.0f + std::pow(1.0f / f, 4)));
                    if (f > 1.0f && resonance > 0.7f)
                    {
                        response += (resonance - 0.7f) * 20.0f;
                    }
                    break;
                    
                case FilterType::BandPass:
                    response = 20.0f * std::log10(1.0f / (1.0f + std::pow(q * (f - 1.0f / f), 2)));
                    break;
                    
                case FilterType::Notch:
                    response = 20.0f * std::log10(std::abs((std::pow(f, 2) - 1.0f) / (std::pow(f / q, 2) + 1.0f)));
                    break;
                    
                case FilterType::LowShelf:
                    if (f < 1.0f)
                    {
                        response = gain;
                    }
                    else
                    {
                        response = gain * (1.0f / f);
                    }
                    break;
                    
                case FilterType::HighShelf:
                    if (f > 1.0f)
                    {
                        response = gain;
                    }
                    else
                    {
                        response = gain * f;
                    }
                    break;
                    
                case FilterType::Peak:
                    response = gain * (1.0f / (1.0f + std::pow(q * (f - 1.0f / f), 2)));
                    break;
            }
            
            return response;
        }
    };
    
    FilterVisualizer visualizer;
    
    // Create and set up the controls
    void createControls();
    
    // Update the visualizer
    void updateVisualizer();
    
    // Update control visibility based on filter type
    void updateControlVisibility();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};

} // namespace UndergroundBeats
