/*
  ==============================================================================

    dsp.h
    Created: 9 Jun 2023 3:12:37pm
    Author:  Ryan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class Distortion
{
public:
    Distortion();
    
    enum class Mode
    {
        kFullWave,
        kHalfWave,
        kHard,
        kSoft1,
        kSoft2,
        kSoft3,
        kSaturation,
        kBitCrush
    };
    
    void setGain(SampleType newGain);
    
    void setMix(SampleType newMix);
    
    void setOutput(SampleType newOutput);
    
    void setMode(Mode newMode);
    
    void prepare(juce::dsp::ProcessSpec& spec);
    
    void reset();
    
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock      = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples  = outputBlock.getNumSamples();

        jassert (inputBlock.getNumChannels() == numChannels);
        jassert (inputBlock.getNumSamples()  == numSamples);

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples  = inputBlock .getChannelPointer (channel);
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample(inputSamples[i]);
        }
    }
    
    SampleType processSample(SampleType inputSample) noexcept;
    
    SampleType processFullWaveRectification(SampleType inputSample);
    
    SampleType processHalfWaveRectification(SampleType inputSample);
    
    SampleType processHardClipping(SampleType inputSample);
    
    SampleType processSoftClipping1(SampleType inputSample);
    
    SampleType processSoftClipping2(SampleType inputSample);
    
    SampleType processSoftClipping3(SampleType inputSample);
    
    SampleType processSaturation(SampleType inputSample);
    
    SampleType processBitReduction(SampleType inputSample);
    
private:
    juce::SmoothedValue<float> gain;
    juce::SmoothedValue<float> mix;
    juce::SmoothedValue<float> output;
    
    float piDivisor = 2.0 / juce::MathConstants<float>::pi;
    
    float sampleRate = 44100.0f;
    Mode mode = Mode::kHard;
};
