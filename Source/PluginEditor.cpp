#include "PluginProcessor.h"
#include "PluginEditor.h"

TolibjonProductionAudioProcessorEditor::TolibjonProductionAudioProcessorEditor (TolibjonProductionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto setupSlider = [this](juce::Slider& slider, const juce::String& text)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible (slider);
    };

    setupSlider (threshSlider, "Threshold");
    setupSlider (ratioSlider, "Ratio");
    setupSlider (warmthSlider, "Warmth");
    setupSlider (gainSlider, "Gain");

    threshAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "thresh", threshSlider);
    ratioAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ratio", ratioSlider);
    warmthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "warmth", warmthSlider);
    gainAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);

    setSize (450, 300);
}

TolibjonProductionAudioProcessorEditor::~TolibjonProductionAudioProcessorEditor() {}

void TolibjonProductionAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1e1e24));
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawFittedText ("Tolibjon Production - Vocal Strip", getLocalBounds().removeFromTop (40), juce::Justification::centred, 1);
}

void TolibjonProductionAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (40);

    auto width = area.getWidth() / 4;
    threshSlider.setBounds (area.removeFromLeft (width).reduced (5));
    ratioSlider.setBounds (area.removeFromLeft (width).reduced (5));
    warmthSlider.setBounds (area.removeFromLeft (width).reduced (5));
    gainSlider.setBounds (area.reduced (5));
}
