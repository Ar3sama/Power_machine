#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "AutoGainCompensation.h"
#include "MultiStageSaturator.h"
#include "OversamplingProcessor.h"
#include "ToneShaper.h"

class PluginDSP
{
public:
    struct Settings
    {
        float inputDriveDb = 0.0f;
        float stage1Drive = 0.0f;
        float stage2Drive = 0.0f;
        float tone = 0.0f;
        float presence = 0.0f;
        float mix = 1.0f;
        float outputGainDb = 0.0f;
        OversamplingProcessor::Mode oversamplingMode = OversamplingProcessor::Mode::x4;
        float analogCharacter = 0.0f;
        bool autoGainEnabled = true;
    };

    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    void processBlock (juce::AudioBuffer<float>& buffer, const Settings& settings);

    float getLastInputLevel() const noexcept { return lastInputLevel; }
    float getLastOutputLevel() const noexcept { return lastOutputLevel; }
    float getLastSaturationAmount() const noexcept { return lastSaturationAmount; }

private:
    static float calculatePeakLevel (const juce::AudioBuffer<float>& buffer) noexcept;

    juce::AudioBuffer<float> dryBuffer;
    OversamplingProcessor oversampling;
    ToneShaper toneShaper;
    MultiStageSaturator saturator;
    AutoGainCompensation autoGain;

    int preparedChannels = 0;
    int preparedSamples = 0;
    float lastInputLevel = 0.0f;
    float lastOutputLevel = 0.0f;
    float lastSaturationAmount = 0.0f;
};
