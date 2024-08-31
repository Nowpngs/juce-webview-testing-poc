.PHONY: help install-dependencies build-plugin format-style run-plugin start-local-server

help:
	@echo "Available commands:"
	@echo "  install-dependencies  - Install the necessary dependencies for the project."
	@echo "  build-plugin          - Compile and build the plugin project."
	@echo "  format-style          - Format the C++ source code files."
	@echo "  run-plugin            - Run the standalone JUCE plugin."
	@echo "  start-local-server    - Start a local HTTP server in the plugin UI directory."


install-dependencies:
	@echo "Installing Dependencies..."
	cmake -S . -B build

build-plugin:
	@echo "Building Project..."
	cmake --build build

format-style:
	@echo "Formatting Code..."
	@find plugin \( -name '*.cpp' -o -name '*.h' \) -exec clang-format -i {} +
	@echo "Done!"

run-plugin:
	@echo "Running Plugin..."
	@cd ./build/plugin/JuceWebViewPlugin_artefacts/Standalone/JuceWebViewPlugin.app/Contents/MacOS && ./JuceWebViewPlugin

start-local-server:
	@echo "Starting Local Server..."
	@cd plugin/ui/public && npx http-server