#pragma once

#include <juce_dsp/juce_dsp.h>

class ToneShaper
{
public:
    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    void setPreTone (float tone);
    void setPostTone (float tone, float presence);
    void processPre (juce::dsp::AudioBlock<float>& block);
    void processPost (juce::dsp::AudioBlock<float>& block);

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;

    double currentSampleRate = 44100.0;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> preHighPass;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> preTilt;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> postTilt;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> presencePeak;
};
