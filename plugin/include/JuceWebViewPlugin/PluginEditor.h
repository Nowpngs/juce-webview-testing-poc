#pragma once

#include "PluginProcessor.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace audio_plugin
{
//==============================================================================
class AudioPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor () override;

    //==============================================================================
    void resized () override;

    void timerCallback () override;

private:
    std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String & url);

    void nativeFunction (const juce::Array<juce::var> & args,
                         juce::WebBrowserComponent::NativeFunctionCompletion completion);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor & processorRef;
    juce::WebBrowserComponent webView;

    juce::TextButton runJavaScriptButton {"Run some JavaScript"};
    juce::TextButton emitJavaScriptEventButton {"Emit  JavaScript event"};

    juce::Label labelUpdatedFromJavaScript {"label", "To be updated from JavaScript"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

}
