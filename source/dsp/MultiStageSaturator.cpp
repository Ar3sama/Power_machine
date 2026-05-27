#include "MultiStageSaturator.h"

namespace
{
    float driveFromPercent (float value, float minimum, float maximum) noexcept
    {
        const auto normalised = juce::jlimit (0.0f, 1.0f, value);
        return minimum * std::pow (maximum / minimum, normalised);
    }
}

void MultiStageSaturator::reset()
{
    stage1.reset();
    stage2.reset();
}

void MultiStageSaturator::process (juce::dsp::AudioBlock<float>& block,
                                   float stage1Drive,
                                   float stage2Drive,
                                   float analogCharacter,
                                   float& saturationAmount)
{
    const auto channels = block.getNumChannels();
    const auto samples = block.getNumSamples();
    const auto stage1Gain = driveFromPercent (stage1Drive, 1.0f, 3.2f);
    const auto stage2Gain = driveFromPercent (stage2Drive, 1.0f, 5.0f);
    const auto character = juce::jlimit (0.0f, 1.0f, analogCharacter);

    auto accumulatedDifference = 0.0f;

    for (size_t channel = 0; channel < channels; ++channel)
    {
        auto* data = block.getChannelPointer (channel);

        for (size_t sample = 0; sample < samples; ++sample)
        {
            const auto input = data[sample];
            auto value = stage1.processSample (input, stage1Gain, character * 0.65f);

            // A tiny interstage trim keeps the second stage thick instead of brittle.
            value = stage2.processSample (value * (1.0f + 0.35f * stage2Drive), stage2Gain, character);
            value = softClipper.processSample (value * 1.08f, character);

            accumulatedDifference += std::abs (value - input);
            data[sample] = value;
        }
    }

    const auto divisor = juce::jmax (1.0f, static_cast<float> (channels * samples));
    saturationAmount = juce::jlimit (0.0f, 1.0f, accumulatedDifference / divisor * 3.0f);
}
