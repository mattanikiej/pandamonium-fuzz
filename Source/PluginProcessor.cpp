/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
PandamoniumAudioProcessor::PandamoniumAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),

    _parameters(*this, nullptr, juce::Identifier("Pandamonium"),
        {
            std::make_unique<juce::AudioParameterFloat>("gain",            // parameterID
                                                         "Gain",            // parameter name
                                                         0.0f,              // minimum value
                                                         24.0f,              // maximum value
                                                         1.0f),             // default value


            std::make_unique<juce::AudioParameterFloat>("fuzz",            // parameterID
                                                         "Fuzz",            // parameter name
                                                         0.0f,              // minimum value
                                                         30.0f,              // maximum value
                                                         15.0f),             // default value

            std::make_unique<juce::AudioParameterFloat>("volume",            // parameterID
                                                         "Volume",            // parameter name
                                                         0.0f,              // minimum value
                                                         24.0f,              // maximum value
                                                         1.0f),             // default value

            std::make_unique<juce::AudioParameterInt>("mode",            // parameterID
                                                         "Mode",            // parameter name
                                                         0,              // minimum value
                                                         2,              // maximum value
                                                         0),             // default value
        })
#endif
{
    _gain = _parameters.getRawParameterValue("gain");
    _fuzz = _parameters.getRawParameterValue("fuzz");
    _volume = _parameters.getRawParameterValue("volume");
    _mode = _parameters.getRawParameterValue("mode");
}

PandamoniumAudioProcessor::~PandamoniumAudioProcessor()
{
}

//==============================================================================
const juce::String PandamoniumAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PandamoniumAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PandamoniumAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PandamoniumAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PandamoniumAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PandamoniumAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PandamoniumAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PandamoniumAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PandamoniumAudioProcessor::getProgramName (int index)
{
    return {};
}

void PandamoniumAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PandamoniumAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PandamoniumAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PandamoniumAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PandamoniumAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) 
        {
            auto input = channelData[sample];

            float gain = powf(10.0f, *_gain / 20.f); // decibels

            float x = input * gain;

            // add distortion
            if (*_mode == 0)
            {
                if (x < 0)
                {
                    x = -1.0f + exp(x * *_fuzz);
                }
                else
                {
                    x = 1.0f - exp(-x * *_fuzz);
                }
            }

            else if (*_mode == 1)
            {
                float threshold = 1.0f / 3.0f;
                float fuzz = 6.0f * (*_fuzz / 30.0f);

                if (x > threshold)
                {
                    if (x > 2.0f * threshold)
                    {
                        x = 1.0f;
                    }
                    else
                    {
                        x = (3.0f - (2.0f - fuzz*x) * (2.0f - fuzz*x)) / 3.0f;
                    }
                }
                else if (x < -threshold)
                {
                    if (x < -2.0f * threshold)
                    {
                        x = -1.0f;
                    }
                    else
                    {
                        x = -(3.0f - (2.0f - fuzz*x) * (2.0f - fuzz*x)) / 3.0f;
                    }
                }
                else
                {
                    x *= 2.0f;
                }

                x /= 2.0f;
           
            }

            else
            {
                float threshold = 1 - *_fuzz / 30.0f;

                if (x > threshold) 
                {
                    x = 1;
                }
                else if (x < -threshold)
                {
                    x = -1;
                }
            }
                    
            float volume = powf(10.0f, *_volume / 20.f); // decibels
            auto output = x * volume;

            channelData[sample] = output;
        }
    }
}

//==============================================================================
bool PandamoniumAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PandamoniumAudioProcessor::createEditor()
{
    return new PandamoniumAudioProcessorEditor (*this, _parameters);
}

//==============================================================================
void PandamoniumAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = _parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PandamoniumAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(_parameters.state.getType()))
            _parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PandamoniumAudioProcessor();
}

//==============================================================================
float PandamoniumAudioProcessor::getGain()
{
    return *_gain;
}

void PandamoniumAudioProcessor::setGain(float gain)
{
    *_gain = gain;
}

float PandamoniumAudioProcessor::getFuzz()
{
    return *_fuzz;
}

void PandamoniumAudioProcessor::setFuzz(float fuzz)
{
    *_fuzz = fuzz;
}

float PandamoniumAudioProcessor::getVolume()
{
    return *_volume;
}

void PandamoniumAudioProcessor::setVolume(float volume)
{
    *_volume = volume;
}

float PandamoniumAudioProcessor::getMode()
{
    return *_mode;
}

void PandamoniumAudioProcessor::setMode(float mode)
{
    *_mode = mode;
}

