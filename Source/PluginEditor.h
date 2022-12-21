#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
class MonoBassAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MonoBassAudioProcessorEditor (MonoBassAudioProcessor&);
    ~MonoBassAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MonoBassAudioProcessor& audioProcessor;

    juce::Slider freqSlider;
    juce::Label freqLabel;
    std::unique_ptr<SliderAttachment> freqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonoBassAudioProcessorEditor)
};
