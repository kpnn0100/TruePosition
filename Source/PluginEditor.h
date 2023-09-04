/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Slider/XyzPad.h"
#include "Button/ToggleButtonText.h"
#include "Slider/OneDimension/CleanSlider.h"
#include "LookAndFeelOfTruePosition.h"
#include "colorTheme.h"
using namespace juce;
using namespace Gui;
//==============================================================================
/**
*/
class TruePositionAudioProcessorEditor  : public juce::AudioProcessorEditor
                                        , public Slider::Listener
                                        , public Button::Listener
                                        , public XyzPadListener
{
public:
    TruePositionAudioProcessorEditor (TruePositionAudioProcessor&);
    ~TruePositionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sourceValueChanged(Coordinate source) override;
    void updatePositioner();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TruePositionAudioProcessor& audioProcessor;
    LookAndFeelOfTruePosition mLookAndFeel;
    XyzPad spaceSlider;
    Slider dimesionSliderList[3];
    Slider sizeSliderList[3];
    CleanSlider wetSlider;
    Slider drySlider;
    ToggleButtonText toggleButtonText;
    virtual void sliderValueChanged(Slider* slider) override;
    virtual void buttonStateChanged(Button* button) override;
    virtual void buttonClicked(Button*);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TruePositionAudioProcessorEditor)
};
