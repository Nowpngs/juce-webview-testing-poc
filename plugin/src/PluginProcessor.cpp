#include "JuceWebViewPlugin/PluginProcessor.h"

#include "JuceWebViewPlugin/PluginEditor.h"

namespace audio_plugin
{
//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor ()
    : AudioProcessor (BusesProperties ()
#if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo (), true)
    #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo (), true)
#endif
      )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor ()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName () const
{
    return "AudioPlugin";
}

bool AudioPluginAudioProcessor::acceptsMidi () const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi () const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect () const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds () const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms ()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram ()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String & newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare the envelope follower with the processing specifications
    // juce::dsp::ProcessSpec holds the sample rate, block size, and number of channels
    envelopeFollower.prepare (
        juce::dsp::ProcessSpec {sampleRate,
                                static_cast<juce::uint32> (samplesPerBlock),
                                static_cast<juce::uint32> (getTotalNumOutputChannels ())});

    // Set the attack time and release time for the envelope follower (in milliseconds)
    // Set the type of level calculation for the envelope follower to 'peak'
    envelopeFollower.setAttackTime (200.f);
    envelopeFollower.setReleaseTime (200.f);
    envelopeFollower.setLevelCalculationType (
        juce::dsp::BallisticsFilter<float>::LevelCalculationType::peak);

    // Resize the buffer used for storing the output of the envelope follower
    // The buffer is set to match the number of output channels and samples per block
    envelopeFollowerOutputBuffer.setSize (getTotalNumOutputChannels (), samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources ()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout & layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet () != juce::AudioChannelSet::mono () &&
        layouts.getMainOutputChannelSet () != juce::AudioChannelSet::stereo ())
        return false;

        // This checks if the input layout matches the output layout
    #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet () != layouts.getMainInputChannelSet ())
        return false;
    #endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float> & buffer,
                                              juce::MidiBuffer & midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels ();
    auto totalNumOutputChannels = getTotalNumOutputChannels ();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples ());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto * channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }

    // Create an AudioBlock from the input buffer, selecting a subset of channels
    // inBlock will only include the channels from 0 to the total number of output channels
    const auto inBlock = juce::dsp::AudioBlock<float> (buffer).getSubsetChannelBlock (
        0u, static_cast<size_t> (getTotalNumOutputChannels ()));

    // Create an AudioBlock that wraps around the envelope follower's output buffer
    auto outBlock = juce::dsp::AudioBlock<float> (envelopeFollowerOutputBuffer);

    // Process the input block with the envelope follower, storing the result in the output block
    // ProcessContextNonReplacing means that the input block is not modified; the output block
    // stores the result
    envelopeFollower.process (juce::dsp::ProcessContextNonReplacing<float> (inBlock, outBlock));

    // Convert the final sample of the first channel of the output block to decibels
    // This represents the output level of the left channel
    outputLevelLeft = juce::Decibels::gainToDecibels (
        outBlock.getSample (0u, static_cast<int> (outBlock.getNumSamples () - 1)));
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor () const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor * AudioPluginAudioProcessor::createEditor ()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock & destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void * data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor * JUCE_CALLTYPE createPluginFilter ()
{
    return new audio_plugin::AudioPluginAudioProcessor ();
}
