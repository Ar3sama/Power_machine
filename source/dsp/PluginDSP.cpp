#include "PluginDSP.h"

void PluginDSP::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    preparedChannels = juce::jmax (1, numChannels);
    preparedSamples = juce::jmax (1, samplesPerBlock);

    dryBuffer.setSize (preparedChannels, preparedSamples, false, false, true);
    oversampling.prepare (preparedChannels, preparedSamples);
    toneShaper.prepare (sampleRate, preparedSamples, preparedChannels);
    reset();
}

void PluginDSP::reset()
{
    dryBuffer.clear();
    oversampling.reset();
    toneShaper.reset();
    saturator.reset();
    autoGain.reset();
    lastInputLevel = 0.0f;
    lastOutputLevel = 0.0f;
    lastSaturationAmount = 0.0f;
}

void PluginDSP::processBlock (juce::AudioBuffer<float>& buffer, const Settings& settings)
{
    const auto numChannels = juce::jmin (buffer.getNumChannels(), preparedChannels);
    const auto numSamples = buffer.getNumSamples();

    if (numChannels <= 0 || numSamples <= 0 || numSamples > preparedSamples)
        return;

    for (int channel = 0; channel < numChannels; ++channel)
        dryBuffer.copyFrom (channel, 0, buffer, channel, 0, numSamples);

    lastInputLevel = calculatePeakLevel (buffer);

    const auto inputGain = juce::Decibels::decibelsToGain (settings.inputDriveDb);
    buffer.applyGain (inputGain);

    auto block = juce::dsp::AudioBlock<float> (buffer).getSubBlock (0, static_cast<size_t> (numSamples));
    toneShaper.setPreTone (settings.tone);
    toneShaper.processPre (block);

    auto oversampledBlock = oversampling.processSamplesUp (block, settings.oversamplingMode);
    saturator.process (oversampledBlock,
                       settings.stage1Drive,
                       settings.stage2Drive,
                       settings.analogCharacter,
                       lastSaturationAmount);
    oversampling.processSamplesDown (block, settings.oversamplingMode);

    toneShaper.setPostTone (settings.tone, settings.presence);
    toneShaper.processPost (block);

    const auto automaticGain = autoGain.process (buffer, settings.autoGainEnabled, settings.inputDriveDb);
    const auto outputGain = juce::Decibels::decibelsToGain (settings.outputGainDb) * automaticGain;
    const auto wetAmount = juce::jlimit (0.0f, 1.0f, settings.mix);
    const auto dryAmount = 1.0f - wetAmount;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* wet = buffer.getWritePointer (channel);
        const auto* dry = dryBuffer.getReadPointer (channel);

        for (int sample = 0; sample < numSamples; ++sample)
            wet[sample] = (wet[sample] * outputGain * wetAmount) + (dry[sample] * dryAmount);
    }

    lastOutputLevel = calculatePeakLevel (buffer);
}

float PluginDSP::calculatePeakLevel (const juce::AudioBuffer<float>& buffer) noexcept
{
    auto peak = 0.0f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        peak = juce::jmax (peak, buffer.getMagnitude (channel, 0, buffer.getNumSamples()));

    return juce::jlimit (0.0f, 1.0f, peak);
}
