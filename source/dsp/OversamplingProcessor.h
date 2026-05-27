#pragma once

#include <juce_dsp/juce_dsp.h>

class OversamplingProcessor
{
public:
    enum class Mode
    {
        off = 0,
        x2,
        x4,
        x8
    };

    void prepare (int numChannels, int samplesPerBlock);
    void reset();

    juce::dsp::AudioBlock<float> processSamplesUp (juce::dsp::AudioBlock<float>& inputBlock, Mode mode);
    void processSamplesDown (juce::dsp::AudioBlock<float>& outputBlock, Mode mode);

private:
    using Oversampler = juce::dsp::Oversampling<float>;

    Oversampler* getOversampler (Mode mode) noexcept;

    std::unique_ptr<Oversampler> oversampling2x;
    std::unique_ptr<Oversampler> oversampling4x;
    std::unique_ptr<Oversampler> oversampling8x;
};
