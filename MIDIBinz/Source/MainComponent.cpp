/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class AltLookAndFeel : public LookAndFeel_V3
{
public:
    AltLookAndFeel()
    {
        setColour (Slider::rotarySliderFillColourId, Colours::red);
    }
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        const float radius = jmin (width / 2, height / 2) - 4.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // fill
        g.setColour (Colours::orange);
        g.fillEllipse (rx, ry, rw, rw);
        
        // outline
        g.setColour (Colours::red);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);
        
        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer
        g.setColour (Colours::yellow);
        g.fillPath (p);
    }
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override
    {
        Rectangle<int> buttonArea = button.getLocalBounds();
        const int edge = 4;
        
        buttonArea.removeFromLeft (edge);
        buttonArea.removeFromTop (edge);
        
        // shadow
        g.setColour (Colours::darkgrey.withAlpha (0.5f));
        g.fillRect (buttonArea);

        const int offset = isButtonDown ? -edge / 2 : -edge;
        buttonArea.translate (offset, offset);
        
        g.setColour (backgroundColour);
        g.fillRect (buttonArea);
    }
    
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        Font font (getTextButtonFont (button, button.getHeight()));
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                        : TextButton::textColourOffId)
                     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        
        const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;
        
        const int fontHeight = roundToInt (font.getHeight() * 0.6f);
        const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        const int edge = 4;
        const int offset = isButtonDown ? edge / 2 : 0;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
                              Justification::centred, 2);
    }
    
};



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent, 
                               public Slider::Listener,
                               public ButtonListener,
                               private Timer
{
public:
    //==============================================================================
    MainContentComponent() : forwardFFT (fftOrder, false)
    {
        setSize (width, height);
        
        
        
        //Level Slider
        levelSlider.setSliderStyle(Slider::Rotary);
        
        levelSlider.setRange (0.0, 0.25);
        levelSlider.setTextBoxStyle (Slider::TextBoxRight, false, 100, 20);
        levelLabel.setText ("Noise Level", dontSendNotification);
        levelLabel.setColour(slider1Label.textColourId, Colours::white);
        levelLabel.attachToComponent(&levelSlider, true);
        addAndMakeVisible (levelSlider);
        addAndMakeVisible (levelLabel);
        
        
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 0);
        startTimerHz (60);
        
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

                if (bufferToFill.buffer->getNumChannels() > 0)
        {
            const float* channelData = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
             
            for (int i = 0; i < bufferToFill.numSamples; ++i)
            {
                pushNextSampleIntoFifo (channelData[i]);
            }
        }
    }
    
    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            printFFT();
            nextFFTBlockReady = false;
            repaint();
        }
    }
    void pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }
            
            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }
    void printFFT()
    {
        forwardFFT.performFrequencyOnlyForwardTransform (fftData);
        std::cout << *fftData << std::endl;
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
        g.fillAll (Colour (0xffa9a9a9));


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
        levelSlider.setBounds(100, 40, 200, 200);
        recordButton.setBounds(100, 450, width - 110, 20);
        
        
    }
    void sliderValueChanged (Slider* slider) override
    {
    }
    void buttonClicked(Button* buttonThatWasClicked) override
    {
        
    }
    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };

private:
    //==============================================================================

    // Your private member variables go here...

    FFT forwardFFT;

    float fifo [fftSize];
    float fftData [2 * fftSize];
    int fifoIndex;
    bool nextFFTBlockReady;
    float audioOutput = 0.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    int width = 320;
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
    LookAndFeel_V3 otherLookAndFeel;
    
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
