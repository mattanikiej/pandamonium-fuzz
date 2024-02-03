/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PandamoniumAudioProcessorEditor::PandamoniumAudioProcessorEditor (PandamoniumAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 500);

    // define slider params
    gainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    gainSlider.setRange(0.0, 24.0, 0.1);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    gainSlider.setPopupDisplayEnabled(true, false, this);
    gainSlider.setTextValueSuffix(" - Gain");
    gainSlider.setValue(1.0);

    fuzzSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    fuzzSlider.setRange(0.0, 30.0, 0.1);
    fuzzSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    fuzzSlider.setPopupDisplayEnabled(true, false, this);
    fuzzSlider.setTextValueSuffix(" - Fuzz");
    fuzzSlider.setValue(1.0);

    volumeSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    volumeSlider.setRange(0.0, 24.0, 0.1);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    volumeSlider.setPopupDisplayEnabled(true, false, this);
    volumeSlider.setTextValueSuffix(" - Volume");
    volumeSlider.setValue(1.0);

    // add sliders to editor
    addAndMakeVisible(&gainSlider);
    addAndMakeVisible(&fuzzSlider);
    addAndMakeVisible(&volumeSlider);

    // add listeners to sliders
    gainSlider.addListener(this);
    fuzzSlider.addListener(this);
    volumeSlider.addListener(this);
}

PandamoniumAudioProcessorEditor::~PandamoniumAudioProcessorEditor()
{
}

//==============================================================================
void PandamoniumAudioProcessorEditor::paint (juce::Graphics& g)
{
    // fill the whole window white
    g.fillAll(juce::Colours::white);

    // set the current drawing colour to black
    g.setColour(juce::Colours::black);

    // set the font size and draw text to the screen
    g.setFont(15.0f);

    g.drawFittedText("Pandamonium Fuzz", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
    
}

void PandamoniumAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    gainSlider.setBounds(40, 30, 20, getHeight() - 60);
    fuzzSlider.setBounds(80, 30, 20, getHeight() - 60);
    volumeSlider.setBounds(120, 30, 20, getHeight() - 60);
}

void PandamoniumAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.setGain(gainSlider.getValue());
    audioProcessor.setFuzz(fuzzSlider.getValue());
    audioProcessor.setVolume(volumeSlider.getValue());
}
