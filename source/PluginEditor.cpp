#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "PluginConfig.h"
#include "parameters/ParameterIDs.h"

namespace
{
    void configureKnobSlider (juce::Slider& slider)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.22f,
                                    juce::MathConstants<float>::pi * 2.78f,
                                    true);
    }
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel);

    titleLabel.setText ("POWER MACHINE", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::FontOptions { 28.0f, juce::Font::bold });
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("ANALOG GAIN STAGE", juce::dontSendNotification);
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setFont (juce::FontOptions { 13.0f });
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (72, 42, 18));
    addAndMakeVisible (subtitleLabel);

    for (auto* knob : { &driveKnob, &stage1Knob, &stage2Knob, &toneKnob, &presenceKnob, &analogKnob, &mixKnob, &outputKnob })
    {
        configureKnobSlider (knob->slider);
        addAndMakeVisible (*knob);
    }

    configureComboBox();
    addAndMakeVisible (oversamplingLabel);
    addAndMakeVisible (oversamplingBox);
    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);
    addAndMakeVisible (saturationMeter);

    autoGainButton.setClickingTogglesState (true);
    autoGainButton.getProperties().set ("footSwitch", true);
    autoGainButton.setButtonText ("AUTO");
    addAndMakeVisible (autoGainButton);

    bypassButton.setClickingTogglesState (true);
    bypassButton.getProperties().set ("footSwitch", true);
    bypassButton.setButtonText ("BYPASS");
    addAndMakeVisible (bypassButton);

    driveAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::inputDrive, driveKnob.slider);
    stage1Attachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::stage1Drive, stage1Knob.slider);
    stage2Attachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::stage2Drive, stage2Knob.slider);
    toneAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::tone, toneKnob.slider);
    presenceAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::presence, presenceKnob.slider);
    analogAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::analogCharacter, analogKnob.slider);
    mixAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::mix, mixKnob.slider);
    outputAttachment = std::make_unique<SliderAttachment> (
        processorRef.parameters, ParameterIDs::outputGain, outputKnob.slider);
    oversamplingAttachment = std::make_unique<ComboBoxAttachment> (
        processorRef.parameters, ParameterIDs::oversampling, oversamplingBox);
    autoGainAttachment = std::make_unique<ButtonAttachment> (
        processorRef.parameters, ParameterIDs::autoGain, autoGainButton);
    bypassAttachment = std::make_unique<ButtonAttachment> (
        processorRef.parameters, ParameterIDs::bypass, bypassButton);

    setSize (PluginConfig::defaultEditorWidth, PluginConfig::defaultEditorHeight);
    startTimerHz (30);
}


AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));

    const auto pedal = getLocalBounds().reduced (8).toFloat();

    g.setColour (juce::Colour::fromRGB (5, 5, 5).withAlpha (0.42f));
    g.fillRoundedRectangle (pedal.translated (0.0f, 4.0f), 18.0f);

    juce::ColourGradient face (juce::Colour::fromRGB (218, 153, 62),
                               pedal.getX(),
                               pedal.getY(),
                               juce::Colour::fromRGB (126, 73, 28),
                               pedal.getRight(),
                               pedal.getBottom(),
                               false);
    face.addColour (0.42, juce::Colour::fromRGB (238, 183, 92));
    face.addColour (0.72, juce::Colour::fromRGB (176, 101, 38));
    g.setGradientFill (face);
    g.fillRoundedRectangle (pedal, 16.0f);

    g.setColour (juce::Colour::fromRGB (71, 40, 18).withAlpha (0.72f));
    g.drawRoundedRectangle (pedal.reduced (1.0f), 16.0f, 1.5f);
    g.setColour (juce::Colour::fromRGB (255, 221, 142).withAlpha (0.32f));
    g.drawRoundedRectangle (pedal.reduced (5.0f), 12.0f, 1.0f);

    for (int i = 0; i < 95; ++i)
    {
        const auto x = pedal.getX() + 12.0f + static_cast<float> ((i * 37) % juce::jmax (1, static_cast<int> (pedal.getWidth() - 24.0f)));
        const auto y = pedal.getY() + 12.0f + static_cast<float> ((i * 61) % juce::jmax (1, static_cast<int> (pedal.getHeight() - 24.0f)));
        g.setColour ((i % 2 == 0 ? juce::Colour::fromRGB (88, 44, 18) : juce::Colour::fromRGB (255, 227, 154)).withAlpha (0.16f));
        g.fillRect (juce::Rectangle<float> (x, y, 1.2f, 1.2f));
    }

    drawScrew (g, pedal.getTopLeft() + juce::Point<float> (20.0f, 18.0f));
    drawScrew (g, pedal.getTopRight() + juce::Point<float> (-20.0f, 18.0f));
    drawScrew (g, pedal.getBottomLeft() + juce::Point<float> (20.0f, -18.0f));
    drawScrew (g, pedal.getBottomRight() + juce::Point<float> (-20.0f, -18.0f));

    g.setColour (juce::Colour::fromRGB (62, 34, 14).withAlpha (0.6f));
    g.drawVerticalLine (24, 84.0f, static_cast<float> (getHeight() - 96));
    g.drawVerticalLine (getWidth() - 24, 84.0f, static_cast<float> (getHeight() - 96));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (PluginConfig::editorMargin);

    auto header = area.removeFromTop (62);
    titleLabel.setBounds (header.removeFromTop (36));
    subtitleLabel.setBounds (header.removeFromTop (18));

    area.removeFromTop (8);
    layoutKnobRow (area.removeFromTop (92), { &stage1Knob, &driveKnob, &stage2Knob });

    area.removeFromTop (10);
    layoutKnobRow (area.removeFromTop (92), { &toneKnob, &analogKnob, &presenceKnob });

    area.removeFromTop (12);
    auto switchArea = area.removeFromTop (54);
    auto leftSwitch = switchArea.removeFromLeft (98);
    oversamplingLabel.setBounds (leftSwitch.removeFromTop (18));
    oversamplingBox.setBounds (leftSwitch.withSizeKeepingCentre (76, 24));

    auto meters = switchArea.reduced (2, 0);
    inputMeter.setBounds (meters.removeFromLeft (62));
    meters.removeFromLeft (8);
    outputMeter.setBounds (meters.removeFromLeft (62));
    meters.removeFromLeft (8);
    saturationMeter.setBounds (meters.removeFromLeft (62));

    area.removeFromTop (10);
    auto lowerKnobs = area.removeFromTop (92);
    lowerKnobs.reduce (36, 0);
    layoutKnobRow (lowerKnobs, { &mixKnob, &outputKnob });

    area.removeFromTop (18);
    auto footswitches = area.removeFromTop (78);
    autoGainButton.setBounds (footswitches.removeFromLeft (96).withSizeKeepingCentre (62, 62));
    footswitches.removeFromLeft (96);
    bypassButton.setBounds (footswitches.removeFromLeft (96).withSizeKeepingCentre (62, 62));
}

