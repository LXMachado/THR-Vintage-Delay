#include "PluginProcessor.h"
#include "PluginEditor.h"

BlueHazeAudioProcessorEditor::BlueHazeAudioProcessorEditor(BlueHazeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize (400, 300);
}

BlueHazeAudioProcessorEditor::~BlueHazeAudioProcessorEditor()
{
}

void BlueHazeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (20.0f, juce::Font::bold));
    g.setColour (juce::Colours::white);
    g.drawText ("Blue Haze", getLocalBounds(), juce::Justification::centred, true);
}

void BlueHazeAudioProcessorEditor::resized()
{
}
