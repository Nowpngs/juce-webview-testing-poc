import * as Juce from "./juce/index.js";

// Adding a listener from the event that is sent from the C++ backend
window.__JUCE__.backend.addEventListener(
  "exampleEvent",
  (objectFromCppBackend) => {
    console.log(objectFromCppBackend);
  }
);

// Get the initialisation data from the C++ backend
const data = window.__JUCE__.initialisationData;
document.getElementById("vendor").innerText = data.vendor;
document.getElementById("pluginName").innerText = data.pluginName;
document.getElementById("pluginVersion").innerText = data.pluginVersion;
