#pragma once

#include "PluginProcessor.h"
#include "ui/lookandfeel/PluginLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
                                           , private juce::Timer
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Knob final : public juce::Component
    {
    public:
        Knob (const juce::String& title, const juce::String& suffix);

        void resized() override;
        juce::Slider slider;

    private:
        juce::Label nameLabel;
        juce::Label valueLabel;
    };

    class Meter final : public juce::Component
    {
    public:
        explicit Meter (const juce::String& labelText);
        void setLevel (float newLevel) noexcept;
        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        juce::Label label;
        float level = 0.0f;
    };

    void timerCallback() override;
    void configureComboBox();
    void layoutKnobRow (juce::Rectangle<int> area, std::initializer_list<Knob*> knobs);
    void drawScrew (juce::Graphics& g, juce::Point<float> centre);

    AudioPluginAudioProcessor& processorRef;

    PluginLookAndFeel lookAndFeel;
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label oversamplingLabel;

    Knob driveKnob { "GAIN", " dB" };
    Knob stage1Knob { "STAGE A", "%" };
    Knob stage2Knob { "STAGE B", "%" };
    Knob toneKnob { "TONE", "" };
    Knob presenceKnob { "PRES", "%" };
    Knob analogKnob { "STACK", "%" };
    Knob mixKnob { "BLEND", "%" };
    Knob outputKnob { "MASTER", " dB" };

    juce::ComboBox oversamplingBox;
    Meter inputMeter { "IN" };
    Meter outputMeter { "OUT" };
    Meter saturationMeter { "SAT" };

    juce::TextButton autoGainButton { "Auto Gain" };
    juce::TextButton bypassButton { "Bypass" };

    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> stage1Attachment;
    std::unique_ptr<SliderAttachment> stage2Attachment;
    std::unique_ptr<SliderAttachment> toneAttachment;
    std::unique_ptr<SliderAttachment> presenceAttachment;
    std::unique_ptr<SliderAttachment> analogAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<ComboBoxAttachment> oversamplingAttachment;
    std::unique_ptr<ButtonAttachment> autoGainAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
