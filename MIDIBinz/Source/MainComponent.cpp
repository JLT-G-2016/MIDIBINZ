/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Filter.h" //Check

struct MidiDeviceListEntry : ReferenceCountedObject
{
    MidiDeviceListEntry (const String& deviceName) : name (deviceName) {}
    
    String name;
    ScopedPointer<MidiInput> inDevice;
    ScopedPointer<MidiOutput> outDevice;
    
    typedef ReferenceCountedObjectPtr<MidiDeviceListEntry> Ptr;
};

//==============================================================================
struct MidiCallbackMessage : public Message
{
    MidiCallbackMessage (const MidiMessage& msg) : message (msg) {}
    MidiMessage message;
};
class MidiDeviceListBox;
class MainContentComponent;

class AltLookAndFeel : public LookAndFeel_V3
{
public:
    AltLookAndFeel()
    {
        setColour (Slider::rotarySliderFillColourId, Colours::red);
        //setColour(TextButton::buttonColourId, Colours::blue);
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
        g.setColour (Colours::royalblue);
        g.fillEllipse (rx, ry, rw, rw);
        
        // outline
        g.setColour (Colours::blue);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);
        
        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer
        g.setColour (Colours::darkblue);
        g.fillPath (p);
    }
    
    /*
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
        
        g.setColour (Colour(0xfffd0000));
        g.fillRect (buttonArea);
        if (isButtonDown)
        {
            g.setColour (Colour(0xffbe0000));
            g.fillRect (buttonArea);
        }
         
    }
    */
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        if(button.getName() == "Record")
        {
            button.setColour(TextButton::buttonColourId , Colour(0xfffd0000));
            button.setColour(TextButton::textColourOffId, Colours::white);
        }
        if(button.getName() == "Connect")
        {
            button.setColour(TextButton::buttonColourId , Colour(0xff0087ff));
            button.setColour(TextButton::textColourOffId, Colours::white);
        }
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
                               private Timer,
                               private MidiInputCallback,
                               private MessageListener

