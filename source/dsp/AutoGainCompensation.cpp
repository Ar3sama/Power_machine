#include "AutoGainCompensation.h"

namespace
{
    float calculateRms (const juce::AudioBuffer<float>& buffer) noexcept
    {
        auto sum = 0.0f;
        auto count = 0;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const auto* data = buffer.getReadPointer (channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                sum += data[sample] * data[sample];
                ++count;
            }
        }

        return count > 0 ? std::sqrt (sum / static_cast<float> (count)) : 0.0f;
    }
}

void AutoGainCompensation::reset()
{
    smoothedGain = 1.0f;
}

float AutoGainCompensation::process (const juce::AudioBuffer<float>& buffer, bool enabled, float inputDriveDb) noexcept
{
    if (! enabled)
    {
        smoothedGain += 0.01f * (1.0f - smoothedGain);
        return smoothedGain;
    }

    const auto rms = calculateRms (buffer);
    const auto driveCompensation = juce::Decibels::decibelsToGain (-0.45f * inputDriveDb);
    const auto levelCompensation = rms > 0.0001f ? juce::jlimit (0.35f, 1.4f, 0.18f / rms) : 1.0f;
    const auto targetGain = juce::jlimit (0.2f, 1.5f, driveCompensation * std::sqrt (levelCompensation));

    smoothedGain += 0.02f * (targetGain - smoothedGain);
    return smoothedGain;
}
