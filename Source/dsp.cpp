/*
  ==============================================================================

    dsp.cpp
    Created: 9 Jun 2023 3:12:37pm
    Author:  Ryan

  ==============================================================================
*/

#include "dsp.h"

template <typename SampleType>
Distortion<SampleType>::Distortion()
{
    
}

template <typename SampleType>
void Distortion<SampleType>::setGain(SampleType newGain)
{
    gain.setTargetValue(newGain);
}

template <typename SampleType>
void Distortion<SampleType>::setMix(SampleType newMix)
{
    mix.setTargetValue(newMix);
}

template <typename SampleType>
void Distortion<SampleType>::setOutput(SampleType newOutput)
{
    output.setTargetValue(newOutput);
}

template <typename SampleType>
void Distortion<SampleType>::setMode(Mode newMode)
{
    mode = newMode;
}

template <typename SampleType>
void Distortion<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

template <typename SampleType>
void Distortion<SampleType>::reset() {
    if (sampleRate > 0)
    {
        gain.reset(sampleRate, 0.02);
        gain.setTargetValue(0.0);
        
        mix.reset(sampleRate, 0.02);
        mix.setTargetValue(1.0);
        
        output.reset(sampleRate, 0.02);
        output.setTargetValue(0.0);
    }
}

template <typename SampleType>
SampleType Distortion<SampleType>::processSample(SampleType inputSample) noexcept
{
    switch (mode)
    {
        case Mode::kFullWave:
        {
            return processFullWaveRectification(inputSample);
        }
        case Mode::kHalfWave:
        {
            return processHalfWaveRectification(inputSample);
        }
        case Mode::kHard:
        {
            return processHardClipping(inputSample);
            break;
        }
        case Mode::kSoft:
        {
            return processSoftClipping(inputSample);
            break;
        }
        case Mode::kSaturation:
        {
            return processSaturation(inputSample);
            break;
        }
        case Mode::kBitCrush:
        {
            return processBitReduction(inputSample);
            break;
        }
    }
}

template <typename SampleType>
SampleType Distortion<SampleType>::processFullWaveRectification(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(gain.getNextValue());
    
    if (wet < 0.0)
    {
        wet *= -1.0;
    }
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template <typename SampleType>
SampleType Distortion<SampleType>::processHalfWaveRectification(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(gain.getNextValue());
    
    if (wet < 0.0)
    {
        wet = 0.0;
    }
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template <typename SampleType>
SampleType Distortion<SampleType>::processHardClipping(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(gain.getNextValue());
    
    if (std::abs(wet) > 0.99)
    {
        wet *= 0.99 / std::abs(wet);
    }
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template <typename SampleType>
SampleType Distortion<SampleType>::processSoftClipping(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(gain.getNextValue());
    
    wet = piDivisor * std::atan(wet);
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template <typename SampleType>
SampleType Distortion<SampleType>::processSaturation(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(gain.getNextValue());
    
    if (wet >= 0.0)
    {
        wet = std::tanh(wet);
    }
    else
    {
        wet = std::tanh(std::sinh(wet)) - 0.2 * wet * std::sin(juce::MathConstants<float>::pi * wet);
    }
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template <typename SampleType>
SampleType Distortion<SampleType>::processBitReduction(SampleType inputSample)
{
    auto wet = inputSample * juce::Decibels::decibelsToGain(0.0);
    
    int intervals = 28.0 - gain.getNextValue();
    
    wet = std::round(intervals * wet) / intervals;
    
    auto ratio = (1.0 - mix.getNextValue()) * inputSample + wet * mix.getNextValue();
    
    return ratio * juce::Decibels::decibelsToGain(output.getNextValue());
}

template class Distortion<float>;
template class Distortion<double>;
