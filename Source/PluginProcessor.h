#pragma once

#include <JuceHeader.h>
#include "BandSplitter.h"

#define FREQ_ID "freq"
#define FREQ_DEFAULT 100.0f
#define MID_CUT_Q 0.6f

typedef juce::dsp::IIR::Filter<float> Filter;
typedef juce::dsp::IIR::Coefficients<float> Coeffs;
typedef juce::AudioProcessorValueTreeState::ParameterLayout ParameterLayout;

//==============================================================================
class MonoBassAudioProcessor  : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener,
                                public juce::ChangeBroadcaster
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MonoBassAudioProcessor();
    ~MonoBassAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    ParameterLayout createParameterLayout();
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    inline float dBtoRatio(float dB)
    {
        return pow(10, dB / 20);
    }

    //==============================================================================
    std::vector<std::unique_ptr<BandSplitter>> bandSplitters;
    juce::dsp::ProcessorDuplicator<Filter, Coeffs> midCut;
    juce::dsp::ProcessSpec spec;

    int numChannels;
    double sampleRate;
    float freq;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonoBassAudioProcessor)
};
