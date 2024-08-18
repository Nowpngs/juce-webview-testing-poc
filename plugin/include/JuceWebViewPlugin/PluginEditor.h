#pragma once

#include "PluginProcessor.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace audio_plugin
{
//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor () override;

    //==============================================================================
    void resized () override;

private:
    std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String & url);
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor & processorRef;
    juce::WebBrowserComponent webView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

}
