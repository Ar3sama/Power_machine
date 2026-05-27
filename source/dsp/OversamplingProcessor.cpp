#include "OversamplingProcessor.h"

void OversamplingProcessor::prepare (int numChannels, int samplesPerBlock)
{
    const auto channels = static_cast<size_t> (juce::jmax (1, numChannels));

    oversampling2x = std::make_unique<Oversampler> (channels, 1, Oversampler::filterHalfBandPolyphaseIIR, true, false);
    oversampling4x = std::make_unique<Oversampler> (channels, 2, Oversampler::filterHalfBandPolyphaseIIR, true, false);
    oversampling8x = std::make_unique<Oversampler> (channels, 3, Oversampler::filterHalfBandPolyphaseIIR, true, false);

    oversampling2x->initProcessing (static_cast<size_t> (samplesPerBlock));
    oversampling4x->initProcessing (static_cast<size_t> (samplesPerBlock));
    oversampling8x->initProcessing (static_cast<size_t> (samplesPerBlock));

    reset();
}

void OversamplingProcessor::reset()
{
    if (oversampling2x != nullptr)
        oversampling2x->reset();

    if (oversampling4x != nullptr)
        oversampling4x->reset();

    if (oversampling8x != nullptr)
        oversampling8x->reset();
}

juce::dsp::AudioBlock<float> OversamplingProcessor::processSamplesUp (juce::dsp::AudioBlock<float>& inputBlock, Mode mode)
{
    if (auto* oversampler = getOversampler (mode))
        return oversampler->processSamplesUp (inputBlock);

    return inputBlock;
}

void OversamplingProcessor::processSamplesDown (juce::dsp::AudioBlock<float>& outputBlock, Mode mode)
{
    if (auto* oversampler = getOversampler (mode))
        oversampler->processSamplesDown (outputBlock);
}

OversamplingProcessor::Oversampler* OversamplingProcessor::getOversampler (Mode mode) noexcept
{
    switch (mode)
    {
        case Mode::x2: return oversampling2x.get();
        case Mode::x4: return oversampling4x.get();
        case Mode::x8: return oversampling8x.get();
        case Mode::off: break;
    }

    return nullptr;
}
