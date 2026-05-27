#include "ToneShaper.h"

void ToneShaper::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;

    const juce::dsp::ProcessSpec spec { sampleRate,
                                        static_cast<juce::uint32> (juce::jmax (1, samplesPerBlock)),
                                        static_cast<juce::uint32> (numChannels) };
    preHighPass.prepare (spec);
    preTilt.prepare (spec);
    postTilt.prepare (spec);
    presencePeak.prepare (spec);

    reset();
    setPreTone (0.0f);
    setPostTone (0.0f, 0.0f);
}

void ToneShaper::reset()
{
    preHighPass.reset();
    preTilt.reset();
    postTilt.reset();
    presencePeak.reset();
}

void ToneShaper::setPreTone (float tone)
{
    const auto normalisedTone = juce::jlimit (-1.0f, 1.0f, tone);
    const auto highPassFrequency = 18.0f + 42.0f * juce::jmax (0.0f, normalisedTone);
    const auto preGainDb = 1.2f * normalisedTone;

    *preHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, highPassFrequency, 0.707f);
    *preTilt.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 2600.0f, 0.65f,
                                                                          juce::Decibels::decibelsToGain (preGainDb));
}

void ToneShaper::setPostTone (float tone, float presence)
{
    const auto normalisedTone = juce::jlimit (-1.0f, 1.0f, tone);
    const auto shelfGainDb = 4.5f * normalisedTone;
    const auto presenceGainDb = 5.0f * juce::jlimit (0.0f, 1.0f, presence);

    *postTilt.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 4200.0f, 0.7f,
                                                                           juce::Decibels::decibelsToGain (shelfGainDb));
    *presencePeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 5400.0f, 0.85f,
                                                                                juce::Decibels::decibelsToGain (presenceGainDb));
}

void ToneShaper::processPre (juce::dsp::AudioBlock<float>& block)
{
    const juce::dsp::ProcessContextReplacing<float> context (block);
    preHighPass.process (context);
    preTilt.process (context);
}

void ToneShaper::processPost (juce::dsp::AudioBlock<float>& block)
{
    const juce::dsp::ProcessContextReplacing<float> context (block);
    postTilt.process (context);
    presencePeak.process (context);
}
