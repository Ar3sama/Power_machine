#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace Parameters
{
    namespace
    {
        juce::String dbToText (float value, int)
        {
            return juce::String (value, 1) + " dB";
        }

        float textToDb (const juce::String& text)
        {
            return text.retainCharacters ("-0123456789.").getFloatValue();
        }

        juce::String percentToText (float value, int)
        {
            return juce::String (value, 0) + "%";
        }

        float textToPercent (const juce::String& text)
        {
            return text.retainCharacters ("0123456789.").getFloatValue();
        }
    }

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

        const auto addDbParameter = [&parameters] (const char* id,
                                                   const juce::String& name,
                                                   float defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
                id,
                name,
                juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
                defaultValue,
                juce::String(),
                juce::AudioProcessorParameter::genericParameter,
                dbToText,
                textToDb));
        };

        const auto addPercentParameter = [&parameters] (const char* id,
                                                        const juce::String& name,
                                                        float defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
                id,
                name,
                juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01f },
                defaultValue,
                juce::String(),
                juce::AudioProcessorParameter::genericParameter,
                percentToText,
                textToPercent));
        };

        const auto addBoolParameter = [&parameters] (const char* id,
                                                    const juce::String& name,
                                                    bool defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterBool> (
                id,
                name,
                defaultValue));
        };

        parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::inputDrive,
            "Input Drive",
            juce::NormalisableRange<float> { 0.0f, 36.0f, 0.01f, 0.7f },
            6.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            dbToText,
            textToDb));

        parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::stage1Drive,
            "Stage 1 Drive",
            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01f },
            35.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            percentToText,
            textToPercent));

        parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::stage2Drive,
            "Stage 2 Drive",
            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01f },
            25.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            percentToText,
            textToPercent));

        parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::tone,
            "Tone",
            juce::NormalisableRange<float> { -100.0f, 100.0f, 0.01f },
            0.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [] (float value, int) { return juce::String (value, 0); },
            [] (const juce::String& text) { return text.getFloatValue(); }));

        addPercentParameter (ParameterIDs::presence, "Presence", 20.0f);
        addPercentParameter (ParameterIDs::mix, "Mix", 100.0f);
        addDbParameter (ParameterIDs::outputGain, "Output Gain", 0.0f);

        parameters.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParameterIDs::oversampling,
            "Oversampling",
            juce::StringArray { "Off", "2x", "4x", "8x" },
            2));

        addPercentParameter (ParameterIDs::analogCharacter, "Analog Character", 25.0f);
        addBoolParameter (ParameterIDs::autoGain, "Auto Gain", true);
        addBoolParameter (ParameterIDs::bypass, "Bypass", false);

        return { parameters.begin(), parameters.end() };
    }
}
