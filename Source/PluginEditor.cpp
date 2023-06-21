/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UltimateDistortionAudioProcessorEditor::UltimateDistortionAudioProcessorEditor (UltimateDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), gainAttachment(p.treeState, "GAIN", gainKnob), mixAttachment(p.treeState, "MIX", mixKnob), toneAttachment(p.treeState, "TONE", toneKnob), outputAttachment(p.treeState, "OUTPUT", outputKnob)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);
    setSize (500, 250);
    setResizeLimits(500, 250, 700, 350);
    getConstrainer()->setFixedAspectRatio(2.0);
    
    addAndMakeVisible(modeBar);
    modeBar.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    modeBar.setVisible(false);
        
    addAndMakeVisible(modeButton1);
    modeButton1.setClickingTogglesState(true);
    modeButton1.onClick = [this] { selectMode(&modeButton1, 0.0); };
    modeButton1.setRadioGroupId(1001);
    modeButton1.setButtonText("Full");
    addAndMakeVisible(modeButton2);
    modeButton2.setClickingTogglesState(true);
    modeButton2.onClick = [this] { selectMode(&modeButton2, 1.0 / 7.0); };
    modeButton2.setRadioGroupId(1001);
    modeButton2.setButtonText("Half");
    addAndMakeVisible(modeButton3);
    modeButton3.setClickingTogglesState(true);
    modeButton3.onClick = [this] { selectMode(&modeButton3, 2.0 / 7.0); };
    modeButton3.setRadioGroupId(1001);
    modeButton3.setButtonText("Hard");
    modeButton3.triggerClick();
    addAndMakeVisible(modeButton4);
    modeButton4.setClickingTogglesState(true);
    modeButton4.onClick = [this] { selectMode(&modeButton4, 3.0 / 7.0); };
    modeButton4.setRadioGroupId(1001);
    modeButton4.setButtonText("Soft1");
    addAndMakeVisible(modeButton5);
    modeButton5.setClickingTogglesState(true);
    modeButton5.onClick = [this] { selectMode(&modeButton5, 4.0 / 7.0); };
    modeButton5.setRadioGroupId(1001);
    modeButton5.setButtonText("Soft2");
    addAndMakeVisible(modeButton6);
    modeButton6.setClickingTogglesState(true);
    modeButton6.onClick = [this] { selectMode(&modeButton6, 5.0 / 7.0); };
    modeButton6.setRadioGroupId(1001);
    modeButton6.setButtonText("Soft3");
    addAndMakeVisible(modeButton7);
    modeButton7.setClickingTogglesState(true);
    modeButton7.onClick = [this] { selectMode(&modeButton7, 6.0 / 7.0); };
    modeButton7.setRadioGroupId(1001);
    modeButton7.setButtonText("Sat");
    addAndMakeVisible(modeButton8);
    modeButton8.setClickingTogglesState(true);
    modeButton8.onClick = [this] { selectMode(&modeButton8, 1.0); };
    modeButton8.setRadioGroupId(1001);
    modeButton8.setButtonText("Bit");
    
    addAndMakeVisible(gainKnob);
    gainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 6, getHeight() / 7);
    gainKnob.setRange(0.0, 24.0);
    gainKnob.setNumDecimalPlacesToDisplay(1);
    gainKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    gainKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
    gainKnob.setDoubleClickReturnValue(true, 0.0);
    gainKnob.setMouseDragSensitivity(200);
    
    addAndMakeVisible(mixKnob);
    mixKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 6, getHeight() / 7);
    mixKnob.setRange(0.0, 1.0);
    mixKnob.setValue(0.0);
    mixKnob.setNumDecimalPlacesToDisplay(1);
    mixKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    mixKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
    mixKnob.setDoubleClickReturnValue(true, 0.0);
    mixKnob.setMouseDragSensitivity(200);
    
    addAndMakeVisible(toneKnob);
    toneKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    toneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 6, getHeight() / 7);
    toneKnob.setRange(0.0, 20000.0);
    toneKnob.setValue(20000.0);
    toneKnob.setNumDecimalPlacesToDisplay(1);
    toneKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    toneKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
    toneKnob.setDoubleClickReturnValue(true, 20000.0);
    toneKnob.setMouseDragSensitivity(200);
    
    addAndMakeVisible(outputKnob);
    outputKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    outputKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 6, getHeight() / 7);
    outputKnob.setRange(-24.0, 24.0);
    outputKnob.setNumDecimalPlacesToDisplay(1);
    outputKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    outputKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::whitesmoke.withAlpha(0.0f));
    outputKnob.setDoubleClickReturnValue(true, 0.0);
    outputKnob.setMouseDragSensitivity(200);
    
    addAndMakeVisible(modeLabel);
    modeLabel.setText("Mode", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Intensity", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainKnob, false);
    
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixKnob, false);
    
    addAndMakeVisible(toneLabel);
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.setJustificationType(juce::Justification::centred);
    toneLabel.attachToComponent(&toneKnob, false);
    
    addAndMakeVisible(outputLabel);
    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::centred);
    outputLabel.attachToComponent(&outputKnob, false);
    
}

UltimateDistortionAudioProcessorEditor::~UltimateDistortionAudioProcessorEditor()
{
}

//==============================================================================
void UltimateDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void UltimateDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto area = getLocalBounds();
    
    auto sideWidth = getWidth() / 20;
    area.removeFromLeft(sideWidth);
    area.removeFromRight(sideWidth);
    
    auto headerFooterHeight = getHeight() / 10;
    modeLabel.setBounds(area.removeFromTop(headerFooterHeight));
    area.removeFromBottom(headerFooterHeight);
    
    auto buttonHeight = getHeight() / 8;
    auto modeBarArea = area.removeFromTop(buttonHeight);
    modeBar.setBounds(modeBarArea);
    
    auto w = modeBarArea.getWidth() / 8;
    modeButton1.setBounds(modeBarArea.removeFromLeft(w));
    modeButton2.setBounds(modeBarArea.removeFromLeft(w));
    modeButton3.setBounds(modeBarArea.removeFromLeft(w));
    modeButton4.setBounds(modeBarArea.removeFromLeft(w));
    modeButton5.setBounds(modeBarArea.removeFromLeft(w));
    modeButton6.setBounds(modeBarArea.removeFromLeft(w));
    modeButton7.setBounds(modeBarArea.removeFromLeft(w));
    modeButton8.setBounds(modeBarArea);
    
    area.removeFromTop(headerFooterHeight * 1.5);
    
    auto widthWithSideRemoved = area.getWidth();
    gainKnob.setBounds(area.removeFromLeft(widthWithSideRemoved / 4));
    mixKnob.setBounds(area.removeFromLeft(widthWithSideRemoved / 4));
    toneKnob.setBounds(area.removeFromLeft(widthWithSideRemoved / 4));
    outputKnob.setBounds(area.removeFromLeft(widthWithSideRemoved / 4));
}
