#pragma once

#include <juce_core/juce_core.h>

class SoftClipper
{
public:
    float processSample (float sample, float analogCharacter) const noexcept;
};
