#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class HardwareKnobLook : public juce::LookAndFeel_V4
{
public:
    juce::Colour glowColour;

    HardwareKnobLook (juce::Colour col) : glowColour (col)
    {
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffd8e2ec));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 10.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f - 6.0f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;

        // Aniq standart soat mili bo'yicha burchaklar: -135 gradusdan +135 gradusgacha
        float startAngle = -2.35619f; // -135 deg
        float endAngle   =  2.35619f; // +135 deg
        float currentAngle = startAngle + sliderPosProportional * (endAngle - startAngle);

        // Orqa soya
        g.setColour (juce::Colour (0xff05070a));
        g.fillEllipse (rx - 2.0f, ry - 1.0f, rw + 4.0f, rw + 4.0f);

        // Qora metall asos
        juce::ColourGradient grad (juce::Colour (0xff2b313d), centreX, ry, juce::Colour (0xff12141a), centreX, ry + rw, false);
        g.setGradientFill (grad);
        g.fillEllipse (rx, ry, rw, rw);

        g.setColour (juce::Colour (0xff414856));
        g.drawEllipse (rx, ry, rw, rw, 1.5f);

        // Orqa nofaol chiziq (Background Arc)
        juce::Path bgArc;
        bgArc.addCentredArc (centreX, centreY, radius + 4.0f, radius + 4.0f, 0.0f, startAngle, endAngle, true);
        g.setColour (juce::Colour (0xff161a22));
        g.strokePath (bgArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Faol neon rangli chiziq (Active Arc - soat mili bo'yicha to'ladi)
        if (sliderPosProportional > 0.001f)
        {
            juce::Path activeArc;
            activeArc.addCentredArc (centreX, centreY, radius + 4.0f, radius + 4.0f, 0.0f, startAngle, currentAngle, true);
            g.setColour (glowColour);
            g.strokePath (activeArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Ko'rsatkich chizig'i (Pointer)
        juce::Path p;
        p.addRoundedRectangle (-2.0f, -radius + 3.0f, 4.0f, radius * 0.45f, 1.5f);
        p.applyTransform (juce::AffineTransform::rotation (currentAngle).translated (centreX, centreY));
        g.setColour (glowColour.brighter (0.35f));
        g.fillPath (p);
    }
};

class TolibjonProductionAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    TolibjonProductionAudioProcessorEditor (TolibjonProductionAudioProcessor&);
    ~TolibjonProductionAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawVUMeter (juce::Graphics& g, juce::Rectangle<float> bounds, float value, const juce::String& title);

    TolibjonProductionAudioProcessor& audioProcessor;

    HardwareKnobLook amberLook { juce::Colour (0xffffa024) };
    HardwareKnobLook cyanLook  { juce::Colour (0xff00d5f8) };

    juce::Slider threshSlider, ratioSlider, attackSlider, releaseSlider, mixSlider, gainSlider, driveSlider;
    juce::Label threshLabel, ratioLabel, attackLabel, releaseLabel, mixLabel, gainLabel, driveLabel;
    juce::ComboBox satTypeBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshAtt, ratioAtt, attackAtt, releaseAtt, mixAtt, gainAtt, driveAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> satTypeAtt;

    float currentIn = 0.0f;
    float currentOut = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TolibjonProductionAudioProcessorEditor)
};
