/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (300, 600);
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

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff001423));

    
    //g.setFont (Font (16.0f));
    //g.setColour (Colours::white);
    //g.drawText ("MIDIBinz!!!!!!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    const int sliderLeft = 60;
    slider1.setBounds (sliderLeft, 200, getWidth() - sliderLeft - 10, 20);
    slider2.setBounds (sliderLeft, 240, getWidth() - sliderLeft - 10, 20);
    slider3.setBounds (sliderLeft, 280, getWidth() - sliderLeft - 10, 20);
    slider4.setBounds (sliderLeft, 320, getWidth() - sliderLeft - 10, 20);
}

void MainContentComponent::sliderValueChanged (Slider* slider) 
{
}