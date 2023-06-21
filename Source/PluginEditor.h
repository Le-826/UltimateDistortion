/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class UltimateDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UltimateDistortionAudioProcessorEditor (UltimateDistortionAudioProcessor&);
    ~UltimateDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UltimateDistortionAudioProcessor& audioProcessor;
    
    //==============================================================================
    juce::LookAndFeel_V4 knobLookAndFeel;
    
    juce::TextButton modeBar;
    juce::TextButton modeButton1;
    juce::TextButton modeButton2;
    juce::TextButton modeButton3;
    juce::TextButton modeButton4;
    juce::TextButton modeButton5;
    juce::TextButton modeButton6;
    juce::TextButton modeButton7;
    juce::TextButton modeButton8;
    juce::Slider gainKnob;
    juce::Slider mixKnob;
    juce::Slider toneKnob;
    juce::Slider outputKnob;
    juce::Label modeLabel;
    juce::Label gainLabel;
    juce::Label mixLabel;
    juce::Label toneLabel;
    juce::Label outputLabel;
    
    juce::Array<juce::TextButton> buttons;
    
//    juce::AudioProcessorValueTreeState::ButtonAttachment modeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment, mixAttachment, toneAttachment, outputAttachment;

    void selectMode(juce::TextButton* button, float modeIndex)
    {
        
        // Set the selected mode by setting the parameter value
        audioProcessor.treeState.getParameter("MODE")->setValueNotifyingHost(modeIndex);
        
        // Handle button click event
        if (button->getToggleState())
        {
            // Button is toggled (pressed)
            button->setColour(juce::TextButton::buttonOnColourId, findColour(juce::Slider::thumbColourId));
        }
        else
        {
            // Button is untoggled (released)
            button->setColour(juce::TextButton::buttonColourId, findColour(juce::TextButton::buttonColourId));
        }
        
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UltimateDistortionAudioProcessorEditor)
};
