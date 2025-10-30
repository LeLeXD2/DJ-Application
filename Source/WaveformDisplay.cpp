/*====================================================================
WaveformDisplay.cpp
I have created this with the help of the tutorial from this course. I update and change majority of the codes and changed the looks of the wave form by giving it a gradient.
This class is responsible for rendering the visual waveform of an audio file. And generating it onto the application by using paint().
====================================================================*/


#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0)

{
    //adds change listenr to audioThumn
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

//this function paints waveform display for an audio file
void WaveformDisplay::paint(Graphics& g)
{
    g.fillAll(Colour::fromRGB(29, 22, 22)); //set background color
    g.setColour(Colour::fromRGB(29, 22, 22)); //set outline color
    g.drawRect(getLocalBounds(), 1); //draw an outline around the component
    g.setColour(Colours::lightcoral); //setting the color of the waveform to light coral

    //checks if the file is loaded. If it is, the waveform will be drawn
    if (fileLoaded)
    {
        //add padding to the top and bottom
        int padding = 10;
        int usableHeight = getHeight() - 2 * padding; //height for the waveform with padding

        //create a gradient from top to bottom
        ColourGradient gradient(Colours::lightblue, 0.0f, 0.0f, //start color at the top
            Colours::darkblue, 0.0f, (float)getHeight(), //end color at the bottom
            false); //vertical gradient

        g.setGradientFill(gradient); //set the gradient fill

        //draw the waveform with the gradient above
        audioThumb.drawChannel(g,
            getLocalBounds().withTop(padding).withHeight(usableHeight), //apply height padding
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );

        //draw the playhead
        g.setColour(Colour::fromRGB(221, 230, 237));
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight()); //set the position of the rectangle with it moving.
    }
}

//this function loads the url and repaints the waveform if file is loaded
void WaveformDisplay::loadURL(URL audioURL)
{
    //clears audioThumb values
    audioThumb.clear();
    //variable to check if it is loaded
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        repaint(); //paints the waveform
    }
    else {
        std::cout << "wfd: not loaded! " << std::endl;
    }

}

//this function repaints the waveform when a change is broadcasted.
void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}

//this function finds the postion of where the audio is and repaint if needed
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }


}

//this function clears the audio thumbnail and removes the waveform
void WaveformDisplay::clear()
{
    audioThumb.clear();  // This clears the audio thumbnail
    fileLoaded = false;
    repaint();  // Redraw the component
}
