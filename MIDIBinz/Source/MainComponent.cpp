/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/


#include "MainComponent.h"

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


AltLookAndFeel::AltLookAndFeel()
    {
        setColour (Slider::rotarySliderFillColourId, Colours::red);
        //setColour(TextButton::buttonColourId, Colours::blue);
    }
    
    
    
    void AltLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
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

void AltLookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown)
    {
        if(button.getName() == "Record")
        {
            button.setColour(TextButton::buttonColourId , Colour(0xfffd0000));
            button.setColour(TextButton::textColourOffId, Colours::white);
        }
        if(button.getName() == "Connect" || button.getName() == "Done")
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





//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

    //==============================================================================
MainContentComponent::MainContentComponent() : forwardFFT (fftOrder, false),
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
        setAudioChannels (2, 0);
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
        
        // menu Close Button
        doneButton.setName("Done");
        doneButton.setButtonText("Done");
        addAndMakeVisible(doneButton);
        doneButton.addListener(this);
        doneButton.setVisible(false);
        
        // Title Label
        titleLabel1.setText("MIDIBinz", dontSendNotification);
        Font textFont;
        textFont.setTypefaceName("BrushScript");
        textFont.setBold(true);
        textFont.setUnderline(true);
        textFont.setHeight(17);
        titleLabel1.setFont(textFont);
        titleLabel1.setColour(titleLabel1.textColourId, Colour(0xff2ca1ff));
        addAndMakeVisible(titleLabel1);
        
        // Project Group Title
        projectGroupTitle1.setText("A JLT-G", dontSendNotification);
        projectGroupTitle2.setText("Project", dontSendNotification);
        Font textFont2;
        textFont2.setTypefaceName("Connecticut");
        textFont2.setBold(true);
        textFont2.setUnderline(true);
        textFont2.setHeight(15);
        projectGroupTitle1.setFont(textFont2);
        projectGroupTitle2.setFont(textFont2);
        projectGroupTitle1.setColour(projectGroupTitle1.textColourId, Colour(0xff2ca1ff));
        projectGroupTitle2.setColour(projectGroupTitle2.textColourId, Colour(0xff2ca1ff));
        addAndMakeVisible(projectGroupTitle1);
        addAndMakeVisible(projectGroupTitle2);
        
        // MIDI Message
        prototypeMIDIMessage.setText("Test", dontSendNotification);
        addAndMakeVisible(prototypeMIDIMessage);
        
        // MIDI Listbox
        //SparseSet<int> selections;
        //selections.ad
        addAndMakeVisible(MidiOutputListBox);
        listBoxTitle.setText("MIDI Outputs", dontSendNotification );
        listBoxTitle.setColour(Label::textColourId, Colours::white);
        addAndMakeVisible(listBoxTitle);
        MidiOutputListBox.setVisible(false);
        listBoxTitle.setVisible(false);
    }

