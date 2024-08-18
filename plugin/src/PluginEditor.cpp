#include "JuceWebViewPlugin/PluginEditor.h"

#include "JuceWebViewPlugin/PluginProcessor.h"

namespace audio_plugin
{
//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor & p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , webView {juce::WebBrowserComponent::Options {}}
{
    juce::ignoreUnused (processorRef);
    addAndMakeVisible (webView);
    webView.goToURL ("https://www.juce.com");
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable (true, true);
    setSize (800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor ()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::resized ()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    webView.setBounds (getLocalBounds ());
}
}