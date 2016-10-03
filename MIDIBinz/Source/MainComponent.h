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
class MainContentComponent   : public Component, public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider*) override;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
    
    Slider slider1;
    Label slider1Label;
    Slider slider2;
    Label slider2Label;
    Slider slider3;
    Label slider3Label;
    Slider slider4;
    Label slider4Label;
    
};


#endif  // MAINCOMPONENT_H_INCLUDED
