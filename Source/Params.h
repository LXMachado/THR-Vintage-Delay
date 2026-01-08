#pragma once

#include <juce_core/juce_core.h>

namespace ParamIDs
{
    inline constexpr auto timeMs = "timeMs";
    inline constexpr auto feedback = "feedback";
    inline constexpr auto mix = "mix";
    inline constexpr auto outputDb = "outputDb";
    inline constexpr auto lowCutHz = "lowCutHz";
    inline constexpr auto hiCutHz = "hiCutHz";
    inline constexpr auto flutterAmtMs = "flutterAmtMs";
    inline constexpr auto flutterRateHz = "flutterRateHz";
    inline constexpr auto flutterRand = "flutterRand";
    inline constexpr auto drive = "drive";

    inline constexpr auto syncOn = "syncOn";
    inline constexpr auto syncDiv = "syncDiv";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::timeMs,
        "Time (ms)",
        NormalisableRange<float>(1.0f, 2000.0f, 0.01f, 0.5f),
        400.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::feedback,
        "Feedback",
        NormalisableRange<float>(0.0f, 0.95f, 0.0001f, 1.0f),
        0.40f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::mix,
        "Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 1.0f),
        0.25f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::outputDb,
        "Output (dB)",
        NormalisableRange<float>(-12.0f, 12.0f, 0.01f, 1.0f),
        0.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::lowCutHz,
        "Low Cut (Hz)",
        NormalisableRange<float>(20.0f, 1000.0f, 0.01f, 0.5f),
        120.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::hiCutHz,
        "Hi Cut (Hz)",
        NormalisableRange<float>(1500.0f, 18000.0f, 0.01f, 0.5f),
        7000.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::flutterAmtMs,
        "Flutter Amt (ms)",
        NormalisableRange<float>(0.0f, 2.0f, 0.0001f, 1.0f),
        0.30f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::flutterRateHz,
        "Flutter Rate (Hz)",
        NormalisableRange<float>(0.1f, 8.0f, 0.0001f, 0.7f),
        4.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::flutterRand,
        "Flutter Random",
        NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 1.0f),
        0.25f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParamIDs::drive,
        "Drive",
        NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 1.0f),
        0.20f));

    params.push_back(std::make_unique<AudioParameterBool>(
        ParamIDs::syncOn,
        "Sync",
        false));

    params.push_back(std::make_unique<AudioParameterChoice>(
        ParamIDs::syncDiv,
        "Sync Division",
        StringArray { "1/1", "1/2", "1/4", "1/8", "1/8T", "1/16", "1/16T", "1/32" },
        2));

    return { params.begin(), params.end() };
}
