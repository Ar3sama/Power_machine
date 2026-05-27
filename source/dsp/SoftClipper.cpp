#include "SoftClipper.h"

float SoftClipper::processSample (float sample, float analogCharacter) const noexcept
{
    const auto threshold = 0.82f - 0.08f * juce::jlimit (0.0f, 1.0f, analogCharacter);
    const auto sign = sample < 0.0f ? -1.0f : 1.0f;
    const auto magnitude = std::abs (sample);

    if (magnitude <= threshold)
        return sample;

    const auto excess = magnitude - threshold;
    const auto knee = std::tanh (excess / (1.0f - threshold));
    return sign * (threshold + (1.0f - threshold) * knee);
}
