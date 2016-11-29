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
        
    }
    void AltLookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown)
    {
        if(button.getName() == "Record")
        {
            button.setColour(TextButton::buttonColourId , Colour(0xfffd0000));
            button.setColour(TextButton::textColourOffId, Colours::white);
        }
        if(button.getName() == "Connect" || button.getName() == "Done" ||
           button.getName() == "Bin1" || button.getName() == "Bin2" ||
           button.getName() == "Bin3" || button.getName() == "Bin4")
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




class MidiDeviceListBox : public ListBox,
private ListBoxModel
{
public:
    //==============================================================================
    MidiDeviceListBox (const String& name,
                       MainContentComponent& contentComponent,
                       bool isInputDeviceList)
    : ListBox (name, this),
      parent (contentComponent),
      isInput (isInputDeviceList)
    {
        setOutlineThickness (1);
        setMultipleSelectionEnabled (true);
        setClickingTogglesRowSelection (true);
    }

    //==============================================================================
    int getNumRows() override
    {
        return isInput ? parent.getNumMidiInputs()
                       : parent.getNumMidiOutputs();
    }

    //==============================================================================
    void paintListBoxItem (int rowNumber, Graphics &g,
                           int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (Colour (0xffeeeeee));


        g.setColour (Colours::black);
        g.setFont (height * 0.7f);

        if (isInput)
        {
            if (rowNumber < parent.getNumMidiInputs())
                g.drawText (parent.getMidiDevice (rowNumber, true)->name,
                            5, 0, width, height,
                            Justification::centredLeft, true);
        }
        else
        {
            if (rowNumber < parent.getNumMidiOutputs())
                g.drawText (parent.getMidiDevice (rowNumber, false)->name,
                            5, 0, width, height,
                            Justification::centredLeft, true);
        }
    }

    //==============================================================================
    void selectedRowsChanged (int) override
    {
        SparseSet<int> newSelectedItems = getSelectedRows();
        if (newSelectedItems != lastSelectedItems)
        {
            for (int i = 0; i < lastSelectedItems.size(); ++i)
            {
                if (! newSelectedItems.contains (lastSelectedItems[i]))
                    parent.closeDevice (isInput, lastSelectedItems[i]);
            }

            for (int i = 0; i < newSelectedItems.size(); ++i)
            {
                if (! lastSelectedItems.contains (newSelectedItems[i]))
                    parent.openDevice (isInput, newSelectedItems[i]);
            }

            lastSelectedItems = newSelectedItems;
        }
    }

    //==============================================================================
    void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
    {
        SparseSet<int> selectedRows;
        for (int i = 0; i < midiDevices.size(); ++i)
            if (midiDevices[i]->inDevice != nullptr || midiDevices[i]->outDevice != nullptr)
                selectedRows.addRange (Range<int> (i, i+1));

        lastSelectedItems = selectedRows;
        updateContent();
        setSelectedRows (selectedRows, dontSendNotification);
    }

private:
    //==============================================================================
    MainContentComponent& parent;
    bool isInput;
    SparseSet<int> lastSelectedItems;
    

};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

    //==============================================================================
MainContentComponent::MainContentComponent() : forwardFFT (fftOrder, false),
                             midiOutputLabel ("Midi Output Label", "MIDI Output:"),
                             midiOutputSelector (new MidiDeviceListBox ("Midi Output Selector", *this, false))
    {
        
        setSize (width, height);
        buttonText = "Hold To Send MIDI";
        setLookAndFeel (&altLookAndFeel);
        
        // Mic Level Slider
        levelSlider.setRange (0.0, 0.25);
        levelSlider.setValue(0.10);
        levelSlider.setTextBoxStyle (Slider::NoTextBox, false, 100, 20);
        levelLabel.setText ("Mic Level", dontSendNotification);
        levelLabel.setColour(Label::textColourId, Colours::white);
        levelLabel.attachToComponent(&levelSlider, false);
        addAndMakeVisible (levelSlider);
        addAndMakeVisible (levelLabel);
        
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 0);
        startTimerHz (60);
        
        // Bin 1 Button
        bin1Button.setName("Bin1");
        bin1Button.setButtonText("Bin 1");
        bin1=false;
        bin1Button.addListener(this);
        addAndMakeVisible(bin1Button);
        
        // Bin 2 Button
        bin2Button.setName("Bin2");
        bin2Button.setButtonText("Bin 2");
        bin2=false;
        bin2Button.addListener(this);
        addAndMakeVisible(bin2Button);
        
        // Bin 3 Button
        bin3Button.setName("Bin3");
        bin3Button.setButtonText("Bin 3");
        bin3=false;
        bin3Button.addListener(this);
        addAndMakeVisible(bin3Button);
        
        // Bin 4 Button
        bin4Button.setName("Bin4");
        bin4Button.setButtonText("Bin 4");
        bin4=false;
        bin4Button.addListener(this);
        addAndMakeVisible(bin4Button);
        
        // Stop MIDI Button
        zeroMidi.setName("zeroMidi");
        zeroMidi.setButtonText("Stop MIDI");
        zeroMidi.addListener(this);
        addAndMakeVisible(zeroMidi);
        
        
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
        projectGroupTitle1.setText("A JLT-G Project", dontSendNotification);
        Font textFont2;
        textFont2.setTypefaceName("BrushScript");
        textFont2.setBold(true);
        textFont2.setUnderline(true);
        textFont2.setHeight(15);
        projectGroupTitle1.setFont(textFont2);
        projectGroupTitle1.setColour(projectGroupTitle1.textColourId, Colour(0xff2ca1ff));
        addAndMakeVisible(projectGroupTitle1);
        
        // MIDI Message
        prototypeMIDIMessage.setText("Test", dontSendNotification);
        addAndMakeVisible(prototypeMIDIMessage);
        
        // MIDI Listbox
        listBoxTitle.setText("MIDI Outputs", dontSendNotification );
        listBoxTitle.setColour(Label::textColourId, Colours::white);
        addAndMakeVisible(listBoxTitle);
        
        listBoxTitle.setVisible(false);
        addAndMakeVisible (midiOutputSelector);
        midiOutputSelector->setVisible(false);
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
        int midiVal1 = (int)100*(rawData[0]);
        int midiVal2= (int)100*(rawData[1]);
        int midiVal3 = (int)100*(rawData[2]);
        int midiVal4 = (int)100*(rawData[3]);
        
        jmap(abs(midiVal1*100),0,50,1,127);
        jmap(abs(midiVal2*100),0,50,1,127);
        jmap(abs(midiVal3*100),0,50,1,127);
        jmap(abs(midiVal4*100),0,50,1,127);
        
        MidiMessage messages[4] = {MidiMessage::controllerEvent(1, 20, midiVal1),
                                    MidiMessage::controllerEvent(1, 20, midiVal2),
                                    MidiMessage::controllerEvent(1, 20, midiVal3),
                                    MidiMessage::controllerEvent(1, 20, midiVal4)};
        if (bin1 && midiVal1 !=0)
            sendToOutputs(messages[0]);
        else if (bin2 && midiVal2 !=0)
            sendToOutputs(messages[1]);
        else if (bin3&& midiVal3 !=0)
            sendToOutputs(messages[2]);
        else if (bin4&& midiVal4 !=0)
            sendToOutputs(messages[3]);
        repaint();
        
        
    }
    void MainContentComponent::timerCallback()
    {
   
        updateDeviceList (true);
        updateDeviceList (false);
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
            ConvertToMidi(unscaledAvg);
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
            buttonText = "Sending MIDI.....";
            prototypeMIDIMessage.setText(midiMsg, dontSendNotification);
        }
        else
        {
            buttonText = "Hold To Send MIDI";
            prototypeMIDIMessage.setText("", dontSendNotification);
        }
        if(bin1==true)
        {
            bin1Button.setColour(TextButton::buttonColourId, Colours::green);
            bin2Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin3Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin4Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        else
        {
            bin1Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        if(bin2==true)
        {
            bin1Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin2Button.setColour(TextButton::buttonColourId, Colours::green);
            bin3Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin4Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        else
        {
            bin2Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        if(bin3==true)
        {
            bin1Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin2Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin3Button.setColour(TextButton::buttonColourId, Colours::green);
            bin4Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        else
        {
            bin3Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        if(bin4==true)
        {
            bin1Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin2Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin3Button.setColour(TextButton::buttonColourId, Colours::grey);
            bin4Button.setColour(TextButton::buttonColourId, Colours::green);
        }
        else
        {
            bin4Button.setColour(TextButton::buttonColourId, Colours::grey);
        }
        recordButton.setButtonText(buttonText);
        
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
        bin1Button.setBounds(sliderLeft+25, buttonRow, buttonSize+20, buttonSize+20);
        bin2Button.setBounds(sliderLeft+155,buttonRow, buttonSize+20, buttonSize+20);
        bin3Button.setBounds(sliderLeft+25, buttonRow+130, buttonSize+20, buttonSize+20);
        bin4Button.setBounds(sliderLeft+155,buttonRow+130, buttonSize+20, buttonSize+20);
        zeroMidi.setBounds(105,390, 100, 40);
        levelSlider.setBounds(20, 455, 280, buttonSize-75);
        recordButton.setBounds(5, 485, 310, 95);
        titleLabel1.setBounds(120,0,300, 70);
        projectGroupTitle1.setBounds(103, 20, 300, 70);
        connectButton.setBounds(5, 80, 310, 30);
        doneButton.setBounds(250, 540, 60, 40);
        prototypeMIDIMessage.setBounds(5, 465, 200, 15);
        MidiOutputListBox.setBounds(35, 100, 250, 400);
        midiOutputSelector->setBounds(35, 100, 250, 400);
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
        StringArray newDeviceNames = MidiOutput::getDevices();
        
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
            
           
            // update the selection status of the combo-box
            MidiDeviceListBox* midiSelector =  midiOutputSelector;
            midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
        }
    }
    void MainContentComponent::buttonClicked(Button* buttonThatWasClicked)
    {
        //std::cout << buttonThatWasClicked->getName() << std::endl;
        if(buttonThatWasClicked->getName() == "Connect")
        {
            bin1Button.setVisible(false);
            bin2Button.setVisible(false);
            bin3Button.setVisible(false);
            bin4Button.setVisible(false);
            recordButton.setVisible(false);
            levelSlider.setVisible(false);
            connectButton.setVisible(false);
            doneButton.setVisible(true);
            listBoxTitle.setVisible(true);
            midiOutputSelector->setVisible(true);
        }
        if(buttonThatWasClicked->getName() == "Done")
        {
            bin1Button.setVisible(true);
            bin2Button.setVisible(true);
            bin3Button.setVisible(true);
            bin4Button.setVisible(true);
            recordButton.setVisible(true);
            levelSlider.setVisible(true);
            connectButton.setVisible(true);
            doneButton.setVisible(false);
            listBoxTitle.setVisible(false);
            midiOutputSelector->setVisible(false);
        }
        if(buttonThatWasClicked == &bin1Button)
        {
            bin1=!bin1;
            bin2=false;
            bin3=false;
            bin4=false;
            repaint();
            std::cout << bin1 << bin2 << bin3 << bin4 << std::endl;
        }
        if(buttonThatWasClicked == &bin2Button)
        {
            bin1=false;
            bin2=!bin2;
            bin3=false;
            bin4=false;
            repaint();
            std::cout << bin1 << bin2 << bin3 << bin4 << std::endl;
        }
        if(buttonThatWasClicked == &bin3Button)
        {
            bin1=false;
            bin2=false;
            bin3=!bin3;
            bin4=false;
            repaint();
            std::cout << bin1 << bin2 << bin3 << bin4 << std::endl;
        }
        if(buttonThatWasClicked == &bin4Button)
        {
            bin1=false;
            bin2=false;
            bin3=false;
            bin4=!bin4;
            repaint();
            std::cout << bin1 << bin2 << bin3 << bin4 << std::endl;
        }
        if(buttonThatWasClicked == &zeroMidi)
        {
            sendToOutputs(MidiMessage::controllerEvent(1, 20, 0));
        }
    }

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