AudioPluginAudioProcessorEditor::Knob::Knob (const juce::String& title, const juce::String& suffix)
{
    nameLabel.setText (title, juce::dontSendNotification);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setFont (juce::FontOptions { 10.5f, juce::Font::bold });
    addAndMakeVisible (nameLabel);

    slider.setTextValueSuffix (suffix);
    addAndMakeVisible (slider);
}

void AudioPluginAudioProcessorEditor::Knob::resized()
{
    auto area = getLocalBounds();
    nameLabel.setBounds (area.removeFromBottom (16));
    slider.setBounds (area);
}

AudioPluginAudioProcessorEditor::Meter::Meter (const juce::String& labelText)
{
    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::FontOptions { 11.0f, juce::Font::bold });
    addAndMakeVisible (label);
}

void AudioPluginAudioProcessorEditor::Meter::setLevel (float newLevel) noexcept
{
    level = juce::jlimit (0.0f, 1.0f, newLevel);
    repaint();
}

void AudioPluginAudioProcessorEditor::Meter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced (5, 18).toFloat();

    g.setColour (juce::Colour::fromRGB (42, 25, 13).withAlpha (0.65f));
    g.fillRoundedRectangle (bounds, 4.0f);

    constexpr int numLeds = 5;
    const auto activeLeds = juce::roundToInt (level * static_cast<float> (numLeds));
    const auto ledWidth = bounds.getWidth() / static_cast<float> (numLeds);

    for (int i = 0; i < numLeds; ++i)
    {
        auto led = bounds.removeFromLeft (ledWidth).reduced (2.0f, 4.0f);
        const auto active = i < activeLeds;
        const auto hot = level > 0.82f && i >= numLeds - 1;

        g.setColour (active ? (hot ? juce::Colour::fromRGB (205, 38, 26) : juce::Colour::fromRGB (250, 203, 88))
                            : juce::Colour::fromRGB (49, 34, 20));
        g.fillEllipse (led);
        g.setColour (juce::Colour::fromRGB (22, 18, 14).withAlpha (0.65f));
        g.drawEllipse (led, 0.8f);
    }
}

void AudioPluginAudioProcessorEditor::Meter::resized()
{
    label.setBounds (getLocalBounds().removeFromBottom (18));
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    inputMeter.setLevel (processorRef.getInputMeterLevel());
    outputMeter.setLevel (processorRef.getOutputMeterLevel());
    saturationMeter.setLevel (processorRef.getSaturationMeterLevel());
}

void AudioPluginAudioProcessorEditor::configureComboBox()
{
    oversamplingLabel.setText ("Oversampling", juce::dontSendNotification);
    oversamplingLabel.setFont (juce::FontOptions { 10.5f, juce::Font::bold });
    oversamplingLabel.setJustificationType (juce::Justification::centred);

    oversamplingBox.addItem ("Off", 1);
    oversamplingBox.addItem ("2x", 2);
    oversamplingBox.addItem ("4x", 3);
    oversamplingBox.addItem ("8x", 4);
}

void AudioPluginAudioProcessorEditor::layoutKnobRow (juce::Rectangle<int> area, std::initializer_list<Knob*> knobs)
{
    const auto count = static_cast<int> (knobs.size());
    const auto width = (area.getWidth() - PluginConfig::controlGap * (count - 1)) / juce::jmax (1, count);

    for (auto* knob : knobs)
    {
        knob->setBounds (area.removeFromLeft (width));
        area.removeFromLeft (PluginConfig::controlGap);
    }
}

void AudioPluginAudioProcessorEditor::drawScrew (juce::Graphics& g, juce::Point<float> centre)
{
    auto bounds = juce::Rectangle<float> (centre.x - 5.0f, centre.y - 5.0f, 10.0f, 10.0f);
    juce::ColourGradient screw (juce::Colour::fromRGB (238, 210, 150),
                                bounds.getX(),
                                bounds.getY(),
                                juce::Colour::fromRGB (86, 54, 28),
                                bounds.getRight(),
                                bounds.getBottom(),
                                false);
    g.setGradientFill (screw);
    g.fillEllipse (bounds);
    g.setColour (juce::Colour::fromRGB (78, 44, 20));
    g.drawLine (bounds.getX() + 2.0f, centre.y, bounds.getRight() - 2.0f, centre.y, 1.0f);
}
