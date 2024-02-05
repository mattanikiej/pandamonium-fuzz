/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using Track = juce::Grid::TrackInfo;
using Fr = juce::Grid::Fr;
using Px = juce::Grid::Px;

PandamoniumLookAndFeel::PandamoniumLookAndFeel()
{
    // sliders
    // setColour (juce::Slider::trackColourId, _ice);
    setColour (juce::Slider::textBoxOutlineColourId, _grey);
    setColour (juce::Slider::textBoxTextColourId, _whitePanda);
    
    // default window settings
    setColour(juce::ResizableWindow::backgroundColourId, _grey);
    setColour(juce::Label::textColourId, _whitePanda);
    
}

void PandamoniumLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width  * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // fill
    g.setColour (_ice);
    g.fillEllipse (rx, ry, rw, rw);

    // outline
    g.setColour (_gold);
    g.drawEllipse (rx, ry, rw, rw, 4.0f);
    
    juce::Path p;
    auto pointerLength = radius * 0.75f;
    auto pointerThickness = 3.0f;
    p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    
    // pointer
    g.setColour (_whitePanda);
    g.fillPath (p);
    
    slider.setColour(juce::Slider::textBoxOutlineColourId, _grey);
}

juce::String ModeSlider::getTextFromValue(double value)
{
    juce::String s;
    if (value == 0.0)
    {
        s = juce::String("Black - Mode");
    }
    else if (value == 1.0)
    {
        s = juce::String("White - Mode");
    }
    else
    {
        s = juce::String("Red - Mode");
    }
    return s;
}

double ModeSlider::getValueFromText(const juce::String &text)
{
    juce::String trimmed = text.trim();
    float num;
    if (trimmed.equalsIgnoreCase("black"))
    {
        num = 0.0;
    }
    else if (trimmed.equalsIgnoreCase("white"))
    {
        num = 1.0;
    }
    else
    {
        num = 2.0;
    }
    return num;
}

//==============================================================================
PandamoniumAudioProcessorEditor::PandamoniumAudioProcessorEditor (PandamoniumAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 400);
    
    // set default look and feel
    setLookAndFeel(&_lookAndFeel);
    
    // title
    _title.setEditable (false);
    _title.setText("Pandamonium Fuzz", juce::NotificationType::dontSendNotification);
    _title.setFont(15.0f);

    // define slider params
    _gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    _gainSlider.setRange(0.0, 24.0, 0.1);
    _gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    _gainSlider.setPopupDisplayEnabled(true, false, this);
    _gainSlider.setTextValueSuffix(" - Gain");
    _gainSlider.setValue(1.0);

    _fuzzSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    _fuzzSlider.setRange(0.0, 30.0, 0.1);
    _fuzzSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    _fuzzSlider.setPopupDisplayEnabled(true, false, this);
    _fuzzSlider.setTextValueSuffix(" - Fuzz");
    _fuzzSlider.setValue(1.0);

    _volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    _volumeSlider.setRange(0.0, 24.0, 0.1);
    _volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    _volumeSlider.setPopupDisplayEnabled(true, false, this);
    _volumeSlider.setTextValueSuffix(" - Volume");
    _volumeSlider.setValue(1.0);

    _modeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    _modeSlider.setRange(0.0, 2.0, 1);
    _modeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    _modeSlider.setPopupDisplayEnabled(true, false, this);

    // add components visible
    addAndMakeVisible (&_title);
    
    addAndMakeVisible(&_gainSlider);
    addAndMakeVisible(&_fuzzSlider);
    addAndMakeVisible(&_volumeSlider);
    addAndMakeVisible(&_modeSlider);

    // add listeners to components
    _gainSlider.addListener(this);
    _fuzzSlider.addListener(this);
    _volumeSlider.addListener(this);
    _modeSlider.addListener(this);
}

PandamoniumAudioProcessorEditor::~PandamoniumAudioProcessorEditor()
{
}

//==============================================================================
void PandamoniumAudioProcessorEditor::paint (juce::Graphics& g)
{
    
}

void PandamoniumAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float sliderWidth = 130.0f;
    
    juce::Grid grid;

    grid.templateRows = { Track (Fr (1)), Track (Fr (1)) };
    grid.templateColumns = { Track (Fr (1)), Track (Fr (1)) };
    
    grid.justifyContent = juce::Grid::JustifyContent::center;
    grid.justifyItems = juce::Grid::JustifyItems::center;
    
    grid.alignContent = juce::Grid::AlignContent::center;
    grid.alignItems = juce::Grid::AlignItems::center;

    grid.items = {
        juce::GridItem (_gainSlider).withWidth(sliderWidth).withHeight(sliderWidth),
        juce::GridItem (_fuzzSlider).withWidth(sliderWidth).withHeight(sliderWidth),
        juce::GridItem (_volumeSlider).withWidth(sliderWidth).withHeight(sliderWidth),
        juce::GridItem (_modeSlider).withWidth(sliderWidth).withHeight(sliderWidth),
    };

    grid.performLayout (getLocalBounds());
    
}

void PandamoniumAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.setGain(_gainSlider.getValue());
    audioProcessor.setFuzz(_fuzzSlider.getValue());
    audioProcessor.setVolume(_volumeSlider.getValue());

    int mode = _modeSlider.getValue();
    if (mode == 0)
    {
        audioProcessor.setMode(ExpSoftClipping);
    }
    else if (mode == 1)
    {
        audioProcessor.setMode(SoftClipping);
    }
    else
    {
        audioProcessor.setMode(HardClipping);
    }

}
