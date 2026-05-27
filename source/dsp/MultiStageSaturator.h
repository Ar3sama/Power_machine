#pragma once

#include "SaturationStage.h"
#include "SoftClipper.h"

#include <juce_dsp/juce_dsp.h>

class MultiStageSaturator
{
public:
    void reset();
    void process (juce::dsp::AudioBlock<float>& block,
                  float stage1Drive,
                  float stage2Drive,
                  float analogCharacter,
                  float& saturationAmount);

private:
    SaturationStage stage1 { SaturationStage::Mode::consoleTape };
    SaturationStage stage2 { SaturationStage::Mode::tubeTransformer };
    SoftClipper softClipper;
};