{
public:
    //==============================================================================
    MainContentComponent() : forwardFFT (fftOrder, false),
                             midiInputLabel ("Midi Input Label", "MIDI Input:"),
                             midiOutputLabel ("Midi Output Label", "MIDI Output:")
                             //midiInputSelector (new MidiDeviceListBox ("Midi Input Selector", *this, true)),
                             //midiOutputSelector (new MidiDeviceListBox ("Midi Input Selector", *this, false))
    
    {
        
        setSize (width, height);
        buttonUp = Colour (0xffe30000);
        buttonDown = Colour (0xff800000);
        buttonText = "Hold To Record";
        
        setLookAndFeel (&altLookAndFeel);
        //Level Slider
        //levelSlider.setSliderStyle(Slider::Rotary);
        
        levelSlider.setRange (0.0, 0.25);
        levelSlider.setValue(0.10);
        levelSlider.setTextBoxStyle (Slider::NoTextBox, false, 100, 20);
        levelLabel.setText ("Mic Level", dontSendNotification);
        levelLabel.setColour(slider1Label.textColourId, Colours::white);
        levelLabel.attachToComponent(&levelSlider, false);
        addAndMakeVisible (levelSlider);
        addAndMakeVisible (levelLabel);
        
        
        // Sidebar Prototyping
        //sidebar.setColour(TextButton::buttonColourId, Colours::white);
        //addAndMakeVisible(sidebar);
        //addAndMakeVisible(sideItemA);
        
        
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
        startTimerHz (60);
        
        //Slider1 
        addAndMakeVisible(slider1);
        slider1.setRange(20, 20000.0);
        slider1.setTextValueSuffix(" Hz");
        slider1.setSliderStyle(Slider::Rotary);
        slider1.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider1Label.setText("Slider1", dontSendNotification);
        slider1Label.setColour(slider1Label.textColourId, Colours::white);
        //slider1Label.attachToComponent(&slider1, false);
        addAndMakeVisible(slider1Label);
    
        // Slider2 
        addAndMakeVisible (slider2);
        slider2.setRange (20, 20000.0);
        slider2.setTextValueSuffix (" Hz");
        slider2.setSliderStyle(Slider::Rotary);
        slider2.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider2.addListener (this);
        addAndMakeVisible (slider2Label);
        slider2Label.setText ("Slider2", dontSendNotification);
        slider2Label.setColour(slider2Label.textColourId, Colours::white);
        //slider2Label.attachToComponent (&slider2, false);
    
        // Slider3
        addAndMakeVisible (slider3);
        slider3.setRange (20, 20000.0);
        slider3.setTextValueSuffix (" Hz");
        slider3.setSliderStyle(Slider::Rotary);
        slider3.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider3.addListener (this);
        addAndMakeVisible (slider3Label);
        slider3Label.setText ("Slider3", dontSendNotification);
        slider3Label.setColour(slider3Label.textColourId, Colours::white);
        //slider3Label.attachToComponent (&slider3, false);
    
        // Slider4
        addAndMakeVisible (slider4);
        slider4.setRange (20, 20000.0);
        slider4.setTextValueSuffix (" Hz");
        slider4.setSliderStyle(Slider::Rotary);
        slider4.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider4.addListener (this);
        addAndMakeVisible (slider4Label);
        slider4Label.setText ("Slider4", dontSendNotification);
        slider4Label.setColour(slider4Label.textColourId, Colours::white);
        //slider4Label.attachToComponent (&slider4, false);
    
        // Slider5
        addAndMakeVisible (slider5);
        slider5.setRange (20, 20000.0);
        slider5.setTextValueSuffix (" Hz");
        slider5.setSliderStyle(Slider::Rotary);
        slider5.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider5.addListener (this);
        addAndMakeVisible (slider5Label);
        slider5Label.setText ("Slider5", dontSendNotification);
        slider5Label.setColour(slider5Label.textColourId, Colours::white);
     
        // Slider6
        addAndMakeVisible (slider6);
        slider6.setRange (20, 20000.0);
        slider6.setTextValueSuffix (" Hz");
        slider6.setSliderStyle(Slider::Rotary);
        slider6.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        slider6.addListener (this);
        addAndMakeVisible (slider6Label);
        slider6Label.setText ("Slider6", dontSendNotification);
        slider6Label.setColour(slider6Label.textColourId, Colours::white);
        
        // Record Button
        recordButton.setName("Record");
        recordButton.setButtonText(buttonText);
        addAndMakeVisible(recordButton);
        
        // Connect Button
        connectButton.setName("Connect");
        connectButton.setButtonText("Connect");
        addAndMakeVisible(connectButton);
        connectButton.addListener(this);
        
        // Title Label
        titleLabel.setText("MIDIBinz", dontSendNotification);
        Font textFont;
        textFont.setTypefaceName("BrushScript");
        textFont.setBold(true);
        textFont.setUnderline(true);
        textFont.setHeight(30);
        titleLabel.setFont(textFont);
        titleLabel.setColour(titleLabel.textColourId, Colour(0xff2ca1ff));
        addAndMakeVisible(titleLabel);
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
        recording = false;
        if (recordButton.getState() == Button::buttonDown)
        {
            if (bufferToFill.buffer->getNumChannels() > 0)
            {
                const float* channelData = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
             
                
                for (int i = 0; i < bufferToFill.numSamples; ++i)
                {
                    pushNextSampleIntoFilter (channelData[i]);
                }
            }
        }
    }
    // Takes Amp values
    //
    void ConvertToMidi(float* rawData)
    {
        int midiVal = (int)*rawData;
        jmap(abs(midiVal*100),0,50,1,127);
        MidiMessage msg = MidiMessage::controllerEvent(1, 20, midiVal);
        midiMsg = msg.getDescription();
        repaint();
        
    }
    void timerCallback() override
    {
        if (nextFFTBlockReady) // If bin is filled
        {
            // get max vals and clear filter bank arrays for refilling
            unscaledAvg[0] = getAverageBinAmplitude(filtBank1);
            zeromem(filtBank1, sizeof(filtBank1));
            unscaledAvg[1] = getAverageBinAmplitude(filtBank2);
            zeromem(filtBank2, sizeof(filtBank2));
            unscaledAvg[2] = getAverageBinAmplitude(filtBank3);
            zeromem(filtBank3, sizeof(filtBank3));
            unscaledAvg[3] = getAverageBinAmplitude(filtBank4);
            zeromem(filtBank4, sizeof(filtBank4));
            
            // Set false to fill filter banks again
            nextFFTBlockReady = false;
            repaint();
            for(int i = 0; i < 4; i++) {
                printf("%f\n",unscaledAvg[i]);
            }
        }
    }
    
    
    void pushNextSampleIntoFilter (float sample) noexcept
    {
        //Process Filter Function & Fill Bank
        if(fifoIndex == blockSize) // strictly comparitive statement
        {
            nextFFTBlockReady = true;
            fifoIndex = 0;
        }
        
        else
        {
            *filtBank1 += myFilter1->do_sample(sample);
            *filtBank2 += myFilter2->do_sample(sample);
            *filtBank3 += myFilter3->do_sample(sample);
            *filtBank4 += myFilter4->do_sample(sample);
            
            fifoIndex++; // count to get size to process (1024 for now)
        }
    }
    
    /* FUNCTION TO FIND AVERAGE OF EACH FILTER BIN */
    // Return MAX to test
    float getAverageBinAmplitude (float * bank)
    {
        // Temporary Test
        // Get max of each array
        // Must get average to avoid randomly loud spikes. Unless that's what you're going for, then cool for you. Bitch.
        
        return findMaximum(bank, sizeof(bank)); // return the max of Type array of size 1024 elements
        // return *bank;
    }
    
//    void pushNextSampleIntoFifo (float sample) noexcept
//    {
//        // if the fifo contains enough data, set a flag to say
//        // that the next line should now be rendered..
//        if (fifoIndex == fftSize)
//        {
//            if (! nextFFTBlockReady)
//            {
//                zeromem (fftData, sizeof (fftData));
//                memcpy (fftData, fifo, sizeof (fifo));
//                nextFFTBlockReady = true;
//            }
//            
//            fifoIndex = 0;
//        }
//
//        fifo[fifoIndex++] = sample;
//        
//    }
    void printFFT()
    {
        forwardFFT.performRealOnlyForwardTransform(fftData);
        
        ConvertToMidi(fftData);
        std::cout << recordButton.getName() << std::endl;
        std::cout << midiMsg << std::endl;
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
        g.fillAll (Colour (0xff48494a));


        // You can add your drawing code here!
        if (recordButton.getState() == Button::buttonDown)
        {
            buttonText = "Recording.....";
        }
        else
        {
            buttonText = "Hold To Record";
        }
        recordButton.setButtonText(buttonText);
        g.drawText(midiMsg,10, 40, 200, 860, true);
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        Rectangle<int> area = getLocalBounds();
        
       
        const int sliderLeft = 15;
        const int buttonSize = 90;
        const int buttonRow = 130;
        slider1.setBounds (sliderLeft-10, buttonRow-10, buttonSize, buttonSize);
        slider2.setBounds (sliderLeft + 100, buttonRow-60, buttonSize, buttonSize);
        slider3.setBounds (sliderLeft+210, buttonRow-10, buttonSize, buttonSize);
        slider4.setBounds (sliderLeft-10, buttonRow+140, buttonSize, buttonSize);
        slider5.setBounds (sliderLeft+100, buttonRow+180, buttonSize, buttonSize);
        slider6.setBounds (sliderLeft+210, buttonRow+140, buttonSize, buttonSize);
        levelSlider.setBounds(20, 438, 280, buttonSize-75);
        recordButton.setBounds(5, 485, 310, 110);
        
        slider1Label.setBounds(sliderLeft+10, buttonRow-25, 50, 15);
        slider2Label.setBounds(sliderLeft+120, buttonRow-75, 50, 15);
        slider3Label.setBounds(sliderLeft+230, buttonRow-25, 50, 15);
        slider4Label.setBounds(sliderLeft+10, buttonRow+125, 50, 15);
        slider5Label.setBounds(sliderLeft+120, buttonRow+165, 50, 15);
        slider6Label.setBounds(sliderLeft+230, buttonRow+125, 50, 15);
        
        titleLabel.setBounds(95,-20,300, 100);
        connectButton.setBounds(122, 190, 80, 80);
        
        
    }
    void sliderValueChanged (Slider* slider) override
    {
    }
    
    
    void sendToOutputs(const MidiMessage& msg)
    {
        for (int i = 0; i < midiOutputs.size(); ++i)
            if (midiOutputs[i]->outDevice != nullptr)
                midiOutputs[i]->outDevice->sendMessageNow (msg);
        
        const String mesg = (String) *msg.getRawData();
        //myMsg = msg.getDescription();
        
        repaint();
        
    }
    
    //==============================================================================
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage &message) override
    {
        // This is called on the MIDI thread
        
        if (message.isNoteOnOrOff())
            postMessage (new MidiCallbackMessage (message));
    }
    
    //==============================================================================
    void handleMessage (const Message& msg) override
    {
        // This is called on the message loop
        
        const MidiMessage& mm = dynamic_cast<const MidiCallbackMessage&> (msg).message;
        String midiString;
        midiString << (mm.isNoteOn() ? String ("Note on: ") : String ("Note off: "));
        midiString << (MidiMessage::getMidiNoteName (mm.getNoteNumber(), true, true, true));
        midiString << (String (" vel = "));
        midiString << static_cast<int>(mm.getVelocity());
        midiString << "\n";
        
        //midiMonitor.insertTextAtCaret (midiString);
    }
    
    
    int getNumMidiInputs() const noexcept
    {
        return midiInputs.size();
    }
    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }
    
    //==============================================================================
    int getNumMidiOutputs() const noexcept
    {
        return midiOutputs.size();
    }
    void openDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice == nullptr);
            midiInputs[index]->inDevice = MidiInput::openDevice (index, this);
            
            if (midiInputs[index]->inDevice == nullptr)
            {
                DBG ("MainContentComponent::openDevice: open input device for index = " << index << " failed!" );
                return;
            }
            
            midiInputs[index]->inDevice->start();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice == nullptr);
            midiOutputs[index]->outDevice = MidiOutput::openDevice (index);
            
            if (midiOutputs[index]->outDevice == nullptr)
                DBG ("MainContentComponent::openDevice: open output device for index = " << index << " failed!" );
        }
    }
    
    
    void closeDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice != nullptr);
            midiInputs[index]->inDevice->stop();
            midiInputs[index]->inDevice = nullptr;
        }
        else
        {
            jassert (midiOutputs[index]->outDevice != nullptr);
            midiOutputs[index]->outDevice = nullptr;
        }
    }
    
    void buttonClicked(Button* buttonThatWasClicked) override
    {
        //std::cout << buttonThatWasClicked->getName() << std::endl;
        if(buttonThatWasClicked->getName() == "Connect")
        {
            slider1.setVisible(false);
            slider1Label.setVisible(false);
            slider2.setVisible(false);
            slider2Label.setVisible(false);
            slider3.setVisible(false);
            slider3Label.setVisible(false);
            slider4.setVisible(false);
            slider4Label.setVisible(false);
            slider5.setVisible(false);
            slider5Label.setVisible(false);
            slider6.setVisible(false);
            slider6Label.setVisible(false);
            recordButton.setVisible(false);
            levelSlider.setVisible(false);
            connectButton.setVisible(false);
            
            
        }
          
    }
    
    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };

