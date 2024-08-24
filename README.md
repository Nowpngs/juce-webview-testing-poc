# JUCE Webview Testing POC

Webview Testing POC is a JUCE-based audio plugin project designed to explore and evaluate the integration of webview functionality within a JUCE framework. This project serves as a proof of concept, aiming to test the viability and performance of embedding web-based interfaces in a native audio plugin environment.

## Setup

### Prerequisites

Before starting, ensure you have the following installed:

- **CMake**: Required for generating the build system. [Download CMake here](https://cmake.org/download/).

### Build & Install

Follow these steps to build and install your plugin:

1. **Clone the repository:**

   Start by cloning the repository to your local machine:

   ```bash
   git clone <your-repo-url>
   cd <your-repo-folder>
   ```

2. **Configure the project:**

   ```bash
   cmake -S . -B build
   ```

   This command will create a build directory where all the build files and dependencies will be placed.

3. **Build this plugin:**
   After configuring, build the plugin using:

   ```bash
   cmake --build build
   ```

   This will compile your plugin and create the necessary binaries.

## Running the Plugin

There are two ways to run the plugin with webview functionality in `PluginEditor.cpp`:

1. **Using `webView.getResourceProviderRoot()`:**

   This method loads the webview resources directly from the built-in resource provider. In `PluginEditor.cpp`, use the following:

   ```cpp
   webView.goToURL(webView.getResourceProviderRoot());
   // webView.goToURL(audio_plugin_util::LOCAL_DEV_SERVER_ADDRESS);
   ```

   After building the plugin, you can run the output executable directly or use the following command at the root path:

   ```bash
   make run-plugin
   ```

2. **Using a Local Developer Front-End Server:**

   If you prefer to use a local developer server for the front-end, follow these steps:

   - First, start the local server with:

   ```bash
   make start-local-server
   ```

   - Then, modify the code in `PluginEditor.cpp` to point to the local server address:

   ```cpp
   // webView.goToURL(webView.getResourceProviderRoot());
   webView.goToURL(audio_plugin_util::LOCAL_DEV_SERVER_ADDRESS);
   ```

   - Finally, run the executable.

   ```bash
   make run-plugin
   ```

**Note:** Remember to always build the plugin before running it to ensure all changes are applied.

## Coding Standard

This project uses `clang-format` to maintain a consistent coding standard. To apply the coding standard to all `.cpp` and `.h` files in the plugin directories, follow these steps:

1. **Install clang-format**

   Ensure you have `clang-format` installed on your system. On macOS, you can easily install it using Homebrew:

   ```bash
   brew install clang-format
   ```

2. **Format the code**

   Run the following command from the root directory of the project to format all `.cpp` and `.h` files in the plugin directories:

   ```bash
   find plugin \( -name '*.cpp' -o -name '*.h' \) -exec clang-format -i {} +
   ```

   This will apply `clang-format` to all relevant files according to the style defined in your `.clang-format` file.

   **Note:** Always format your code before committing changes to ensure that your code adheres to the project's coding standards. This helps maintain a consistent style across the codebase.

## License

This project is licensed under the MIT License. See the full license text in the [LICENSE](https://opensource.org/license/mit) file.
