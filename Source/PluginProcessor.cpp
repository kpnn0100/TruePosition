/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TruePositionAudioProcessor::TruePositionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TruePositionAudioProcessor::~TruePositionAudioProcessor()
{
    
}

//==============================================================================
const juce::String TruePositionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TruePositionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TruePositionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TruePositionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TruePositionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TruePositionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TruePositionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TruePositionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TruePositionAudioProcessor::getProgramName (int index)
{
    return {};
}

void TruePositionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TruePositionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    SignalProcessor::setSampleRate(sampleRate);
    SignalProcessor::setBufferSize(samplesPerBlock);
    mFilter.setMaxDistance(100.0);
    mFilter.prepare();
    updateFilter();
}

void TruePositionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TruePositionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TruePositionAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    float* bufferChannel[2];
    bufferChannel[0] = buffer.getWritePointer(0);
    bufferChannel[1] = buffer.getWritePointer(1);
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.



    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //data structure that points to the buffer
    updateFilter();
    std::vector<double> outputData[2];
    int bufferSize = buffer.getNumSamples();
    for (int sample = 0; sample < bufferSize; sample++)
    {
        //data[1] = (double)bufferChannel[0][sample];
        double data = bufferChannel[0][sample];
        for (int channel = 0; channel < totalNumOutputChannels; channel++)
        {
            outputData[channel].push_back(data);
            
        }
    }

    for (int channel = 0; channel < totalNumOutputChannels; channel++)
    {
        mThreadPool.addJob([bufferSize, channel, this, bufferChannel, &outputData] {
            for (int sample = 0; sample < bufferSize; sample++)
            {
                bufferChannel[channel][sample] = mFilter.getFilter(channel).out(outputData[channel][sample]);
            }
            });
    }
    while (mThreadPool.getNumJobs() !=0);
}

//==============================================================================
bool TruePositionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TruePositionAudioProcessor::createEditor()
{
    return new TruePositionAudioProcessorEditor (*this);
}

//==============================================================================
void TruePositionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TruePositionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void TruePositionAudioProcessor::updateFilter()
{
    for (int i = 0; i < 3; i++)
    {
        mFilter.setRoomSize(i, mRoomSize.get(i));
    }
    
    mFilter.setSource(mSource);
    mFilter.setDestination(mDestination);
    mFilter.setKeepGain(mKeepGain);
    if (mDrySlider != nullptr && mWetSlider != nullptr)
    {
        mFilter.setDryMix(mDrySlider->getValue());
        mFilter.setWetMix(mWetSlider->getValue());
    }
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TruePositionAudioProcessor();
}
