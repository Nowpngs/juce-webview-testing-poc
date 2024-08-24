import * as Juce from "./juce/index.js";

window.__JUCE__.backend.addEventListener(
  "exampleEvent",
  (objectFromCppBackend) => {
    console.log(objectFromCppBackend);
  }
);

const data = window.__JUCE__.initialisationData;
document.getElementById("vendor").innerText = data.vendor;
document.getElementById("pluginName").innerText = data.pluginName;
document.getElementById("pluginVersion").innerText = data.pluginVersion;
