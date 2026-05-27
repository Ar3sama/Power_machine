#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "parameters/ParameterIDs.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr, "Parameters", Parameters::createParameterLayout())
{
}


AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void AudioPluginAudioProcessor::releaseResources()
{
    dsp.reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    // clears any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const auto bypass = parameters.getRawParameterValue (ParameterIDs::bypass)->load() > 0.5f;

    if (bypass)
    {
        inputMeterLevel.store (0.0f);
        outputMeterLevel.store (0.0f);
        saturationMeterLevel.store (0.0f);
        return;
    }

    dsp.processBlock (buffer, readSettings());

    inputMeterLevel.store (dsp.getLastInputLevel());
    outputMeterLevel.store (dsp.getLastOutputLevel());
    saturationMeterLevel.store (dsp.getLastSaturationAmount());
}

PluginDSP::Settings AudioPluginAudioProcessor::readSettings() const
{
    PluginDSP::Settings settings;

    settings.inputDriveDb = parameters.getRawParameterValue (ParameterIDs::inputDrive)->load();
    settings.stage1Drive = parameters.getRawParameterValue (ParameterIDs::stage1Drive)->load() / 100.0f;
    settings.stage2Drive = parameters.getRawParameterValue (ParameterIDs::stage2Drive)->load() / 100.0f;
    settings.tone = parameters.getRawParameterValue (ParameterIDs::tone)->load() / 100.0f;
    settings.presence = parameters.getRawParameterValue (ParameterIDs::presence)->load() / 100.0f;
    settings.mix = parameters.getRawParameterValue (ParameterIDs::mix)->load() / 100.0f;
    settings.outputGainDb = parameters.getRawParameterValue (ParameterIDs::outputGain)->load();
    settings.analogCharacter = parameters.getRawParameterValue (ParameterIDs::analogCharacter)->load() / 100.0f;
    settings.autoGainEnabled = parameters.getRawParameterValue (ParameterIDs::autoGain)->load() > 0.5f;

    switch (static_cast<int> (parameters.getRawParameterValue (ParameterIDs::oversampling)->load()))
    {
        case 1: settings.oversamplingMode = OversamplingProcessor::Mode::x2; break;
        case 2: settings.oversamplingMode = OversamplingProcessor::Mode::x4; break;
        case 3: settings.oversamplingMode = OversamplingProcessor::Mode::x8; break;
        default: settings.oversamplingMode = OversamplingProcessor::Mode::off; break;
    }

    return settings;
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    if (xml != nullptr)
        copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
