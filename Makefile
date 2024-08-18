install-dependencies:
	@echo "Installing Dependencies..."
	cmake -S . -B build

build-plugin:
	@echo "Building Project..."
	cmake --build build

format-style:
	@echo "Formatting Code..."
	@find plugin test \( -name '*.cpp' -o -name '*.h' \) -exec clang-format -i {} +
	@echo "Done!"

run-plugin:
	@echo "Running Plugin..."
	@cd ./build/plugin/AudioPlugin_artefacts/Standalone/BasicAudioPlugin.app/Contents/MacOS && ./BasicAudioPlugin