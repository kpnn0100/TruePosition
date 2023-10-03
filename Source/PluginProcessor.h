/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <iostream>
#include "gyrus_space_dsp.h"
//==============================================================================
/**
*/
using namespace juce;
class TruePositionAudioProcessor  : public juce::AudioProcessor, public AudioProcessorValueTreeState::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TruePositionAudioProcessor();
    ~TruePositionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void updateFilter();
    Coordinate mSource;
    Coordinate mDestination;
    Coordinate mRoomSize;
    Coordinate mSourceNormalize;
    double mReverbMix = 0.0;
    double mDecay = 100.0;
    float mDryMix = 0;
    float mWetMix = 0;
    bool mKeepGain;
    float SPEED_OF_SOUND = 343.0; // m/s
    AudioProcessorValueTreeState& getParameterTree();
private:
    //==============================================================================
    void forceParameterSync();
    AudioProcessorValueTreeState parameters;
    ThreadPool mThreadPool;
    RoomSimulation mFilter;
    void parameterChanged(const String& parameterID, float newValue) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TruePositionAudioProcessor)
};
