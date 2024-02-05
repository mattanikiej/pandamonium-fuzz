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
class PandamoniumAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         private juce::Slider::Listener
{
public:
    PandamoniumAudioProcessorEditor (PandamoniumAudioProcessor&);
    ~PandamoniumAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PandamoniumAudioProcessor& audioProcessor;

    juce::Slider gainSlider;
    juce::Slider fuzzSlider;
    juce::Slider volumeSlider;
    juce::Slider modeSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PandamoniumAudioProcessorEditor)
};
