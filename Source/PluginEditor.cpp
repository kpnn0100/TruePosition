/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TruePositionAudioProcessorEditor::TruePositionAudioProcessorEditor(TruePositionAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    parameters(p.getParameterTree()),
    sizeAttachmentX(*parameters.getParameter("sizeX"), sizeSliderList[0]),
    sizeAttachmentY(*parameters.getParameter("sizeY"), sizeSliderList[1]),
    sizeAttachmentZ(*parameters.getParameter("sizeZ"), sizeSliderList[2]),
  
    posAttachmentX(*parameters.getParameter("posX"), dimesionSliderList[0]),
    posAttachmentY(*parameters.getParameter("posY"), dimesionSliderList[1]),
    posAttachmentZ(*parameters.getParameter("posZ"), dimesionSliderList[2]),

    wetAttachment(*parameters.getParameter("wet"), wetSlider),
    dryAttachment(*parameters.getParameter("dry"), drySlider),
    reverbAttachment(*parameters.getParameter("reverb"), reverbSlider),
    decayAttachment(*parameters.getParameter("decay"), decaySlider),
    keepGainAttachment(*parameters.getParameter("keepGain"), keepGainButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 340);
    addAndMakeVisible(helper);
    Point<int> topLeftOfEverything(5, 15);
    Point<int> xyPadSize(308, 308);
    int gap = 15;
    int sliderWidth = getWidth() - topLeftOfEverything.getX() - xyPadSize.getX() - gap * 2;
    int sliderHeight = 25;


    float keepGainWidth = 80.0;
    float cornerSize = 5.0;
    auto componentBackgroundColor = ColorTheme::backgroundColor.brighter(0.02);
    spaceSlider.setSize(xyPadSize.getX()-gap, xyPadSize.getY()-gap);
    spaceSlider.setCentrePosition(topLeftOfEverything + Point<int>(spaceSlider.getWidth()/2+gap/2, spaceSlider.getHeight()/2+gap/2));
    Point<int> topLeftOfSlider(spaceSlider.getRight() + gap*3/2, topLeftOfEverything.getY());
    Point<int> boxSize(sliderWidth / 3, sliderHeight * 2 / 3);
    spaceSlider.setBackgroundColor(ColorTheme::backgroundColor);
    spaceSlider.setBorderColor(ColorTheme::whiteColor.darker(0.1));
    spaceSlider.setAxisColor(ColorTheme::mainColor[0], ColorTheme::mainColor[1], ColorTheme::mainColor[2]);
    spaceSlider.registerSource(this);
    spaceSlider.setTooltip("Press Shift to slide front or back");

    keepGainButton.addListener(this);
    mSizeSliderSection.setSize(sliderWidth, 3 * sliderHeight + 3 * boxSize.y + gap * 5);
    mSizeSliderSection.setTopLeftPosition(topLeftOfSlider);

    float mixHeight = getHeight() - mSizeSliderSection.getBottom() - gap*2.5;
    float rotaryHeight = mixHeight*4/5;
    mWetAndDrySection.setSize(mSizeSliderSection.getBounds().getWidth(), mixHeight);
    mWetAndDrySection.setCentrePosition(mSizeSliderSection.getBounds().getCentre().getX(), mSizeSliderSection.getBottom() + gap*3/2 + mWetAndDrySection.getHeight() / 2);
    addAndMakeVisible(backgroundForSizeSlider);
    addAndMakeVisible(backgroundForWetSection);
    addAndMakeVisible(backgroundOfPad);

    backgroundOfPad.setBounds(spaceSlider.getBounds().withSizeKeepingCentre(spaceSlider.getWidth()+gap, spaceSlider.getHeight()+2*gap));
    backgroundOfPad.setColour(RectangleComponent::backgroundColourId, componentBackgroundColor);
    backgroundOfPad.setCornerSize(cornerSize);

    backgroundForSizeSlider.setBounds(mSizeSliderSection.getBounds().withSizeKeepingCentre(mSizeSliderSection.getWidth() + gap, mSizeSliderSection.getHeight() + gap));
    backgroundForSizeSlider.setColour(RectangleComponent::backgroundColourId, componentBackgroundColor);
    backgroundForSizeSlider.setCornerSize(cornerSize);

    backgroundForWetSection.setBounds(mWetAndDrySection.getBounds().withSizeKeepingCentre(backgroundForSizeSlider.getWidth(), mWetAndDrySection.getHeight() + gap));
    backgroundForWetSection.setColour(RectangleComponent::backgroundColourId, componentBackgroundColor);
    backgroundForWetSection.setCornerSize(cornerSize);
    for (int i = 0; i < 3; i++)
    {
        mBoxIcon[i][0].setSize(sliderHeight, sliderHeight);
        mBoxIcon[i][0].setCentrePosition(mBoxIcon[i][0].getWidth()/2 + gap, mBoxIcon[i][0].getHeight() / 2 + i * sliderHeight + i * boxSize.y + gap * i * 2);
        for (int j = 0; j < 3; j++)
        {
            mBoxIcon[i][0].setBoxSize(j, 0.5);
        }
        mBoxIcon[i][0].setBoxSize(i, 0.2);
        
        mBoxIcon[i][1].setSize(sliderHeight, sliderHeight);
        mBoxIcon[i][1].setCentrePosition(mSizeSliderSection.getWidth()-mBoxIcon[i][0].getWidth() / 2 - gap, mBoxIcon[i][1].getHeight() / 2 + i * sliderHeight + i * boxSize.y + gap * i * 2);
        for (int j = 0; j < 3; j++)
        {
            mBoxIcon[i][1].setBoxSize(j, 0.5);
        }
        mBoxIcon[i][1].setBoxSize(i, 1.0);
        sizeSliderList[i].setTooltip("Press Ctrl to slowly slide");
        sizeSliderList[i].setLookAndFeel(&mLookAndFeel);
        //sizeSliderList[i].setTextValueSuffix("m");
        sizeSliderList[i].setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, sliderWidth, sliderHeight/1.5);
        mSizeTextBox[i].setJustificationType(Justification::centred);
        mSizeTextBox[i].setSize(boxSize.x, boxSize.y);
        mSizeTextBox[i].setCentrePosition(mSizeSliderSection.getWidth() / 2, boxSize.y / 2 + boxSize.y / 2 + i * sliderHeight + i * boxSize.y + gap  * i*2);
        //mSizeTextBox[i].setText("hello", NotificationType::dontSendNotification);
        mSizeTextBox[i].setFont(mSizeTextBox[i].getFont().boldened());
        mSizeTextBox[i].setColour(Label::textColourId, ColorTheme::mainColor[i].brighter(0.2));

        sizeSliderList[i].addListener(this);
        //dimesionSliderList[i].setRange(Range<double>(-1.0, 1.0), 0.001);
        spaceSlider.registerSlider(&dimesionSliderList[i],i);
        sizeSliderList[i].setSize(sliderWidth, sliderHeight);
        //sizeSliderList[i].setRange(1.0f,40.0f);
        sizeSliderList[i].setCentrePosition(mSizeSliderSection.getWidth() / 2,
            boxSize.y + sliderHeight/2 + gap + i * sliderHeight + i * boxSize.y + gap * i*2);
        sizeSliderList[i].setColour(Slider::trackColourId,ColorTheme::mainColor[i].darker(0.2));
        sizeSliderList[i].setColour(Slider::thumbColourId, ColorTheme::mainColor[i]);
        sizeSliderList[i].setNumDecimalPlacesToDisplay(2);

        mSizeSliderSection.addAndMakeVisible(mBoxIcon[i][0]);
        mSizeSliderSection.addAndMakeVisible(mBoxIcon[i][1]);
        mSizeSliderSection.addAndMakeVisible(mSizeTextBox[i]);
        mSizeSliderSection.addAndMakeVisible(sizeSliderList[i]);
    }



    
    drySlider.setSize(rotaryHeight, rotaryHeight);
    drySlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    drySlider.setTopLeftPosition(gap,0.0);
    drySlider.setLookAndFeel(&mLookAndFeel);
    drySlider.setNumDecimalPlacesToDisplay(2);
    drySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 100, 50);
    drySlider.setColour(Slider::thumbColourId, ColorTheme::mainColor[0]);
    drySlider.setColour(Slider::rotarySliderOutlineColourId, ColorTheme::backgroundColor.brighter(0.2));
    drySlider.setColour(Slider::rotarySliderFillColourId, ColorTheme::whiteColor);
    
    mMixName[0].setText("dry",NotificationType::dontSendNotification);
    mMixName[0].setJustificationType(Justification::centred);
    mMixName[0].setSize(boxSize.x, boxSize.y);
    mMixName[0].setCentrePosition(drySlider.getPosition() + Point<int>(drySlider.getWidth() / 2, drySlider.getHeight() + mMixName[0].getHeight()/2));
    mMixName[0].setFont(mMixName[0].getFont().boldened());
    
    mWetAndDrySection.addAndMakeVisible(mMixName[0]);

    wetSlider.setSize(rotaryHeight, rotaryHeight);
    wetSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    wetSlider.setTopLeftPosition(drySlider.getRight() + gap, 0.0);
    wetSlider.setLookAndFeel(&mLookAndFeel);
    wetSlider.setNumDecimalPlacesToDisplay(2);
    wetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 100, 50);
    wetSlider.setColour(Slider::thumbColourId, ColorTheme::mainColor[1]);
    wetSlider.setColour(Slider::rotarySliderOutlineColourId, ColorTheme::backgroundColor.brighter(0.2));
    wetSlider.setColour(Slider::rotarySliderFillColourId, ColorTheme::whiteColor);

    mMixName[1].setText("wet", NotificationType::dontSendNotification);
    mMixName[1].setJustificationType(Justification::centred);
    mMixName[1].setSize(boxSize.x, boxSize.y);
    mMixName[1].setCentrePosition(wetSlider.getPosition() + Point<int>(wetSlider.getWidth() / 2, wetSlider.getHeight() + mMixName[1].getHeight() / 2));
    mMixName[1].setFont(mMixName[1].getFont().boldened());
    mWetAndDrySection.addAndMakeVisible(mMixName[1]);

    reverbSlider.setSize(rotaryHeight, rotaryHeight);
    reverbSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    reverbSlider.setTopLeftPosition(wetSlider.getRight() + gap, 0.0);
    reverbSlider.setLookAndFeel(&mLookAndFeel);
    reverbSlider.setNumDecimalPlacesToDisplay(2);
    reverbSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 100, 50);
    reverbSlider.setColour(Slider::thumbColourId, ColorTheme::mainColor[2]);
    reverbSlider.setColour(Slider::rotarySliderOutlineColourId, ColorTheme::backgroundColor.brighter(0.2));
    reverbSlider.setColour(Slider::rotarySliderFillColourId, ColorTheme::whiteColor);

    mMixName[2].setText("reverb", NotificationType::dontSendNotification);
    mMixName[2].setJustificationType(Justification::centred);
    mMixName[2].setSize(boxSize.x, boxSize.y);
    mMixName[2].setCentrePosition(reverbSlider.getPosition() + Point<int>(reverbSlider.getWidth() / 2, reverbSlider.getHeight() + mMixName[2].getHeight() / 2));
    mMixName[2].setFont(mMixName[2].getFont().boldened());

    mWetAndDrySection.addAndMakeVisible(mMixName[2]);

    decaySlider.setSize(rotaryHeight, rotaryHeight);
    decaySlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    decaySlider.setTopLeftPosition(reverbSlider.getRight() + gap, 0.0);
    decaySlider.setLookAndFeel(&mLookAndFeel);
    decaySlider.setNumDecimalPlacesToDisplay(2);
    decaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 100, 50);
    decaySlider.setColour(Slider::thumbColourId, ColorTheme::mainColor[3]);
    decaySlider.setColour(Slider::rotarySliderOutlineColourId, ColorTheme::backgroundColor.brighter(0.2));
    decaySlider.setColour(Slider::rotarySliderFillColourId, ColorTheme::whiteColor);

    mDecayName.setText("decay", NotificationType::dontSendNotification);
    mDecayName.setJustificationType(Justification::centred);
    mDecayName.setSize(boxSize.x, boxSize.y);
    mDecayName.setCentrePosition(decaySlider.getPosition() + Point<int>(decaySlider.getWidth() / 2, decaySlider.getHeight() + mDecayName.getHeight() / 2));
    mDecayName.setFont(mDecayName.getFont().boldened());

    mWetAndDrySection.addAndMakeVisible(mDecayName);

    keepGainButton.setSize(keepGainWidth, 50);
    keepGainButton.setTitle("Keep gain");
    keepGainButton.setTooltip("Make gain independent of distance");

    keepGainButton.setCentrePosition(decaySlider.getRight() + gap + keepGainButton.getWidth() / 2, decaySlider.getHeight() / 2);

    mWetAndDrySection.addAndMakeVisible(keepGainButton);
    
    drySlider.addListener(this);
    wetSlider.addListener(this);

    updatePositioner();
    addAndMakeVisible(mSizeSliderSection);
    mWetAndDrySection.addAndMakeVisible(wetSlider);
    mWetAndDrySection.addAndMakeVisible(drySlider);
    mWetAndDrySection.addAndMakeVisible(decaySlider);
    mWetAndDrySection.addAndMakeVisible(reverbSlider);
    addAndMakeVisible(mWetAndDrySection);
    addAndMakeVisible(spaceSlider);
    wetAttachment.sendInitialUpdate();
    dryAttachment.sendInitialUpdate();
    decayAttachment.sendInitialUpdate();
    reverbAttachment.sendInitialUpdate();
    for (int i = 0; i < 3; i++)
    {
        juce::String newValue(sizeSliderList[i].getValue(), 2);
        newValue += "m";
        mSizeTextBox[i].setText(newValue, NotificationType::dontSendNotification);  
    }
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
    
}

void TruePositionAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    int whichSizeSlider = -1;
    for (int i = 0; i < 3; i++)
    {
        if (slider == &sizeSliderList[i])
            whichSizeSlider = i;
    }
    if (whichSizeSlider > -1)
    {
        juce::String newValue(slider->getValue(), 2);
        newValue += "m";
        mSizeTextBox[whichSizeSlider].setText(newValue, NotificationType::dontSendNotification);
    }
    updatePositioner();
}

void TruePositionAudioProcessorEditor::buttonStateChanged(Button* button)
{
}

void TruePositionAudioProcessorEditor::buttonClicked(Button*)
{
}
