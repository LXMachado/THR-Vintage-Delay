#include "PluginProcessor.h"
#include "PluginEditor.h"

BlueHazeAudioProcessor::BlueHazeAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
}

void BlueHazeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

    dsp.prepare(spec);
}

bool BlueHazeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

VintageDelayDSP::Params BlueHazeAudioProcessor::pullParams() const
{
    VintageDelayDSP::Params p;

    auto getF = [this](const char* id)
    {
        return apvts.getRawParameterValue(id)->load();
    };

    auto getB = [this](const char* id)
    {
        return apvts.getRawParameterValue(id)->load() > 0.5f;
    };

    p.timeMs = getF(ParamIDs::timeMs);
    p.feedback = getF(ParamIDs::feedback);
    p.mix = getF(ParamIDs::mix);
    p.outputDb = getF(ParamIDs::outputDb);
    p.lowCutHz = getF(ParamIDs::lowCutHz);
    p.hiCutHz = getF(ParamIDs::hiCutHz);
    p.flutterAmtMs = getF(ParamIDs::flutterAmtMs);
    p.flutterRateHz = getF(ParamIDs::flutterRateHz);
    p.flutterRand = getF(ParamIDs::flutterRand);
    p.drive = getF(ParamIDs::drive);

    p.syncOn = getB(ParamIDs::syncOn);
    p.syncDivIndex = (int) getF(ParamIDs::syncDiv);

    return p;
}

void BlueHazeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    auto params = pullParams();
    dsp.processBlock(buffer, params);
}

void BlueHazeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BlueHazeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* BlueHazeAudioProcessor::createEditor()
{
    return new BlueHazeAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BlueHazeAudioProcessor();
}
