#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

class VintageDelayDSP
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        constexpr double maxDelaySeconds = 2.0;
        const auto maxDelaySamples = (int) std::ceil(maxDelaySeconds * sampleRate) + 8;

        delayLineL = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>(maxDelaySamples);
        delayLineR = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>(maxDelaySamples);
        delayLineL.reset();
        delayLineR.reset();

        lowCutL.reset();
        lowCutR.reset();
        hiCutL.reset();
        hiCutR.reset();

        lowCutL.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        lowCutR.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        hiCutL.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        hiCutR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        lowCutL.prepare(spec);
        lowCutR.prepare(spec);
        hiCutL.prepare(spec);
        hiCutR.prepare(spec);

        lowCutL.setResonance(0.707f);
        lowCutR.setResonance(0.707f);
        hiCutL.setResonance(0.707f);
        hiCutR.setResonance(0.707f);

        timeMsSm.reset(sampleRate, 0.08);
        fbSm.reset(sampleRate, 0.03);
        mixSm.reset(sampleRate, 0.03);
        driveSm.reset(sampleRate, 0.03);
        lowCutSm.reset(sampleRate, 0.05);
        hiCutSm.reset(sampleRate, 0.05);
        flutterAmtSm.reset(sampleRate, 0.05);
        flutterRateSm.reset(sampleRate, 0.05);

        flutterPhase = 0.0f;
        driftLP = 0.0f;
    }

    void reset()
    {
        delayLineL.reset();
        delayLineR.reset();
        lowCutL.reset();
        lowCutR.reset();
        hiCutL.reset();
        hiCutR.reset();
        flutterPhase = 0.0f;
        driftLP = 0.0f;
    }

    struct Params
    {
        float timeMs = 400.0f;
        bool syncOn = false;
        int syncDivIndex = 2;

        float feedback = 0.4f;
        float mix = 0.25f;
        float outputDb = 0.0f;
        float lowCutHz = 120.0f;
        float hiCutHz = 7000.0f;

        float flutterAmtMs = 0.3f;
        float flutterRateHz = 4.0f;
        float flutterRand = 0.25f;

        float drive = 0.2f;
    };

    void processBlock(juce::AudioBuffer<float>& buffer, const Params& p)
    {
        const auto numCh = buffer.getNumChannels();
        const auto numSamp = buffer.getNumSamples();
        if (numCh == 0 || numSamp == 0)
            return;

        timeMsSm.setTargetValue(p.timeMs);
        fbSm.setTargetValue(juce::jlimit(0.0f, 0.95f, p.feedback));
        mixSm.setTargetValue(juce::jlimit(0.0f, 1.0f, p.mix));
        driveSm.setTargetValue(juce::jlimit(0.0f, 1.0f, p.drive));
        lowCutSm.setTargetValue(juce::jlimit(20.0f, 1000.0f, p.lowCutHz));
        hiCutSm.setTargetValue(juce::jlimit(1500.0f, 18000.0f, p.hiCutHz));
        flutterAmtSm.setTargetValue(juce::jlimit(0.0f, 2.0f, p.flutterAmtMs));
        flutterRateSm.setTargetValue(juce::jlimit(0.1f, 8.0f, p.flutterRateHz));

        const float outGain = juce::Decibels::decibelsToGain(p.outputDb);

        auto* chL = buffer.getWritePointer(0);
        auto* chR = (numCh > 1) ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < numSamp; ++i)
        {
            const float timeMs = timeMsSm.getNextValue();
            const float fb = fbSm.getNextValue();
            const float mix = mixSm.getNextValue();
            const float drive = driveSm.getNextValue();
            const float lowCutHz = lowCutSm.getNextValue();
            const float hiCutHz = hiCutSm.getNextValue();
            const float flAmtMs = flutterAmtSm.getNextValue();
            const float flRateHz = flutterRateSm.getNextValue();

            lowCutL.setCutoffFrequency(lowCutHz);
            lowCutR.setCutoffFrequency(lowCutHz);
            hiCutL.setCutoffFrequency(hiCutHz);
            hiCutR.setCutoffFrequency(hiCutHz);

            const float phaseInc = (float) (juce::MathConstants<double>::twoPi * (double) flRateHz / sampleRate);
            flutterPhase += phaseInc;
            if (flutterPhase > juce::MathConstants<float>::twoPi)
                flutterPhase -= juce::MathConstants<float>::twoPi;

            const float lfo = std::sin(flutterPhase);

            const float noise = (random.nextFloat() * 2.0f - 1.0f);
            driftLP = driftLP + 0.0015f * (noise - driftLP);
            const float driftPart = driftLP * p.flutterRand;

            const float modMs = flAmtMs * (0.75f * lfo + 0.25f * driftPart);

            float delaySamp = (timeMs + modMs) * (float) sampleRate / 1000.0f;
            delaySamp = juce::jlimit(1.0f, (float) (delayLineL.getMaximumDelayInSamples() - 2), delaySamp);

            delayLineL.setDelay(delaySamp);
            delayLineR.setDelay(delaySamp);

            const float inL = chL[i];
            const float inR = (chR != nullptr) ? chR[i] : inL;

            float dL = delayLineL.popSample(0);
            float dR = delayLineR.popSample(0);

            float fbL = dL;
            float fbR = dR;

            fbL = lowCutL.processSample(0, fbL);
            fbR = lowCutR.processSample(chR != nullptr ? 1 : 0, fbR);
            fbL = hiCutL.processSample(0, fbL);
            fbR = hiCutR.processSample(chR != nullptr ? 1 : 0, fbR);

            fbL = saturate(fbL, drive);
            fbR = saturate(fbR, drive);

            const float writeL = inL + fbL * fb;
            const float writeR = inR + fbR * fb;

            delayLineL.pushSample(0, writeL);
            delayLineR.pushSample(0, writeR);

            const float wetL = dL;
            const float wetR = dR;

            chL[i] = (inL * (1.0f - mix) + wetL * mix) * outGain;
            if (chR != nullptr)
                chR[i] = (inR * (1.0f - mix) + wetR * mix) * outGain;
        }

        for (int ch = 2; ch < numCh; ++ch)
            buffer.copyFrom(ch, 0, buffer, 0, 0, numSamp);
    }

private:
    static float saturate(float x, float drive)
    {
        const float g = 1.0f + drive * 5.0f;
        return std::tanh(x * g);
    }

    double sampleRate = 44100.0;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineL { 48000 };
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineR { 48000 };

    juce::dsp::StateVariableTPTFilter<float> lowCutL, lowCutR;
    juce::dsp::StateVariableTPTFilter<float> hiCutL, hiCutR;

    juce::SmoothedValue<float> timeMsSm, fbSm, mixSm, driveSm, lowCutSm, hiCutSm, flutterAmtSm, flutterRateSm;

    float flutterPhase = 0.0f;
    float driftLP = 0.0f;

    juce::Random random;
};