MainContentComponent::~MainContentComponent()
    {
        shutdownAudio();
        midiInputs.clear();
        midiOutputs.clear();
    }

    //==============================================================================
    void MainContentComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        // For more details, see the help for AudioProcessor::prepareToPlay()
    }

    void MainContentComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
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
    void MainContentComponent::ConvertToMidi(float* rawData)
    {
        int midiVal = (int)100*(*rawData);
        jmap(abs(midiVal*100),0,50,1,127);
        MidiMessage msg = MidiMessage::controllerEvent(1, 20, midiVal);
        midiMsg = msg.getDescription();
        repaint();
        
    }
    void MainContentComponent::timerCallback()
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
            //for(int i = 0; i < 4; i++) {
            //    printf("%f\n",unscaledAvg[i]);
            //}
            ConvertToMidi(unscaledAvg);
            std::cout << midiMsg << std::endl;
        }
    }
    
    
    void MainContentComponent::pushNextSampleIntoFilter (float sample)noexcept
    {
        //Process Filter Function & Fill Bank
        if(fifoIndex == blockSize) // strictly comparitive statement
        {
            nextFFTBlockReady = true;
            fifoIndex = 0;
            iter = 0;
        }
        
        else
        {
            
            filtBank1[iter] = myFilter1->do_sample(sample);
            filtBank2[iter] = myFilter2->do_sample(sample);
            filtBank3[iter] = myFilter3->do_sample(sample);
            filtBank4[iter] = myFilter4->do_sample(sample);
            iter++;
            fifoIndex++; // count to get size to process (1024 for now)
        }
    }
    
    /* FUNCTION TO FIND AVERAGE OF EACH FILTER BIN */
    // Return MAX to test
    float MainContentComponent::getAverageBinAmplitude (float * bank)
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
    void MainContentComponent::printFFT()
    {
        forwardFFT.performRealOnlyForwardTransform(fftData);
        
        ConvertToMidi(fftData);
        std::cout << recordButton.getName() << std::endl;
        std::cout << midiMsg << std::endl;
    }
    
    void MainContentComponent::releaseResources()
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }
    //==============================================================================
    void MainContentComponent::paint (Graphics& g)
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colour (0xff48494a));


        // You can add your drawing code here!
        if (recordButton.getState() == Button::buttonDown)
        {
            buttonText = "Recording.....";
            prototypeMIDIMessage.setText(midiMsg, dontSendNotification);
        }
        else
        {
            buttonText = "Hold To Record";
            prototypeMIDIMessage.setText("", dontSendNotification);
        }
        recordButton.setButtonText(buttonText);
        //g.drawText(midiMsg,10, 40, 200, 860, true);
        
    }

    void MainContentComponent::resized()
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
        levelSlider.setBounds(20, 442, 280, buttonSize-75);
        recordButton.setBounds(5, 485, 310, 95);
        
        slider1Label.setBounds(sliderLeft+10, buttonRow-25, 50, 15);
        slider2Label.setBounds(sliderLeft+120, buttonRow-75, 50, 15);
        slider3Label.setBounds(sliderLeft+230, buttonRow-25, 50, 15);
        slider4Label.setBounds(sliderLeft+10, buttonRow+125, 50, 15);
        slider5Label.setBounds(sliderLeft+120, buttonRow+165, 50, 15);
        slider6Label.setBounds(sliderLeft+230, buttonRow+125, 50, 15);
        
        titleLabel1.setBounds(5,0,300, 70);
        projectGroupTitle1.setBounds(250, 0, 300, 70);
        projectGroupTitle2.setBounds(250, 20, 300, 70);
        connectButton.setBounds(120, 190, 80, 80);
        doneButton.setBounds(250, 540, 60, 40);
        prototypeMIDIMessage.setBounds(5, 465, 200, 15);
        MidiOutputListBox.setBounds(35, 100, 250, 400);
        listBoxTitle.setBounds(30, 60, 90, 40);
        
        
    }
    void MainContentComponent::sliderValueChanged (Slider* slider)
    {
    }
    
    
    void MainContentComponent::sendToOutputs(const MidiMessage& msg)
    {
        for (int i = 0; i < midiOutputs.size(); ++i)
            if (midiOutputs[i]->outDevice != nullptr)
                midiOutputs[i]->outDevice->sendMessageNow (msg);
        
        const String mesg = (String) *msg.getRawData();
        //myMsg = msg.getDescription();
        
        repaint();
        
    }
    
    //==============================================================================
    void MainContentComponent::handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage &message)
    {
        // This is called on the MIDI thread
        
        if (message.isNoteOnOrOff())
            postMessage (new MidiCallbackMessage (message));
    }
    
    //==============================================================================
    void MainContentComponent::handleMessage (const Message& msg)
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
    
    
    int MainContentComponent::getNumMidiInputs() const noexcept
    {
        return midiInputs.size();
    }
    ReferenceCountedObjectPtr<MidiDeviceListEntry> MainContentComponent::getMidiDevice (int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }
    
    //==============================================================================
    int MainContentComponent::getNumMidiOutputs()const noexcept
    {
        return midiOutputs.size();
    }
    void MainContentComponent::openDevice (bool isInput, int index)
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
    
    
    void MainContentComponent::closeDevice (bool isInput, int index)
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
    void MainContentComponent::closeUnpluggedDevices (StringArray& currentlyPluggedInDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
        : midiOutputs;
        
        for (int i = midiDevices.size(); --i >= 0;)
        {
            MidiDeviceListEntry& d = *midiDevices[i];
            
            if (! currentlyPluggedInDevices.contains (d.name))
            {
                if (isInputDevice ? d.inDevice != nullptr
                    : d.outDevice != nullptr)
                    closeDevice (isInputDevice, i);
                
                midiDevices.remove (i);
            }
        }
    }
    bool MainContentComponent::hasDeviceListChanged (const StringArray& deviceNames, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
        : midiOutputs;
        
        if (deviceNames.size() != midiDevices.size())
            return true;
        
        for (int i = 0; i < deviceNames.size(); ++i)
            if (deviceNames[i] != midiDevices[i]->name)
                return true;
        
        return false;
    }
