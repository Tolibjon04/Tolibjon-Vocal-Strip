#include "PluginProcessor.h"
#include "PluginEditor.h"

TolibjonProductionAudioProcessor::TolibjonProductionAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

TolibjonProductionAudioProcessor::~TolibjonProductionAudioProcessor() {}

const juce::String TolibjonProductionAudioProcessor::getName() const { return "Tolibjon Production Vocal Strip"; }
bool TolibjonProductionAudioProcessor::acceptsMidi() const { return false; }
bool TolibjonProductionAudioProcessor::producesMidi() const { return false; }
bool TolibjonProductionAudioProcessor::isMidiEffect() const { return false; }
double TolibjonProductionAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int TolibjonProductionAudioProcessor::getNumPrograms() { return 1; }
int TolibjonProductionAudioProcessor::getCurrentProgram() { return 0; }
void TolibjonProductionAudioProcessor::setCurrentProgram (int) {}
const juce::String TolibjonProductionAudioProcessor::getProgramName (int) { return {}; }
void TolibjonProductionAudioProcessor::changeProgramName (int, const juce::String&) {}

void TolibjonProductionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

    compressor.prepare (spec);
    outputGain.prepare (spec);
}

void TolibjonProductionAudioProcessor::releaseResources() {}

bool TolibjonProductionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void TolibjonProductionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto thresh = apvts.getRawParameterValue ("thresh")->load();
    auto ratio  = apvts.getRawParameterValue ("ratio")->load();
    auto gain   = apvts.getRawParameterValue ("gain")->load();
    auto warmth = apvts.getRawParameterValue ("warmth")->load();

    compressor.setThreshold (thresh);
    compressor.setRatio (ratio);
    compressor.setAttack (15.0f);
    compressor.setRelease (100.0f);

    outputGain.setGainDecibels (gain);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    compressor.process (context);

    if (warmth > 0.0f)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float x = channelData[sample];
                channelData[sample] = std::tanh (x * (1.0f + warmth * 2.0f));
            }
        }
    }

    outputGain.process (context);
}

bool TolibjonProductionAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* TolibjonProductionAudioProcessor::createEditor()
{
    return new TolibjonProductionAudioProcessorEditor (*this);
}

void TolibjonProductionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TolibjonProductionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout TolibjonProductionAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> ("thresh", "Threshold", -60.0f, 0.0f, -15.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("ratio",  "Ratio",      1.0f, 20.0f, 4.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("warmth", "Warmth",     0.0f, 1.0f, 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("gain",   "Gain",       -24.0f, 24.0f, 0.0f));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TolibjonProductionAudioProcessor();
}
