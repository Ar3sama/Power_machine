#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class AutoGainCompensation
{
public:
    void reset();
    float process (const juce::AudioBuffer<float>& buffer, bool enabled, float inputDriveDb) noexcept;

private:
    float smoothedGain = 1.0f;
};
