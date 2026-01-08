#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class BlueHazeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BlueHazeAudioProcessorEditor(BlueHazeAudioProcessor&);
    ~BlueHazeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BlueHazeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlueHazeAudioProcessorEditor)
};
