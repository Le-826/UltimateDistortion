/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UltimateDistortionAudioProcessor::UltimateDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("MODE", this);
    treeState.addParameterListener("GAIN", this);
    treeState.addParameterListener("MIX", this);
    treeState.addParameterListener("TONE", this);
    treeState.addParameterListener("OUTPUT", this);
}

UltimateDistortionAudioProcessor::~UltimateDistortionAudioProcessor()
{
    treeState.removeParameterListener("MODE", this);
    treeState.removeParameterListener("GAIN", this);
    treeState.removeParameterListener("MIX", this);
    treeState.removeParameterListener("TONE", this);
    treeState.removeParameterListener("OUTPUT", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout UltimateDistortionAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    juce::StringArray modes = {"Full Wave Rectification", "Half Wave Rectification", "Hard Clippping", "Soft Clipping1","Soft Clipping2","Soft Clipping3", "Saturation", "Bit Reduction"};
    
    auto pMode = std::make_unique<juce::AudioParameterChoice>(juce::ParameterID({"MODE", 1}), "Mode", modes, 0);
    auto pGain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID({"GAIN", 1}), "Gain", 0.0f, 24.0f, 0.0f);
    auto pMix = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID({"MIX", 1}), "Mix", 0.0f, 1.0f, 1.0f);
    auto pTone = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID({"TONE", 1}), "Tone", 0.0f, 20000.0f, 20000.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID({"OUTPUT", 1}), "Output", -24.0f, 24.0f, 0.0f);
    
    params.push_back(std::move(pMode));
    params.push_back(std::move(pGain));
    params.push_back(std::move(pMix));
    params.push_back(std::move(pTone));
    params.push_back(std::move(pOutput));
    return { params.begin(), params.end () };
}

void UltimateDistortionAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    updateParameters();
}

void UltimateDistortionAudioProcessor::updateParameters()
{
    auto model = static_cast<int>(treeState.getRawParameterValue("MODE")->load());
    switch(model)
    {
        case 0:
        {
            distortion.setMode(Distortion<float>::Mode::kFullWave);
            break;
        }
        case 1:
        {
            distortion.setMode(Distortion<float>::Mode::kHalfWave);
            break;
        }
        case 2:
        {
            distortion.setMode(Distortion<float>::Mode::kHard);
            break;
        }
        case 3:
        {
            distortion.setMode(Distortion<float>::Mode::kSoft1);
            break;
        }
        case 4:
        {
            distortion.setMode(Distortion<float>::Mode::kSoft2);
            break;
        }
        case 5:
        {
            distortion.setMode(Distortion<float>::Mode::kSoft3);
            break;
        }
        case 6:
        {
            distortion.setMode(Distortion<float>::Mode::kSaturation);
            break;
        }
        case 7:
        {
            distortion.setMode(Distortion<float>::Mode::kBitCrush);
            break;
        }
    }
    
    distortion.setGain(treeState.getRawParameterValue("GAIN")->load());
    distortion.setMix(treeState.getRawParameterValue("MIX")->load());
    distortion.setOutput(treeState.getRawParameterValue("OUTPUT")->load());
    
    lpFilter.setCutoffFrequency(treeState.getRawParameterValue("TONE")->load());
}
//==============================================================================
const juce::String UltimateDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UltimateDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UltimateDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UltimateDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UltimateDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UltimateDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UltimateDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UltimateDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UltimateDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void UltimateDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UltimateDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    distortion.prepare(spec);
    
    lpFilter.prepare(spec);
    
    updateParameters();
}

void UltimateDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UltimateDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UltimateDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block {buffer};
    
    distortion.process(juce::dsp::ProcessContextReplacing<float>(block));
    lpFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool UltimateDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UltimateDistortionAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void UltimateDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void UltimateDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (treeState.state.getType()))
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UltimateDistortionAudioProcessor();
}
