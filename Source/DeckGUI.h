/*===================================================================
DeckGUI.h
====================================================================*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

class CustomRotarySlider : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        auto r = juce::jmin(width / 2, height / 2) - 4.0f;
        auto cX = x + width * 0.5f;
        auto cY = y + height * 0.5f;
        auto rx = cX - r;
        auto ry = cY - r;
        auto rw = r * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        //this creates a glowing effect on the outer circke
        juce::ColourGradient glow(juce::Colours::cyan.withAlpha(0.3f), cX, cY,
            juce::Colours::transparentBlack, cX, cY + r, true);
        g.setGradientFill(glow);
        g.fillEllipse(rx - 5, ry - 5, rw + 10, rw + 10);

        //this creates a background color with gradient
        juce::ColourGradient baseGradient(juce::Colours::darkgrey, rx, ry,
            juce::Colours::black, rx + rw, ry + rw, false);
        g.setGradientFill(baseGradient);
        g.fillEllipse(rx, ry, rw, rw);

        //this creates an inner glowing knob
        juce::ColourGradient knobGradient(juce::Colours::darkcyan, cX, cY,
            juce::Colours::black, cX, cY + r * 0.7f, true);
        g.setGradientFill(knobGradient);
        g.fillEllipse(rx + 6, ry + 6, rw - 12, rw - 12);

        //this creates an indicator
        juce::Path pointer;
        float pointerLength = r * 0.7f;
        float pointerThickness = 4.0f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -r + 10, pointerThickness, pointerLength, 2.0f);

        g.setColour(juce::Colours::white);
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(cX, cY));

        //this add reflector effects
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.fillEllipse(rx + 8, ry + 8, rw - 16, rw - 16);
    }
};

//this function manage the graphical user interface and provide controls for playback, volume, speed, position, and audio effects (treble, bass, mid)
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener,
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
           AudioFormatManager & 	formatManagerToUse,
           AudioThumbnailCache & 	cacheToUse
           );
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

    //implementing listener for button and slider
    void buttonClicked (Button *) override;
    void sliderValueChanged (Slider *slider) override;
    void loadTrackFromPlaylist(juce::URL trackURL);
    bool CheckAudioLoaded();
    void timerCallback() override; 

private:

    //this checks if the audio is loaded
    bool isAudioLoaded = false;

    //creating button variables
    ImageButton playButton{"PLAY"};
    ImageButton pauseButton{"PAUSE"};
    ImageButton stopButton{"STOP"};

    //creating image variables
    juce::Image playImage;
    juce::Image pauseImage;
    juce::Image stopImage;

    //creating sliders variables
    Slider volSlider; 
    Slider speedSlider;
    Slider posSlider;
    Slider trebleSlider;
    Slider bassSlider;
    Slider midSlider;
    
    WaveformDisplay waveformDisplay;

    CustomRotarySlider customLookAndFeel;

    DJAudioPlayer* player; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};  