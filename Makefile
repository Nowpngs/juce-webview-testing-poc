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