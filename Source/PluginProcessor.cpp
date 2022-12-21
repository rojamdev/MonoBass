#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MonoBassAudioProcessor::MonoBassAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "PARAMS", createParameterLayout())
#endif
{
    sampleRate = 0.0;
    freq = FREQ_DEFAULT;
    numChannels = getTotalNumInputChannels();

    for (int channel = 0; channel < numChannels; ++channel)
        bandSplitters.push_back(std::make_unique<BandSplitter>());

    apvts.addParameterListener(FREQ_ID, this);
}

MonoBassAudioProcessor::~MonoBassAudioProcessor()
{
}

ParameterLayout MonoBassAudioProcessor::createParameterLayout()
{
    ParameterLayout params;

    juce::NormalisableRange<float> freqRange = juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f);
    freqRange.setSkewForCentre(FREQ_DEFAULT);
    params.add(std::make_unique<juce::AudioParameterFloat>("freq", "Frequency", freqRange, FREQ_DEFAULT));
    
    return params;
}

void MonoBassAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == FREQ_ID)
    {
        freq = newValue;

        for (int channel = 0; channel < numChannels; ++channel)
            bandSplitters[channel]->calcCoeffs(sampleRate, freq);

        *midCut.state = *Coeffs::makePeakFilter(sampleRate, freq, MID_CUT_Q, dBtoRatio(-3.0f));
    }
}

//==============================================================================
const juce::String MonoBassAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MonoBassAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MonoBassAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MonoBassAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MonoBassAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int MonoBassAudioProcessor::getNumPrograms() { return 1; }

int MonoBassAudioProcessor::getCurrentProgram() { return 0; }

void MonoBassAudioProcessor::setCurrentProgram (int index) {}

const juce::String MonoBassAudioProcessor::getProgramName (int index) { return {}; }

void MonoBassAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

//==============================================================================
void MonoBassAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    
    for (int channel = 0; channel < numChannels; ++channel)
        bandSplitters[channel]->calcCoeffs(sampleRate, freq);

    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;

    midCut.state = Coeffs::makePeakFilter(sampleRate, freq, MID_CUT_Q, dBtoRatio(-3.0f));
    midCut.reset();
    midCut.prepare(spec);
}

void MonoBassAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MonoBassAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MonoBassAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);

    // Main processing loop
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float lowSampleSum = 0.0f;
        std::vector<float> highSamples;
        
        // Iterates through channels, splitting the signal and adding to arrays
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getReadPointer(channel);

            std::array<float, 2> bandSamples = bandSplitters[channel]->processSample(channelData[sample]);

            lowSampleSum += bandSamples[0];
            highSamples.push_back(bandSamples[1]);
        }

        lowSampleSum = lowSampleSum / numChannels; // Sums low band samples from each channel

        // Iterates through channels, this time writing the sum of the low samples added to the
        // respective channel's high sample
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            channelData[sample] = lowSampleSum + highSamples[channel];
        }
    }

    midCut.process(juce::dsp::ProcessContextReplacing(block));
}

//==============================================================================
bool MonoBassAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MonoBassAudioProcessor::createEditor()
{
    return new MonoBassAudioProcessorEditor (*this);
}

//==============================================================================
void MonoBassAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void MonoBassAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MonoBassAudioProcessor();
}
