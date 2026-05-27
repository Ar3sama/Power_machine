#pragma once

#include <juce_dsp/juce_dsp.h>

class SaturationStage
{
public:
    enum class Mode
    {
        consoleTape,
        tubeTransformer
    };

    explicit SaturationStage (Mode stageMode);

    void reset();
    float processSample (float sample, float drive, float analogCharacter) noexcept;

private:
    Mode mode;
    float envelope = 0.0f;
};
