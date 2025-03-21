/*
 * Underground Beats
 * EnvelopePanel.cpp
 * 
 * Implementation of UI panel for envelope controls
 */

#include "EnvelopePanel.h"

namespace UndergroundBeats {

EnvelopePanel::EnvelopePanel(const juce::String& name)
    : envelopeName(name)
    , currentAttackTime(10.0f)
    , currentDecayTime(100.0f)
    , currentSustainLevel(0.7f)
    , currentReleaseTime(200.0f)
{
    createControls();
}

EnvelopePanel::~EnvelopePanel()
{
}

void EnvelopePanel::setAttackTime(float timeMs, juce::NotificationType notification)
{
    currentAttackTime = timeMs;
    attackSlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getAttackTime() const
{
    return currentAttackTime;
}

void EnvelopePanel::setDecayTime(float timeMs, juce::NotificationType notification)
{
    currentDecayTime = timeMs;
    decaySlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getDecayTime() const
{
    return currentDecayTime;
}

void EnvelopePanel::setSustainLevel(float level, juce::NotificationType notification)
{
    currentSustainLevel = level;
    sustainSlider.setValue(level, notification);
    updateVisualizer();
}

float EnvelopePanel::getSustainLevel() const
{
    return currentSustainLevel;
}

void EnvelopePanel::setReleaseTime(float timeMs, juce::NotificationType notification)
{
    currentReleaseTime = timeMs;
    releaseSlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getReleaseTime() const
{
    return currentReleaseTime;
}

void EnvelopePanel::setAttackTimeChangeCallback(std::function<void(float)> callback)
{
    attackTimeChangeCallback = callback;
}

void EnvelopePanel::setDecayTimeChangeCallback(std::function<void(float)> callback)
{
    decayTimeChangeCallback = callback;
}

void EnvelopePanel::setSustainLevelChangeCallback(std::function<void(float)> callback)
{
    sustainLevelChangeCallback = callback;
}

void EnvelopePanel::setReleaseTimeChangeCallback(std::function<void(float)> callback)
{
    releaseTimeChangeCallback = callback;
}

void EnvelopePanel::resized()
{
    // Layout the controls
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int controlHeight = 24;
    int labelHeight = 20;
    int visualizerHeight = 80;
    
    // Panel title at the top
    
    // Visualizer at the top
    visualizer.setBounds(margin, margin, width - margin * 2, visualizerHeight);
    
    // ADSR controls below visualizer
    int controlsY = margin + visualizerHeight + margin;
    
    // Attack control
    attackLabel.setBounds(margin, controlsY, width - margin * 2, labelHeight);
    attackSlider.setBounds(margin, controlsY + labelHeight + 5, width - margin * 2, controlHeight);
    
    // Decay control
    int decayY = controlsY + labelHeight + 5 + controlHeight + margin;
    decayLabel.setBounds(margin, decayY, width - margin * 2, labelHeight);
    decaySlider.setBounds(margin, decayY + labelHeight + 5, width - margin * 2, controlHeight);
    
    // Sustain control
    int sustainY = decayY + labelHeight + 5 + controlHeight + margin;
    sustainLabel.setBounds(margin, sustainY, width - margin * 2, labelHeight);
    sustainSlider.setBounds(margin, sustainY + labelHeight + 5, width - margin * 2, controlHeight);
    
    // Release control
    int releaseY = sustainY + labelHeight + 5 + controlHeight + margin;
    releaseLabel.setBounds(margin, releaseY, width - margin * 2, labelHeight);
    releaseSlider.setBounds(margin, releaseY + labelHeight + 5, width - margin * 2, controlHeight);
}

void EnvelopePanel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey.darker(0.2f));
    
    // Draw border
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(envelopeName, getLocalBounds().reduced(10, 10), juce::Justification::topLeft, true);
}

void EnvelopePanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &attackSlider)
    {
        currentAttackTime = static_cast<float>(attackSlider.getValue());
        
        // Call the callback if set
        if (attackTimeChangeCallback)
        {
            attackTimeChangeCallback(currentAttackTime);
        }
        
        updateVisualizer();
    }
    else if (slider == &decaySlider)
    {
        currentDecayTime = static_cast<float>(decaySlider.getValue());
        
        // Call the callback if set
        if (decayTimeChangeCallback)
        {
            decayTimeChangeCallback(currentDecayTime);
        }
        
        updateVisualizer();
    }
    else if (slider == &sustainSlider)
    {
        currentSustainLevel = static_cast<float>(sustainSlider.getValue());
        
        // Call the callback if set
        if (sustainLevelChangeCallback)
        {
            sustainLevelChangeCallback(currentSustainLevel);
        }
        
        updateVisualizer();
    }
    else if (slider == &releaseSlider)
    {
        currentReleaseTime = static_cast<float>(releaseSlider.getValue());
        
        // Call the callback if set
        if (releaseTimeChangeCallback)
        {
            releaseTimeChangeCallback(currentReleaseTime);
        }
        
        updateVisualizer();
    }
}

void EnvelopePanel::createControls()
{
    // Add the visualizer
    addAndMakeVisible(visualizer);
    
    // Attack control
    attackLabel.setText("Attack (ms)", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(attackLabel);
    
    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setRange(1.0, 1000.0, 1.0);
    attackSlider.setSkewFactor(0.5); // Logarithmic scaling for more intuitive control
    attackSlider.setValue(currentAttackTime);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);
    
    // Decay control
    decayLabel.setText("Decay (ms)", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(decayLabel);
    
    decaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    decaySlider.setRange(1.0, 2000.0, 1.0);
    decaySlider.setSkewFactor(0.5); // Logarithmic scaling
    decaySlider.setValue(currentDecayTime);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    decaySlider.addListener(this);
    addAndMakeVisible(decaySlider);
    
    // Sustain control
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(sustainLabel);
    
    sustainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(currentSustainLevel);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    sustainSlider.addListener(this);
    addAndMakeVisible(sustainSlider);
    
    // Release control
    releaseLabel.setText("Release (ms)", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(releaseLabel);
    
    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setRange(1.0, 3000.0, 1.0);
    releaseSlider.setSkewFactor(0.5); // Logarithmic scaling
    releaseSlider.setValue(currentReleaseTime);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    releaseSlider.addListener(this);
    addAndMakeVisible(releaseSlider);
    
    // Initialize the visualizer
    updateVisualizer();
}

void EnvelopePanel::updateVisualizer()
{
    visualizer.setParameters(currentAttackTime, currentDecayTime, currentSustainLevel, currentReleaseTime);
}

} // namespace UndergroundBeats
