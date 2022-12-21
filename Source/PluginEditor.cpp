#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MonoBassAudioProcessorEditor::MonoBassAudioProcessorEditor (MonoBassAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (200, 200);

    addAndMakeVisible(freqSlider);
    freqAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, FREQ_ID, freqSlider);
    freqSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, getWidth(), getHeight() / 5);

    addAndMakeVisible(freqLabel);
    freqLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
    freqLabel.setJustificationType(juce::Justification::centred);
}

MonoBassAudioProcessorEditor::~MonoBassAudioProcessorEditor()
{
}

//==============================================================================
void MonoBassAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MonoBassAudioProcessorEditor::resized()
{
    freqSlider.setBounds(getBounds());
    freqLabel.setBounds(0, getHeight() * (4 / 5), getWidth(), getHeight() * (1 / 5));
}