ReferenceCountedObjectPtr<MidiDeviceListEntry>  MainContentComponent::findDeviceWithName (const String& name, bool isInputDevice) const
    {
        const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
        : midiOutputs;
        
        for (int i = 0; i < midiDevices.size(); ++i)
            if (midiDevices[i]->name == name)
                return midiDevices[i];
        
        return nullptr;
    }
void MainContentComponent::updateDeviceList (bool isInputDeviceList)
    {
        StringArray newDeviceNames = isInputDeviceList ? MidiInput::getDevices()
        : MidiOutput::getDevices();
        
        if (hasDeviceListChanged (newDeviceNames, isInputDeviceList))
        {
            
            ReferenceCountedArray<MidiDeviceListEntry>& midiDevices
            = isInputDeviceList ? midiInputs : midiOutputs;
            
            closeUnpluggedDevices (newDeviceNames, isInputDeviceList);
            
            ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;
            
            // add all currently plugged-in devices to the device list
            for (int i = 0; i < newDeviceNames.size(); ++i)
            {
                MidiDeviceListEntry::Ptr entry = findDeviceWithName (newDeviceNames[i], isInputDeviceList);
                
                if (entry == nullptr)
                    entry = new MidiDeviceListEntry (newDeviceNames[i]);
                
                newDeviceList.add (entry);
            }
            
            // actually update the device list
            midiDevices = newDeviceList;
            
            //TODO
            // update the selection status of the combo-box
            //if (MidiDeviceListBox* midiSelector = isInputDeviceList ? midiInputSelector : midiOutputSelector)
            //    midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
        }
    }
    void MainContentComponent::buttonClicked(Button* buttonThatWasClicked)
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
            doneButton.setVisible(true);
            listBoxTitle.setVisible(true);
            MidiOutputListBox.setVisible(true);
        }
        if(buttonThatWasClicked->getName() == "Done")
        {
            slider1.setVisible(true);
            slider1Label.setVisible(true);
            slider2.setVisible(true);
            slider2Label.setVisible(true);
            slider3.setVisible(true);
            slider3Label.setVisible(true);
            slider4.setVisible(true);
            slider4Label.setVisible(true);
            slider5.setVisible(true);
            slider5Label.setVisible(true);
            slider6.setVisible(true);
            slider6Label.setVisible(true);
            recordButton.setVisible(true);
            levelSlider.setVisible(true);
            connectButton.setVisible(true);
            doneButton.setVisible(false);
            listBoxTitle.setVisible(false);
            MidiOutputListBox.setVisible(false);
        }
        
    }




// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


