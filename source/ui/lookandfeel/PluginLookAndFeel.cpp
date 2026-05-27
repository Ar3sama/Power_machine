#include "PluginLookAndFeel.h"

PluginLookAndFeel::PluginLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGB (14, 12, 10));
    setColour (juce::Label::textColourId, juce::Colour::fromRGB (36, 26, 16));
    setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (244, 224, 176));
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (80, 45, 22));
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB (24, 20, 18));
    setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB (24, 22, 20));
    setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromRGB (230, 190, 112));
    setColour (juce::TextButton::textColourOffId, juce::Colour::fromRGB (238, 226, 198));
    setColour (juce::TextButton::textColourOnId, juce::Colour::fromRGB (18, 18, 18));
    setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB (24, 21, 18));
    setColour (juce::ComboBox::textColourId, juce::Colour::fromRGB (242, 225, 186));
    setColour (juce::ComboBox::outlineColourId, juce::Colour::fromRGB (90, 56, 24));
}

void PluginLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          float sliderPos,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                               static_cast<float> (width), static_cast<float> (height)).reduced (8.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto knobBounds = juce::Rectangle<float> (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    g.setColour (juce::Colour::fromRGB (92, 56, 26).withAlpha (0.55f));
    g.fillEllipse (knobBounds.translated (0.0f, 2.0f));

    juce::ColourGradient knobGradient (juce::Colour::fromRGB (72, 70, 64),
                                       knobBounds.getX(),
                                       knobBounds.getY(),
                                       juce::Colour::fromRGB (15, 14, 13),
                                       knobBounds.getRight(),
                                       knobBounds.getBottom(),
                                       false);
    g.setGradientFill (knobGradient);
    g.fillEllipse (knobBounds);

    g.setColour (juce::Colour::fromRGB (194, 166, 106));
    g.drawEllipse (knobBounds.reduced (1.0f), 1.5f);
    g.setColour (juce::Colour::fromRGB (28, 24, 20));
    g.drawEllipse (knobBounds.reduced (5.0f), 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.4f, -radius + 8.0f, 2.8f, radius * 0.46f, 1.4f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (juce::Colour::fromRGB (244, 229, 190));
    g.fillPath (pointer);

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, radius + 5.0f, radius + 5.0f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (juce::Colour::fromRGB (75, 38, 16).withAlpha (0.62f));
    g.strokePath (valueArc, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void PluginLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                              juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    auto colour = button.getToggleState() ? findColour (juce::TextButton::buttonOnColourId) : backgroundColour;

    if (shouldDrawButtonAsDown)
        colour = colour.darker (0.15f);
    else if (shouldDrawButtonAsHighlighted)
        colour = colour.brighter (0.08f);

    if (button.getProperties().contains ("footSwitch"))
    {
        const auto diameter = juce::jmin (bounds.getWidth(), bounds.getHeight());
        const auto switchBounds = bounds.withSizeKeepingCentre (diameter, diameter).reduced (3.0f);

        juce::ColourGradient metal (juce::Colour::fromRGB (248, 242, 226),
                                    switchBounds.getX(),
                                    switchBounds.getY(),
                                    juce::Colour::fromRGB (86, 82, 76),
                                    switchBounds.getRight(),
                                    switchBounds.getBottom(),
                                    false);
        g.setGradientFill (metal);
        g.fillEllipse (switchBounds);
        g.setColour (juce::Colour::fromRGB (30, 28, 25));
        g.drawEllipse (switchBounds, 1.4f);

        if (button.getToggleState())
        {
            g.setColour (juce::Colour::fromRGB (255, 219, 128).withAlpha (0.85f));
            g.drawEllipse (switchBounds.reduced (6.0f), 2.2f);
        }

        return;
    }

    g.setColour (colour);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (button.getToggleState() ? juce::Colour::fromRGB (110, 64, 24) : juce::Colour::fromRGB (118, 82, 38));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}

void PluginLookAndFeel::drawComboBox (juce::Graphics& g,
                                      int width,
                                      int height,
                                      bool isButtonDown,
                                      int buttonX,
                                      int buttonY,
                                      int buttonW,
                                      int buttonH,
                                      juce::ComboBox& box)
{
    juce::ignoreUnused (isButtonDown, buttonX, buttonY, buttonW, buttonH);

    const auto bounds = juce::Rectangle<float> (0.5f, 0.5f, static_cast<float> (width) - 1.0f, static_cast<float> (height) - 1.0f);
    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    juce::Path arrow;
    arrow.addTriangle (static_cast<float> (width - 17), static_cast<float> (height / 2 - 3),
                       static_cast<float> (width - 7), static_cast<float> (height / 2 - 3),
                       static_cast<float> (width - 12), static_cast<float> (height / 2 + 4));
    g.setColour (juce::Colour::fromRGB (232, 195, 120));
    g.fillPath (arrow);
}
