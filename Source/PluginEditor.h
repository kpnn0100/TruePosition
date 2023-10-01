/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <sstream>
#include "PluginProcessor.h"
#include "Slider/XyzPad.h"
#include "Button/ToggleButtonText.h"
#include "Slider/OneDimension/CleanSlider.h"
#include "Icon/BoxIcon.h"
#include "Shape/RectangleComponent.h"
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
    AudioProcessorValueTreeState& parameters;
    TooltipWindow helper;
    TruePositionAudioProcessor& audioProcessor;
    LookAndFeelOfTruePosition mLookAndFeel;
    Label mSizeTextBox[3];
    Label mMixName[3];
    Label mDecayName;
    BoxIcon mBoxIcon[3][2];
    Component mWetAndDrySection;
    Component mSizeSliderSection;
    XyzPad spaceSlider;
    Slider dimesionSliderList[3];
    Slider sizeSliderList[3];
    Slider wetSlider;
    Slider drySlider;
    Slider reverbSlider;
    Slider decaySlider;
    ToggleButtonText keepGainButton;
    SliderParameterAttachment sizeAttachmentX, sizeAttachmentY, sizeAttachmentZ,
        posAttachmentX, posAttachmentY, posAttachmentZ,
        wetAttachment, dryAttachment,reverbAttachment,decayAttachment;
    ButtonParameterAttachment keepGainAttachment;
    RectangleComponent backgroundForSizeSlider,backgroundForWetSection,backgroundOfPad;
    virtual void sliderValueChanged(Slider* slider) override;
    virtual void buttonStateChanged(Button* button) override;
    virtual void buttonClicked(Button*);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TruePositionAudioProcessorEditor)
};
