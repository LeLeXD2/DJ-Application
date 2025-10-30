/*====================================================================
WaveformDisplay.h
====================================================================*/


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//this class displays an audio waveform and handles interactions like setting the playhead position
class WaveformDisplay    : public Component, 
                           public ChangeListener
{
public:
    WaveformDisplay( AudioFormatManager & 	formatManagerToUse,
                    AudioThumbnailCache & 	cacheToUse );
    ~WaveformDisplay();

    void paint (Graphics&) override;

    void changeListenerCallback (ChangeBroadcaster *source) override;

    void loadURL(URL audioURL);

    void clear();

    // set the relative position of the playhead
    void setPositionRelative(double pos);

private:
    AudioThumbnail audioThumb;

    bool fileLoaded;

    double position;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
