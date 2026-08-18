#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class TolibjonProductionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TolibjonProductionAudioProcessorEditor (TolibjonProductionAudioProcessor&);
    ~TolibjonProductionAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TolibjonProductionAudioProcessor& audioProcessor;

    juce::Slider threshSlider, ratioSlider, warmthSlider, gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshAttachment, ratioAttachment, warmthAttachment, gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TolibjonProductionAudioProcessorEditor)
};
