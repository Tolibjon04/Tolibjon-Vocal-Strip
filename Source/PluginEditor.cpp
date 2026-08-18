#include "PluginProcessor.h"
#include "PluginEditor.h"

TolibjonProductionAudioProcessorEditor::TolibjonProductionAudioProcessorEditor (TolibjonProductionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto setup = [this](juce::Slider& s, juce::Label& l, HardwareKnobLook& look, const juce::String& txt, const juce::String& suf)
    {
        s.setLookAndFeel (&look);
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 65, 18);
        s.setTextValueSuffix (suf);
        addAndMakeVisible (s);

        l.setText (txt, juce::dontSendNotification);
        l.setFont (juce::Font (10.5f, juce::Font::bold));
        l.setColour (juce::Label::textColourId, juce::Colour (0xff8a96a8));
        l.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (l);
    };

    setup (threshSlider,  threshLabel,  amberLook, "THRESHOLD", " dB");
    setup (ratioSlider,   ratioLabel,   cyanLook,  "RATIO",     ":1");
    setup (gainSlider,    gainLabel,    amberLook, "GAIN",      " dB");
    setup (attackSlider,  attackLabel,  cyanLook,  "ATTACK",    " ms");
    setup (releaseSlider, releaseLabel, amberLook, "RELEASE",   " ms");
    setup (mixSlider,     mixLabel,     cyanLook,  "MIX",       " %");
    setup (driveSlider,   driveLabel,   amberLook, "DRIVE",     "");

    satTypeBox.addItemList ({ "Tube", "Tape", "Solid State" }, 1);
    satTypeBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff12161f));
    satTypeBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xff00d5f8));
    satTypeBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff2a3242));
    addAndMakeVisible (satTypeBox);

    threshAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "thresh", threshSlider);
    ratioAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ratio", ratioSlider);
    attackAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "attack", attackSlider);
    releaseAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "release", releaseSlider);
    mixAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "mix", mixSlider);
    gainAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    driveAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "drive", driveSlider);
    satTypeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "satType", satTypeBox);

    startTimerHz (30);
    setSize (820, 420);
}

TolibjonProductionAudioProcessorEditor::~TolibjonProductionAudioProcessorEditor()
{
    stopTimer();
    threshSlider.setLookAndFeel (nullptr);
    ratioSlider.setLookAndFeel (nullptr);
    attackSlider.setLookAndFeel (nullptr);
    releaseSlider.setLookAndFeel (nullptr);
    mixSlider.setLookAndFeel (nullptr);
    gainSlider.setLookAndFeel (nullptr);
    driveSlider.setLookAndFeel (nullptr);
}

void TolibjonProductionAudioProcessorEditor::timerCallback()
{
    currentIn  = currentIn * 0.7f + audioProcessor.inputMeterValue.load() * 0.3f;
    currentOut = currentOut * 0.7f + audioProcessor.outputMeterValue.load() * 0.3f;
    repaint();
}

void TolibjonProductionAudioProcessorEditor::drawVUMeter (juce::Graphics& g, juce::Rectangle<float> b, float val, const juce::String& title)
{
    g.setColour (juce::Colour (0xff323a48));
    g.drawRoundedRectangle (b, 6.0f, 2.0f);

    auto inner = b.reduced (3.0f);
    juce::ColourGradient vuBg (juce::Colour (0xffffcf85), inner.getCentreX(), inner.getY(), juce::Colour (0xffb57930), inner.getCentreX(), inner.getBottom(), false);
    g.setGradientFill (vuBg);
    g.fillRoundedRectangle (inner, 4.0f);

    g.setColour (juce::Colours::white.withAlpha (0.15f));
    g.fillRoundedRectangle (inner.removeFromTop (inner.getHeight() * 0.45f), 4.0f);

    g.setColour (juce::Colour (0xff2b1d0c));
    g.setFont (juce::Font (8.5f, juce::Font::bold));
    g.drawText ("-20  -10  -5  -3  -1  0  +1  +2  +3", b.getX(), b.getY() + 14, b.getWidth(), 12, juce::Justification::centred);

    g.setColour (juce::Colour (0xff8a1a10));
    g.drawText ("+5", b.getRight() - 25, b.getY() + 14, 15, 12, juce::Justification::centred);

    g.setColour (juce::Colour (0xff3a2810));
    g.setFont (juce::Font (12.0f, juce::Font::bold));
    g.drawText ("VU", b.getX(), b.getY() + 32, b.getWidth(), 14, juce::Justification::centred);

    float needleAngle = juce::jmap (juce::jlimit (0.0f, 1.0f, val), -0.7f, 0.7f);
    auto pivot = juce::Point<float> (b.getCentreX(), b.getBottom() + 15.0f);
    juce::Path needle;
    needle.startNewSubPath (pivot);
    needle.lineTo (pivot.translated (std::sin (needleAngle) * 75.0f, -std::cos (needleAngle) * 75.0f));

    g.setColour (juce::Colour (0xff121212));
    g.strokePath (needle, juce::PathStrokeType (1.8f));

    g.setColour (juce::Colour (0xff8a96a8));
    g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.drawText (title, b.getX(), b.getBottom() + 4, b.getWidth(), 16, juce::Justification::centred);
}

void TolibjonProductionAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bg (juce::Colour (0xff0f131a), 0.0f, 0.0f, juce::Colour (0xff080a0e), 0.0f, (float) getHeight(), false);
    g.setGradientFill (bg);
    g.fillAll();

    g.setColour (juce::Colour (0xff252c38));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (2.0f), 10.0f, 2.5f);

    g.setColour (juce::Colour (0xff00d5f8));
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("TOLIBJON", 30, 22, 220, 24, juce::Justification::left);
    g.setColour (juce::Colours::white);
    g.drawText ("PRODUCTION", 30, 44, 220, 20, juce::Justification::left);

    g.setColour (juce::Colour (0xff8a96a8));
    g.setFont (juce::Font (10.5f, juce::Font::bold));
    g.drawText ("VOCAL COMPRESSOR & SATURATOR", 30, 72, 260, 16, juce::Justification::left);

    drawVUMeter (g, juce::Rectangle<float> (380, 22, 175, 95), currentIn, "INPUT");
    drawVUMeter (g, juce::Rectangle<float> (580, 22, 175, 95), currentOut, "OUTPUT");

    auto specArea = juce::Rectangle<float> (275, 175, 260, 175);
    g.setColour (juce::Colour (0xff0a0d13));
    g.fillRoundedRectangle (specArea, 6.0f);
    g.setColour (juce::Colour (0xff1f2736));
    g.drawRoundedRectangle (specArea, 6.0f, 1.5f);

    g.setColour (juce::Colour (0xff141a24));
    for (int i = 1; i < 5; ++i)
        g.drawHorizontalLine ((int) (specArea.getY() + i * (specArea.getHeight() / 5.0f)), specArea.getX(), specArea.getRight());

    juce::Path amberCurve;
    amberCurve.startNewSubPath (specArea.getX(), specArea.getCentreY() + 30);
    amberCurve.cubicTo (specArea.getX() + 60, specArea.getCentreY() - 40, specArea.getX() + 180, specArea.getCentreY() - 30, specArea.getRight(), specArea.getCentreY() + 45);
    g.setColour (juce::Colour (0xffffa024));
    g.strokePath (amberCurve, juce::PathStrokeType (2.0f));

    juce::Path cyanCurve;
    cyanCurve.startNewSubPath (specArea.getX(), specArea.getCentreY() + 40);
    cyanCurve.cubicTo (specArea.getX() + 80, specArea.getCentreY() + 5, specArea.getX() + 160, specArea.getCentreY() + 20, specArea.getRight(), specArea.getCentreY() + 65);
    g.setColour (juce::Colour (0xff00d5f8));
    g.strokePath (cyanCurve, juce::PathStrokeType (1.8f));

    g.setColour (juce::Colour (0xff8a96a8));
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    g.drawText ("SATURATION TYPE", 665, 345, 130, 16, juce::Justification::centred);
}

void TolibjonProductionAudioProcessorEditor::resized()
{
    threshLabel.setBounds (35, 150, 90, 16);
    threshSlider.setBounds (35, 168, 90, 85);

    ratioLabel.setBounds (145, 150, 90, 16);
    ratioSlider.setBounds (145, 168, 90, 85);

    gainLabel.setBounds (90, 275, 90, 16);
    gainSlider.setBounds (90, 293, 90, 85);

    attackLabel.setBounds (560, 150, 75, 16);
    attackSlider.setBounds (560, 168, 75, 80);

    releaseLabel.setBounds (645, 150, 75, 16);
    releaseSlider.setBounds (645, 168, 75, 80);

    mixLabel.setBounds (725, 150, 75, 16);
    mixSlider.setBounds (725, 168, 75, 80);

    driveLabel.setBounds (580, 275, 80, 16);
    driveSlider.setBounds (580, 293, 80, 85);

    satTypeBox.setBounds (675, 305, 110, 26);
}
