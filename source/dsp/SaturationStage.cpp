#include "SaturationStage.h"

namespace
{
    constexpr float envelopeAttack = 0.015f;
    constexpr float envelopeRelease = 0.0015f;

    float sigmoid (float value) noexcept
    {
        return value / (1.0f + std::abs (value));
    }
}

SaturationStage::SaturationStage (Mode stageMode)
    : mode (stageMode)
{
}

void SaturationStage::reset()
{
    envelope = 0.0f;
}

float SaturationStage::processSample (float sample, float drive, float analogCharacter) noexcept
{
    const auto level = std::abs (sample);
    const auto coefficient = level > envelope ? envelopeAttack : envelopeRelease;
    envelope += coefficient * (level - envelope);

    const auto character = juce::jlimit (0.0f, 1.0f, analogCharacter);
    const auto levelPush = 1.0f + 0.35f * envelope;
    const auto asymmetry = character * (mode == Mode::consoleTape ? 0.08f : 0.16f);

    auto driven = sample * drive * levelPush;

    // The small DC offset before shaping creates controlled even harmonics.
    driven += asymmetry;

    if (mode == Mode::consoleTape)
    {
        const auto rounded = std::tanh (driven);
        return (rounded - std::tanh (asymmetry)) / juce::jmax (0.25f, drive);
    }

    const auto tanhPart = std::tanh (driven * 0.85f);
    const auto sigmoidPart = sigmoid (driven * 1.45f);
    const auto blended = 0.62f * tanhPart + 0.38f * sigmoidPart;

    return (blended - (0.62f * std::tanh (asymmetry * 0.85f) + 0.38f * sigmoid (asymmetry * 1.45f)))
           / juce::jmax (0.25f, drive);
}
