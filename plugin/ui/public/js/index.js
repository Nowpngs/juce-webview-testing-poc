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

// Get the native function from the C++ backend
const nativeFunction = Juce.getNativeFunction("nativeFunction");

// Fetch a resource from the C++ backend (for local development similar to API calls)
fetch(Juce.getBackendResourceAddress("exampleResource"))
  .then((response) => response.text())
  .then((textFromBackend) => {
    console.log(textFromBackend);
  });

// After the UI was rendered, add event listeners
document.addEventListener("DOMContentLoaded", () => {
  // Send an event to the C++ backend
  const button = document.getElementById("nativeFunctionButton");
  button.addEventListener("click", () => {
    nativeFunction("one", 2, null).then((result) => {
      console.log(result);
    });
  });

  const emitEventButton = document.getElementById("emitEventButton");
  let emittedCount = 0;
  emitEventButton.addEventListener("click", () => {
    emittedCount++;
    window.__JUCE__.backend.emitEvent("exampleJavaScriptEvent", {
      emittedCount: emittedCount,
    });
  });

  // Plotting with Plotly
  const base = -60;
  Plotly.newPlot("outputLevelPlot", {
    data: [
      {
        x: ["left"],
        y: [base],
        base: [base],
        type: "bar",
      },
    ],
    layout: {
      width: 200,
      height: 400,
      yaxis: {
        range: [base, 0],
      },
    },
  });

  window.__JUCE__.backend.addEventListener("outputLevel", () => {
    fetch(Juce.getBackendResourceAddress("getOutputLevel"))
      .then((response) => response.text())
      .then((outputLevel) => {
        const levelData = JSON.parse(outputLevel);
        Plotly.animate(
          "outputLevelPlot",
          {
            data: [
              {
                y: [levelData.left - base],
              },
            ],
            traces: [0],
            layout: {},
          },
          {
            transition: {
              duration: 20,
              easing: "cubic-in-out",
            },
            frame: {
              duration: 20,
              redraw: false,
            },
          }
        );
      });
  });
});