private:
    //==============================================================================

    // Your private member variables go here...

    
    Filter *myFilter1 = new Filter(BPF, 51, 44.1, .05, .2);
    Filter *myFilter2 = new Filter(BPF, 51, 44.1, .2, 1.0);
    Filter *myFilter3 = new Filter(BPF, 51, 44.1, 1.0, 5.0);
    Filter *myFilter4 = new Filter(BPF, 51, 44.1, 5.0, 12.0);
    int numFreqBanks = 4;
    static const int blockSize = 1024;
    float unscaledAvg[blockSize];
    
    FFT forwardFFT;
    String midiMsg;
    float fifo [fftSize];
    //Banks to fold Freq Data
    float filtBank1 [fftSize];
    float filtBank2 [fftSize];
    float filtBank3 [fftSize];
    float filtBank4 [fftSize];
    
    float fftData [2 * fftSize];
    float filtSample = 0;
    int fifoIndex;
    bool nextFFTBlockReady;
    float audioOutput = 0.0;
    
    bool recording;
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
    Slider slider5;
    Label slider5Label;
    Slider slider6;
    Label slider6Label;
    Random random;
    Slider levelSlider;
    Label levelLabel;
    TextButton recordButton;
    TextButton connectButton;
    AltLookAndFeel altLookAndFeel;
    String buttonText;
    Colour buttonUp;
    Colour buttonDown;
    Label titleLabel;
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs;
    ReferenceCountedArray<MidiDeviceListEntry> midiOutputs;
    Label midiInputLabel;
    Label midiOutputLabel;
    //ScopedPointer<MidiDeviceListBox> midiInputSelector;
    //ScopedPointer<MidiDeviceListBox> midiOutputSelector;
    
    
};




// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
