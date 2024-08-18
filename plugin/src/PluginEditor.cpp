#include "JuceWebViewPlugin/PluginEditor.h"

#include "JuceWebViewPlugin/PluginProcessor.h"

namespace audio_plugin_util
{
static auto streamToVector (juce::InputStream & stream)
{
    std::vector<std::byte> result ((size_t) stream.getTotalLength ());
    stream.setPosition (0);
    [[maybe_unused]] const auto bytesRead = stream.read (result.data (), result.size ());
    jassert (bytesRead == (ssize_t) result.size ());
    return result;
}

static const char * getMimeForExtension (const juce::String & extension)
{
    static const std::unordered_map<juce::String, const char *> mimeMap = {
        {{"htm"}, "text/html"},
        {{"html"}, "text/html"},
        {{"txt"}, "text/plain"},
        {{"jpg"}, "image/jpeg"},
        {{"jpeg"}, "image/jpeg"},
        {{"svg"}, "image/svg+xml"},
        {{"ico"}, "image/vnd.microsoft.icon"},
        {{"json"}, "application/json"},
        {{"png"}, "image/png"},
        {{"css"}, "text/css"},
        {{"map"}, "application/json"},
        {{"js"}, "text/javascript"},
        {{"woff2"}, "font/woff2"}};

    if (const auto it = mimeMap.find (extension.toLowerCase ()); it != mimeMap.end ())
        return it->second;

    jassertfalse;
    return "";
}

static juce::String getCurrentRootPath ()
{
    return juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile)
        .getParentDirectory ()
        .getParentDirectory ()
        .getParentDirectory ()
        .getParentDirectory ()
        .getParentDirectory ()
        .getFullPathName ();
}
}

namespace audio_plugin
{
//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor & p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , webView (juce::WebBrowserComponent::Options ().withResourceProvider (
          [this] (const juce::String & url) { return getResource (url); }))
{
    juce::ignoreUnused (processorRef);
    addAndMakeVisible (webView);
    webView.goToURL (webView.getResourceProviderRoot ());
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

std::optional<juce::WebBrowserComponent::Resource>
AudioPluginAudioProcessorEditor::getResource (const juce::String & url)
{
    const juce::File resourceFileRoot =
        juce::File (audio_plugin_util::getCurrentRootPath () + "/plugin/ui/public");

    const juce::String urlToRetrive =
        url == "/" ? juce::String ("index.html") : url.fromFirstOccurrenceOf ("/", false, false);

    const juce::File resource = resourceFileRoot.getChildFile (urlToRetrive);

    if (! resource.existsAsFile ())
    {
        return std::nullopt;
    }

    const juce::String extension = urlToRetrive.fromLastOccurrenceOf (".", false, false);
    return juce::WebBrowserComponent::Resource {
        audio_plugin_util::streamToVector (*resource.createInputStream ()),
        audio_plugin_util::getMimeForExtension (extension)};
}

}