#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MonoBassAudioProcessorEditor::MonoBassAudioProcessorEditor (MonoBassAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (200, 250);

    addAndMakeVisible(freqSlider);
    freqAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, FREQ_ID, freqSlider);
    freqSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, getWidth(), getHeight() / 10);
    freqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

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
    freqSlider.setBounds(0, 10, getWidth(), getWidth());
    freqLabel.setBounds(0, 210, getWidth(), getHeight() / 10);
}
