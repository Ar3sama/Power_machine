#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace Parameters
{
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
                defaultValue));
        };

        const auto addPercentParameter = [&parameters] (const char* id,
                                                        const juce::String& name,
                                                        float defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
                id,
                name,
                juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01f },
                defaultValue));
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


        // Register plugin parameters here.
        // Keep IDs in ParameterIDs.h and connect UI controls with APVTS attachments.

        addBoolParameter (ParameterIDs::bypass,"Bypass", false);

        return { parameters.begin(), parameters.end() };
    }
}
