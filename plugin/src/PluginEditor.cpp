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

constexpr auto LOCAL_DEV_SERVER_ADDRESS = "http://127.0.0.1:8080";
}

namespace audio_plugin
{
//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor & p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , webView (juce::WebBrowserComponent::Options ()
                   .withResourceProvider (
                       [this] (const juce::String & url) { return getResource (url); },
                       juce::URL {audio_plugin_util::LOCAL_DEV_SERVER_ADDRESS}.getOrigin ())
                   .withNativeIntegrationEnabled ()
                   .withUserScript (R"(console.log("Initialising Script");)")
                   .withInitialisationData ("vendor", JUCE_COMPANY_NAME)
                   .withInitialisationData ("pluginName", JUCE_PRODUCT_NAME)
                   .withInitialisationData ("pluginVersion", JUCE_PRODUCT_VERSION)
                   .withNativeFunction (
                       juce::Identifier ("nativeFunction"),
                       [this] (const juce::Array<juce::var> & args,
                               juce::WebBrowserComponent::NativeFunctionCompletion completion)
                       { nativeFunction (args, std::move (completion)); })
                   .withEventListener (
                       "exampleJavaScriptEvent",
                       [this] (juce::var objectFromFrontend)
                       {
                           labelUpdatedFromJavaScript.setText (
                               "Event received from JavaScript: " +
                                   objectFromFrontend.getProperty ("emittedCount", 0).toString (),
                               juce::dontSendNotification);
                       }))

{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (webView);
    // webView.goToURL (webView.getResourceProviderRoot ());
    webView.goToURL (audio_plugin_util::LOCAL_DEV_SERVER_ADDRESS);

    runJavaScriptButton.onClick = [this]
    {
        // This is the JavaScript that will be run
        constexpr auto JAVASCRIPT_TO_RUN {"console.log('Hello from JavaScript!');"};
        webView.evaluateJavascript (JAVASCRIPT_TO_RUN,
                                    [] (const juce::WebBrowserComponent::EvaluationResult result)
                                    {
                                        if (const auto * resultPtr = result.getResult ())
                                        {
                                            std::cout << "JavaScript eveluation result: "
                                                      << resultPtr->toString () << std::endl;
                                        }
                                        else
                                        {
                                            std::cout << "JavaScript eveluation failed!"
                                                      << result.getError ()->message << std::endl;
                                        }
                                    });
    };
    addAndMakeVisible (runJavaScriptButton);

    emitJavaScriptEventButton.onClick = [this]
    {
        // This is the event that the JavaScript will listen for
        static const juce::Identifier EVENT_ID {"exampleEvent"};
        webView.emitEventIfBrowserIsVisible (EVENT_ID, 42.0);
    };
    addAndMakeVisible (emitJavaScriptEventButton);

    addAndMakeVisible (labelUpdatedFromJavaScript);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable (true, true);
    setSize (800, 600);

    // Interval of timer callback is 60ms
    startTimer (60);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor ()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::resized ()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds ();
    webView.setBounds (bounds.removeFromRight (getWidth () / 2));
    runJavaScriptButton.setBounds (bounds.removeFromTop (50).reduced (5));
    emitJavaScriptEventButton.setBounds (bounds.removeFromTop (50).reduced (5));
    labelUpdatedFromJavaScript.setBounds (bounds.removeFromTop (50).reduced (5));
}

void AudioPluginAudioProcessorEditor::timerCallback ()
{
    webView.emitEventIfBrowserIsVisible ("outputLevel", juce::var {});
}

std::optional<juce::WebBrowserComponent::Resource>
AudioPluginAudioProcessorEditor::getResource (const juce::String & url)
{
    const juce::File resourceFileRoot =
        juce::File (audio_plugin_util::getCurrentRootPath () + "/plugin/ui/public");

    const juce::String urlToRetrive =
        url == "/" ? juce::String ("index.html") : url.fromFirstOccurrenceOf ("/", false, false);

    if (urlToRetrive == "exampleResource")
    {
        juce::DynamicObject::Ptr data (new juce::DynamicObject ());
        data->setProperty ("exampleProperty", 42);
        const juce::String json = juce::JSON::toString (data.get ());
        juce::MemoryInputStream stream (json.getCharPointer (), json.getNumBytesAsUTF8 (), false);
        return juce::WebBrowserComponent::Resource {audio_plugin_util::streamToVector (stream),
                                                    "application/json"};
    }

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

// This is the function that will be called from JavaScript
void AudioPluginAudioProcessorEditor::nativeFunction (
    const juce::Array<juce::var> & args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    juce::String concatenatedArgs;

    for (const auto & arg : args)
    {
        concatenatedArgs += arg.toString () + " ";
    }
    labelUpdatedFromJavaScript.setText ("Native function called with args: " + concatenatedArgs,
                                        juce::dontSendNotification);

    completion ("nativeFunction callback: All OK!");
}

}