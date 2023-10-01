/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define DEBUG 1
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
                       ),
    parameters(*this, nullptr, "TruePosition", {
        std::make_unique<AudioParameterFloat>("sizeX", "Size X", NormalisableRange<float>{1.f, 40.f, 0.01f}, 2.f),
        std::make_unique<AudioParameterFloat>("sizeY", "Size Y", NormalisableRange<float>{1.f, 40.f, 0.01f}, 2.f),
        std::make_unique<AudioParameterFloat>("sizeZ", "Size Z", NormalisableRange<float>{1.f, 40.f, 0.01f}, 2.f),
        std::make_unique<AudioParameterFloat>("posX", "Pos X", NormalisableRange<float>{-1.0f, 1.0f, 0.001f}, 0.6f),
        std::make_unique<AudioParameterFloat>("posY", "Pos Y", NormalisableRange<float>{-1.0f, 1.0f, 0.001f}, 0.6f),
        std::make_unique<AudioParameterFloat>("posZ", "Pos Z", NormalisableRange<float>{-1.0f, 1.0f, 0.001f}, 0.6f),
        std::make_unique<AudioParameterFloat>("dry", "Dry mix", NormalisableRange<float>{0.f, 1.f, 0.001f}, 1.0f),
        std::make_unique<AudioParameterFloat>("wet", "Wet mix", NormalisableRange<float>{0.f, 1.f, 0.001f}, 0.5f),
        std::make_unique<AudioParameterFloat>("reverb", "Reverb mix", NormalisableRange<float>{0.f, 1.f, 0.001f}, 0.5f),
        std::make_unique<AudioParameterFloat>("decay", "Reverb Decay", NormalisableRange<float>{1000.f, 6000.f, 0.01f}, 1000.f),
        std::make_unique<AudioParameterBool>("keepGain", "Keep gain",false)
        })
#endif
{
    parameters.addParameterListener("sizeX", this);
    parameters.addParameterListener("sizeY", this);
    parameters.addParameterListener("sizeZ", this);

    parameters.addParameterListener("posX", this);
    parameters.addParameterListener("posY", this);
    parameters.addParameterListener("posZ", this);

    parameters.addParameterListener("dry", this);
    parameters.addParameterListener("wet", this);
    parameters.addParameterListener("reverb", this);
    parameters.addParameterListener("decay", this);
    parameters.addParameterListener("keepGain", this);
    forceParameterSync();
}

TruePositionAudioProcessor::~TruePositionAudioProcessor()
{
    parameters.removeParameterListener("sizeX", this);
    parameters.removeParameterListener("sizeY", this);
    parameters.removeParameterListener("sizeZ", this);

    parameters.removeParameterListener("posX", this);
    parameters.removeParameterListener("posY", this);
    parameters.removeParameterListener("posZ", this);

    parameters.removeParameterListener("dry", this);
    parameters.removeParameterListener("wet", this);
    parameters.removeParameterListener("reverb", this);
    parameters.removeParameterListener("decay", this);
    parameters.removeParameterListener("keepGain", this);
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
#if DEBUG
    std::cout << "before prepare" << std::endl;
#endif 
    SignalProcessor::setSampleRate(sampleRate);
    SignalProcessor::setBufferSize(samplesPerBlock);
    mFilter.setMaxDistance(100.0);
    mFilter.prepare();
    updateFilter();
#if DEBUG
    std::cout << "done prepare" << std::endl;
#endif 
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
#if DEBUG
    std::cout << "before update" << std::endl;
#endif 
    updateFilter();
    std::vector<double> outputData[2];
    int bufferSize = buffer.getNumSamples();
#if DEBUG
    std::cout << "before output" << std::endl;
#endif 
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
#if DEBUG
    std::cout << "after output" << std::endl;
#endif 
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
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TruePositionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
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
    mFilter.setDryMix(mDryMix);
    mFilter.setWetMix(mWetMix);
    mFilter.setDecayInMs(mDecay);
    mFilter.setReverbWet(mReverbMix);
}

AudioProcessorValueTreeState& TruePositionAudioProcessor::getParameterTree()
{
    return parameters;
}

void TruePositionAudioProcessor::forceParameterSync()
{
    mDryMix = parameters.getParameter("dry")->getValue();
    mWetMix = parameters.getParameter("wet")->getValue();
    for (int i = 0; i < 3; i++)
    {
        String name = std::string(1,'X' + i);
        String parameterString = "size" + name;
        float roomValue = parameters.getParameter(parameterString)->getValue();
        mRoomSize.set(i, roomValue);
        parameterString = "pos"+ name;
        float sourceValue = parameters.getParameter(parameterString)->getValue();
        mSource.set(i, sourceValue);
    }
    float keep = parameters.getParameter("keepGain")->getValue();
    if (keep < 0.5f)
        mKeepGain = false;
    else
        mKeepGain = true;
}

void TruePositionAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID.containsIgnoreCase("size"))
    {
        int newIndex = parameterID[4] - 'X';
        mRoomSize.set(newIndex, newValue);
        mDestination.set(newIndex, newValue / 2);
    }
    if (parameterID.containsIgnoreCase("pos"))
    {
        int idToSet = parameterID[3] - 'X';
        mSource.set(idToSet, (newValue+1.0) * mRoomSize.get(idToSet)/2);
    }
    if (parameterID.equalsIgnoreCase("dry"))
    {
        mDryMix = newValue;
    }
    if (parameterID.equalsIgnoreCase("wet"))
    {
        mWetMix = newValue;
    }
    if (parameterID.equalsIgnoreCase("reverb"))
    {
        mReverbMix = newValue;
    }
    if (parameterID.equalsIgnoreCase("decay"))
    {
        mDecay = newValue;
    }
    if (parameterID.equalsIgnoreCase("keepGain"))
    {
        if (newValue < 0.5f)
            mKeepGain = false;
        else
            mKeepGain = true;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TruePositionAudioProcessor();
}
