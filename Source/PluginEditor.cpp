/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TruePositionAudioProcessorEditor::TruePositionAudioProcessorEditor (TruePositionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 400);
    Point<int> topLeftOfEverything(25, 25);
    Point<int> xyPadSize(350, 350);
    int gap = 25;
    int sliderWidth = getWidth() - topLeftOfEverything.getX() - xyPadSize.getX() - gap * 2;
    int sliderHeight = 50;
    Point<int> topLeftOfSlider(topLeftOfEverything.getX() + xyPadSize.getX() + gap, topLeftOfEverything.getY());
    Point<int> boxSize(sliderWidth/3, sliderHeight);
    

    spaceSlider.setSize(xyPadSize.getX(), xyPadSize.getY());
    spaceSlider.setTopLeftPosition(topLeftOfEverything);

    spaceSlider.setBackgroundColor(ColorTheme::backgroundColor);
    spaceSlider.setBorderColor(ColorTheme::whiteColor.darker(0.1));
    spaceSlider.setAxisColor(ColorTheme::mainColor[0], ColorTheme::mainColor[1], ColorTheme::mainColor[2]);
    spaceSlider.registerSource(this);
    
    toggleButtonText.addListener(this);


    for (int i = 0; i < 3; i++)
    {
        sizeSliderList[i].setLookAndFeel(&mLookAndFeel);
        sizeSliderList[i].setTextValueSuffix("m");
        sizeSliderList[i].setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, sliderWidth, sliderHeight/1.5);
        sizeSliderList[i].addListener(this);
        dimesionSliderList[i].setRange(Range<double>(-1.0, 1.0), 0.01);
        dimesionSliderList[i].setValue(0.1);
        spaceSlider.registerSlider(&dimesionSliderList[i],i);
        sizeSliderList[i].setSize(sliderWidth, sliderHeight);
        sizeSliderList[i].setRange(1.0f,40.0f);
        sizeSliderList[i].setValue(2.0f);
        sizeSliderList[i].setCentrePosition(topLeftOfSlider + Point<int>(sliderWidth/2, gap/2)
            + Point<int>(0, gap / 2 * i) + Point<int>(0, sliderHeight*i));
        sizeSliderList[i].setColour(Slider::trackColourId,ColorTheme::mainColor[i].darker(0.2));
        sizeSliderList[i].setColour(Slider::thumbColourId, ColorTheme::mainColor[i]);
        sizeSliderList[i].setNumDecimalPlacesToDisplay(2);
        addAndMakeVisible(sizeSliderList[i]);
    }
    toggleButtonText.setSize(100, 50);
    toggleButtonText.setTitle("Keep gain");
    toggleButtonText.setTopLeftPosition({ spaceSlider.getWidth() + 50,8 * 40 });
    
    wetSlider.setTopLeftPosition({ spaceSlider.getWidth() + 50,7 * 40 });
    wetSlider.setSize(150, 20);
    wetSlider.setRange(0.0, 1.0);
    drySlider.setSize(150, 20);
    drySlider.setRange(0.0, 1.0);
    drySlider.setTopLeftPosition({ spaceSlider.getWidth() + 50,6 * 40 });
    audioProcessor.mDrySlider = &drySlider;
    audioProcessor.mWetSlider = &wetSlider;

    updatePositioner();
    addAndMakeVisible(wetSlider);
    addAndMakeVisible(drySlider);
    addAndMakeVisible(spaceSlider);
    addAndMakeVisible(toggleButtonText);

}

TruePositionAudioProcessorEditor::~TruePositionAudioProcessorEditor()
{
}

//==============================================================================
void TruePositionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (ColorTheme::backgroundColor);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::right, 1);
}

void TruePositionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void TruePositionAudioProcessorEditor::sourceValueChanged(Coordinate source)
{
    audioProcessor.mSource = source;
}

void TruePositionAudioProcessorEditor::updatePositioner()
{
    spaceSlider.setBoxSize(sizeSliderList[0].getValue(), sizeSliderList[1].getValue(), sizeSliderList[2].getValue());
    spaceSlider.setDestination(sizeSliderList[0].getValue() / 2, sizeSliderList[1].getValue() / 2, sizeSliderList[2].getValue() / 2);
    audioProcessor.mDestination = Coordinate(sizeSliderList[0].getValue() / 2, sizeSliderList[1].getValue() / 2, sizeSliderList[2].getValue() / 2);
    audioProcessor.mRoomSize = Coordinate(sizeSliderList[0].getValue(), sizeSliderList[1].getValue(), sizeSliderList[2].getValue());
}

void TruePositionAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    updatePositioner();
}

void TruePositionAudioProcessorEditor::buttonStateChanged(Button* button)
{
    audioProcessor.mKeepGain = toggleButtonText.getToggleState();
}

void TruePositionAudioProcessorEditor::buttonClicked(Button*)
{
}
