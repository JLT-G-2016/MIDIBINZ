/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent, 
                               public Slider::Listener,
                               public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent()
    {
        setSize (width, height);
        
        
        
        //Level Slider
        levelSlider.setRange (0.0, 0.25);
        levelSlider.setTextBoxStyle (Slider::TextBoxRight, false, 100, 20);
        levelLabel.setText ("Noise Level", dontSendNotification);
        levelLabel.setColour(slider1Label.textColourId, Colours::white);
        levelLabel.attachToComponent(&levelSlider, true);
        addAndMakeVisible (levelSlider);
        addAndMakeVisible (levelLabel);
        
        
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
        //Slider1 
        addAndMakeVisible(slider1);
        slider1.setRange(20, 20000.0);
        slider1.setTextValueSuffix(" Hz");
    
    
        slider1Label.setText("Slider1", dontSendNotification);
        slider1Label.setColour(slider1Label.textColourId, Colours::white);
        slider1Label.attachToComponent(&slider1, true);
        addAndMakeVisible(slider1Label);
    
        // Slider2 
        addAndMakeVisible (slider2);
        slider2.setRange (20, 20000.0);
        slider2.setTextValueSuffix (" Hz");
        slider2.addListener (this);
        addAndMakeVisible (slider2Label);
        slider2Label.setText ("Slider2", dontSendNotification);
        slider2Label.setColour(slider2Label.textColourId, Colours::white);
        slider2Label.attachToComponent (&slider2, true);
    
        // Slider3
        addAndMakeVisible (slider3);
        slider3.setRange (20, 20000.0);
        slider3.setTextValueSuffix (" Hz");
        slider3.addListener (this);
        addAndMakeVisible (slider3Label);
        slider3Label.setText ("Slider3", dontSendNotification);
        slider3Label.setColour(slider3Label.textColourId, Colours::white);
        slider3Label.attachToComponent (&slider3, true);
    
        // Slider4
        addAndMakeVisible (slider4);
        slider4.setRange (20, 20000.0);
        slider4.setTextValueSuffix (" Hz");
        slider4.addListener (this);
        addAndMakeVisible (slider4Label);
        slider4Label.setText ("Slider4", dontSendNotification);
        slider4Label.setColour(slider4Label.textColourId, Colours::white);
        slider4Label.attachToComponent (&slider4, true);
    
        // Record Button
        addAndMakeVisible(recordButton);
        
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        // For more details, see the help for AudioProcessor::prepareToPlay()
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // Your audio-processing code goes here!

        // For more details, see the help for AudioProcessor::getNextAudioBlock()

        // Right now we are not producing any data, in which case we need to clear the buffer
        // (to prevent the output of random noise)
        AudioIODevice* device = deviceManager.getCurrentAudioDevice();
        const BigInteger activeInputChannels = device->getActiveInputChannels();
        const BigInteger activeOutputChannels = device->getActiveOutputChannels();
        const int maxInputChannels = activeInputChannels.getHighestBit() + 1;
        const int maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

        const float level = (float) levelSlider.getValue();
        
        for (int channel = 0; channel < maxOutputChannels; ++channel)
        {
            if ((! activeOutputChannels[channel]) || maxInputChannels == 0)
            {
                bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                const int actualInputChannel = channel % maxInputChannels; // [1]
                
                if (! activeInputChannels[channel]) // [2]
                {
                    bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
                }
                else // [3]
                {
                    const float* inBuffer = bufferToFill.buffer->getReadPointer (actualInputChannel,
                                                                                 bufferToFill.startSample);
                    float* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
                    
                    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                        outBuffer[sample] = inBuffer[sample] * level;
                }
            }
        }
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colour (0xff001423));


        // You can add your drawing code here!
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        const int sliderLeft = 60;
        slider1.setBounds (sliderLeft, 200, width - sliderLeft - 10, 20);
        slider2.setBounds (sliderLeft, 240, width - sliderLeft - 10, 20);
        slider3.setBounds (sliderLeft, 280, width - sliderLeft - 10, 20);
        slider4.setBounds (sliderLeft, 320, width - sliderLeft - 10, 20);
        levelSlider.setBounds (100, 140, width - 100 - 10, 20);
        recordButton.setBounds(100, 450, width - 110, 20);
        
        
    }
    void sliderValueChanged (Slider* slider) override
    {
    }
    void buttonClicked(Button* buttonThatWasClicked) override
    {
        
    }

private:
    //==============================================================================

    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    int width = 300;
    int height = 600;
    Slider slider1;
    Label slider1Label;
    Slider slider2;
    Label slider2Label;
    Slider slider3;
    Label slider3Label;
    Slider slider4;
    Label slider4Label;
    Random random;
    Slider levelSlider;
    Label levelLabel;
    TextButton recordButton;
    
    
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
