/*
  ==============================================================================

    MainComponent.h
    Created: 14 Nov 2016 8:57:10pm
    Author:  Lindley P Cannon Jr

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Filter.h"

struct MidiDeviceListEntry;
struct MidiCallbackMessage;

class AltLookAndFeel : public LookAndFeel_V3
{
public:
    AltLookAndFeel();
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override;
    
};

class MainContentComponent : public AudioAppComponent,
                            public Slider::Listener,
                            public ButtonListener,
                            private Timer,
                            private MidiInputCallback,
                            private MessageListener
{
public:
    MainContentComponent();
    ~MainContentComponent();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    void ConvertToMidi(float* rawData);
    void timerCallback(); 
    
    void pushNextSampleIntoFilter (float sample)noexcept;
    
    float getAverageBinAmplitude (float * bank);
    void printFFT();
    void releaseResources(); 
    void paint (Graphics& g); 
    void resized(); 
    void sliderValueChanged (Slider* slider); 
    void sendToOutputs(const MidiMessage& msg);
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage &message); 
    void handleMessage (const Message& msg); 
    int getNumMidiInputs()const noexcept;
    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput)const noexcept;
    int getNumMidiOutputs()const noexcept;
    void openDevice (bool isInput, int index);
    void closeDevice (bool isInput, int index);
    void closeUnpluggedDevices (StringArray& currentlyPluggedInDevices, bool isInputDevice);
    bool hasDeviceListChanged (const StringArray& deviceNames, bool isInputDevice);
    ReferenceCountedObjectPtr<MidiDeviceListEntry> findDeviceWithName (const String& name, bool isInputDevice)const;
    void updateDeviceList (bool isInputDeviceList);
    void buttonClicked(Button* buttonThatWasClicked); 
    
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
    int iter;
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
    Label titleLabel1;
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs;
    ReferenceCountedArray<MidiDeviceListEntry> midiOutputs;
    Label midiInputLabel;
    Label midiOutputLabel;
    TextButton doneButton;
    Label prototypeMIDIMessage;
    ListBox MidiOutputListBox;
    Label listBoxTitle;
    Label projectGroupTitle1;
    Label projectGroupTitle2;
    //ScopedPointer<MidiDeviceListBox> midiInputSelector;
    //ScopedPointer<MidiDeviceListBox> midiOutputSelector;
};


#endif  // MAINCOMPONENT_H_INCLUDED
