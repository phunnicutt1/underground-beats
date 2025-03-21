/*
 * Underground Beats
 * OscillatorPanel.cpp
 * 
 * Implementation of UI panel for oscillator controls
 */

#include "OscillatorPanel.h"

namespace UndergroundBeats {

OscillatorPanel::OscillatorPanel(const juce::String& name)
    : oscillatorName(name)
    , currentWaveform(WaveformType::Sine)
    , currentDetune(0.0f)
    , currentLevel(0.75f)
{
    createControls();
}

OscillatorPanel::~OscillatorPanel()
{
}

void OscillatorPanel::setWaveform(WaveformType type, juce::NotificationType notification)
{
    currentWaveform = type;
    
    // Update the combo box selection
    int itemId = static_cast<int>(type) + 1; // +1 because combo box items start at 1
    waveformCombo.setSelectedId(itemId, notification);
}

WaveformType OscillatorPanel::getWaveform() const
{
    return currentWaveform;
}

void OscillatorPanel::setDetune(float cents, juce::NotificationType notification)
{
    currentDetune = cents;
    detuneSlider.setValue(cents, notification);
}

float OscillatorPanel::getDetune() const
{
    return currentDetune;
}

void OscillatorPanel::setLevel(float level, juce::NotificationType notification)
{
    currentLevel = level;
    levelSlider.setValue(level, notification);
}

float OscillatorPanel::getLevel() const
{
    return currentLevel;
}

void OscillatorPanel::setWaveformChangeCallback(std::function<void(WaveformType)> callback)
{
    waveformChangeCallback = callback;
}

void OscillatorPanel::setDetuneChangeCallback(std::function<void(float)> callback)
{
    detuneChangeCallback = callback;
}

void OscillatorPanel::setLevelChangeCallback(std::function<void(float)> callback)
{
    levelChangeCallback = callback;
}

void OscillatorPanel::resized()
{
    // Layout the controls
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int controlHeight = 24;
    int labelHeight = 20;
    
    // Panel title at the top
    waveformLabel.setBounds(margin, margin, width - margin * 2, labelHeight);
    
    // Waveform combo below title
    waveformCombo.setBounds(margin, margin + labelHeight + 5, width - margin * 2, controlHeight);
    
    // Detune slider
    int detuneY = margin + labelHeight + 5 + controlHeight + margin;
    detuneLabel.setBounds(margin, detuneY, width - margin * 2, labelHeight);
    detuneSlider.setBounds(margin, detuneY + labelHeight + 5, width - margin * 2, controlHeight);
    
    // Level slider
    int levelY = detuneY + labelHeight + 5 + controlHeight + margin;
    levelLabel.setBounds(margin, levelY, width - margin * 2, labelHeight);
    levelSlider.setBounds(margin, levelY + labelHeight + 5, width - margin * 2, controlHeight);
}

void OscillatorPanel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey.darker(0.2f));
    
    // Draw border
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(oscillatorName, getLocalBounds().reduced(10, 10), juce::Justification::topLeft, true);
}

void OscillatorPanel::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &waveformCombo)
    {
        int selectedId = waveformCombo.getSelectedId();
        
        // Convert from combo box ID to waveform type (subtract 1 because combo box items start at 1)
        currentWaveform = static_cast<WaveformType>(selectedId - 1);
        
        // Call the callback if set
        if (waveformChangeCallback)
        {
            waveformChangeCallback(currentWaveform);
        }
    }
}

void OscillatorPanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &detuneSlider)
    {
        currentDetune = static_cast<float>(detuneSlider.getValue());
        
        // Call the callback if set
        if (detuneChangeCallback)
        {
            detuneChangeCallback(currentDetune);
        }
    }
    else if (slider == &levelSlider)
    {
        currentLevel = static_cast<float>(levelSlider.getValue());
        
        // Call the callback if set
        if (levelChangeCallback)
        {
            levelChangeCallback(currentLevel);
        }
    }
}

void OscillatorPanel::createControls()
{
    // Waveform selector
    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(waveformLabel);
    
    waveformCombo.addItem("Sine", static_cast<int>(WaveformType::Sine) + 1);
    waveformCombo.addItem("Triangle", static_cast<int>(WaveformType::Triangle) + 1);
    waveformCombo.addItem("Sawtooth", static_cast<int>(WaveformType::Sawtooth) + 1);
    waveformCombo.addItem("Square", static_cast<int>(WaveformType::Square) + 1);
    waveformCombo.addItem("Noise", static_cast<int>(WaveformType::Noise) + 1);
    waveformCombo.addItem("Wavetable", static_cast<int>(WaveformType::Wavetable) + 1);
    waveformCombo.setSelectedId(static_cast<int>(WaveformType::Sine) + 1);
    waveformCombo.addListener(this);
    addAndMakeVisible(waveformCombo);
    
    // Detune control
    detuneLabel.setText("Detune (cents)", juce::dontSendNotification);
    detuneLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(detuneLabel);
    
    detuneSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    detuneSlider.setRange(-100.0, 100.0, 1.0);
    detuneSlider.setValue(0.0);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    detuneSlider.addListener(this);
    addAndMakeVisible(detuneSlider);
    
    // Level control
    levelLabel.setText("Level", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(levelLabel);
    
    levelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    levelSlider.setRange(0.0, 1.0, 0.01);
    levelSlider.setValue(0.75);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    levelSlider.addListener(this);
    addAndMakeVisible(levelSlider);
}

} // namespace UndergroundBeats
