/*====================================================================
DeckGUI.cpp
I have created this with the help of the tutorial from this course. I update and change majority of the codes as well as add on additional features which will be labelled.
The purpose of this cpp allows users to interact with various audio control features like volume, speed, treble, bass, and midrange levels, along with play/pause/stop functionality. It draws the user interface of the DJ application.
====================================================================*/


#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse
           ) : player(_player), 
               waveformDisplay(formatManagerToUse, cacheToUse)
{
    //styling play button
    //load custom image (like a PNG icon)
    Image playImage = ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
    //set the button images for the normal, hovered, and clicked states
    playButton.setImages(true, true, true,
        playImage, 1.0f, juce::Colours::transparentBlack,  //normal state
        playImage, 1.0f, juce::Colours::black.withAlpha(0.5f),  //hovered state
        playImage, 1.0f, juce::Colours::black.withAlpha(0.7f)); //pressed state

    //styling pause button
    //load custom image (like a PNG icon)
    Image pauseImage = ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    //set the button images for the normal, hovered and clicked states
    pauseButton.setImages(true, true, true,
        pauseImage, 1.0f, juce::Colours::transparentBlack,  //normal state
        pauseImage, 1.0f, juce::Colours::black.withAlpha(0.5f),  //hovered state
        pauseImage, 1.0f, juce::Colours::black.withAlpha(0.7f)); //pressed state

    //styling stop button
    //load custom image (like a PNG icon)
    Image stopImage = ImageFileFormat::loadFrom(BinaryData::stop_png, BinaryData::stop_pngSize);
    //set the button images for the normal, hovered and clicked states
    stopButton.setImages(true, true, true,
        stopImage, 1.0f, juce::Colours::transparentBlack,  //normal state
        stopImage, 1.0f, juce::Colours::black.withAlpha(0.5f),  //hovered state
        stopImage, 1.0f, juce::Colours::black.withAlpha(0.7f)); //pressed state

    //styling position slider
    posSlider.setRange(0.0, 1.0);
    //for the slider’s track color (the line the thumb moves along)
    posSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(39, 102, 123));
    //for the slider’s thumb color (the draggable part)
    posSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(161, 227, 249));
    //for the slider’s background color
    posSlider.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(68, 68, 68));
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    //styling volumne slider
    volSlider.setRange(0, 1, 0.1);
    volSlider.setSliderStyle(juce::Slider::Rotary);
    // Set the custom LookAndFeel
    volSlider.setLookAndFeel(&customLookAndFeel);

    //styling speed slider
    speedSlider.setRange(0.1, 5.0, 0.01);
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    // Set the custom LookAndFeel
    speedSlider.setLookAndFeel(&customLookAndFeel);

    //styling treble slider
    trebleSlider.setRange(0, 6.0, 0.1);
    trebleSlider.setSliderStyle(juce::Slider::Rotary);
    // Set the custom LookAndFeel
    trebleSlider.setLookAndFeel(&customLookAndFeel);

    //styling base slider
    bassSlider.setRange(0.0, 6.0, 0.1);
    bassSlider.setSliderStyle(juce::Slider::Rotary);
    // Set the custom LookAndFeel
    bassSlider.setLookAndFeel(&customLookAndFeel);

    //styling mids slider
    midSlider.setRange(0.0, 6.0, 0.1);
    midSlider.setSliderStyle(juce::Slider::Rotary);
    // Set the custom LookAndFeel
    midSlider.setLookAndFeel(&customLookAndFeel);
    
    //make the buttons visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);

    //make the slider visible
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(trebleSlider);
    addAndMakeVisible(bassSlider);
    addAndMakeVisible(midSlider);

    //make the waveform visible
    addAndMakeVisible(waveformDisplay);

    //adds listener for the buttons
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);

    //adds listener for slider
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    trebleSlider.addListener(this);
    bassSlider.addListener(this);
    midSlider.addListener(this);

    //setting timer to allow refreshes per 500ms
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    //stops timer
    stopTimer();
}

//this function paints the playlist component (background and color)
void DeckGUI::paint (Graphics& g)
{
    g.fillAll (Colour::fromRGB(29, 22, 22));
    g.setColour (Colour::fromRGB(29, 22, 22));
    g.drawRect (getLocalBounds(), 1); //draw an outline around the component

    //text title for volume
    //set the text color
    g.setColour(juce::Colours::white);

    //set the font and draw the text for volume
    g.setFont(juce::Font(volSlider.getWidth() / 15)); //set the font size
    g.drawText("Vol", volSlider.getX() - volSlider.getWidth() / 6.5, volSlider.getY() + volSlider.getHeight() / 1.15, volSlider.getWidth(), 20,
        juce::Justification::centred, true);

    //set the font and draw the text for speed
    g.setFont(juce::Font(speedSlider.getWidth() / 15)); //set the font size
    g.drawText("Speed", speedSlider.getX() - speedSlider.getWidth() / 5.3, speedSlider.getY() + speedSlider.getHeight() / 1.15, speedSlider.getWidth(), 20,
        juce::Justification::centred, true);

    //set the font and draw the text for treble
    g.setFont(juce::Font(trebleSlider.getWidth() / 15)); //set the font size
    g.drawText("Treble", trebleSlider.getX() - trebleSlider.getWidth() / 4, trebleSlider.getY() + trebleSlider.getHeight() / 1.125, trebleSlider.getWidth(), 20,
        juce::Justification::centred, true);

    //set the font and draw the text for bass
    g.setFont(juce::Font(bassSlider.getWidth() / 15)); //set the font size
    g.drawText("Bass", bassSlider.getX() - bassSlider.getWidth() / 4, bassSlider.getY() + bassSlider.getHeight() / 1.125, bassSlider.getWidth(), 20,
        juce::Justification::centred, true);

    //set the font and draw the text for bass
    g.setFont(juce::Font(midSlider.getWidth() / 15)); //set the font size
    g.drawText("Mid", midSlider.getX() - midSlider.getWidth() / 4, midSlider.getY() + midSlider.getHeight() / 1.125, midSlider.getWidth(), 20,
        juce::Justification::centred, true);

    //if the audio is not loaded, the fucntion will end here and it will not run the codes below.
    if (!isAudioLoaded) {
        return;
    }
    
    //set the font and color for the text display
    g.setFont(getWidth() / 30);  //you can adjust the font size
    g.setColour(Colours::white);  //setting the color of the next text

    //get the total length of the audio in seconds
    float totalLength = player->getLength(); //assuming this is the total length in seconds

    //converts total length from seconds to minutes and seconds
    int minutes_length = static_cast<int>(totalLength) / 60;
    int seconds_length = static_cast<int>(totalLength) % 60;

    //format minutes and seconds into a string
    String timeString = String::formatted("%02d:%02d", minutes_length, seconds_length);

    //get the relative position of the audio in seconds
    float relative_positionbefore = player->getPosition();
    float relative_position = relative_positionbefore * totalLength;

    //convert total length of the audio in seconds
    int minutes_relativepos = static_cast<int>(relative_position) / 60;
    int seconds_relativepos = static_cast<int>(relative_position) % 60;

    //format minutes and seconds into string
    String relativeposString = String::formatted("%02d:%02d", minutes_relativepos, seconds_relativepos);

    //creating rectangle for the text
    Rectangle<int> textArea(getWidth() / 30, getHeight() / 8 * 7.25, getWidth(), getHeight() / 11);

    //draw the text displaying the total length in minute and secons format
    g.drawText(relativeposString + " / " + timeString, textArea,
        Justification::left, true); //position it with padding and centered
}

//this function lays out the child component and resize it
void DeckGUI::resized()
{
    double rowH = getHeight() / 8; 
    double sliderWidth = getWidth() / 9;
    double buttonWidth = getWidth() / 11;

    //creating the textbox with the volume slider value
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 14, rowH / 3);

    //creating the textbox with the volume slider value
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 14, rowH / 3);
    speedSlider.setTextValueSuffix("x");

    //creating the textbox with the volume slider value
    trebleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 10, rowH / 3);
    trebleSlider.setTextValueSuffix("dB");

    //creating the textbox with the volume slider value
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 10, rowH / 3);
    bassSlider.setTextValueSuffix("dB");

    //creating the textbox with the volume slider value
    midSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, getWidth() / 10, rowH / 3);
    midSlider.setTextValueSuffix("dB");

    //set the coordination for the sliders
    volSlider.setBounds(sliderWidth * 1.45, rowH / 10, getWidth() / 3, rowH * 2.4);
    speedSlider.setBounds(sliderWidth * 4.4, rowH / 10, getWidth() / 3, rowH * 2.4);
    trebleSlider.setBounds(sliderWidth / 34, rowH * 2.8, getWidth() / 3, rowH * 2.4);
    bassSlider.setBounds(sliderWidth * 3, rowH * 2.8, getWidth() / 3, rowH * 2.4);
    midSlider.setBounds(sliderWidth * 6, rowH * 2.8, getWidth() / 3, rowH * 2.4);
    posSlider.setBounds(0, rowH * 3, getWidth(), rowH);
    posSlider.setBounds(0, rowH * 6.7, getWidth(), rowH);

    //set the coordination for the waveform
    waveformDisplay.setBounds(0, rowH * 5.4, getWidth(), rowH * 1.5);

    //set the coordination for the buttons
    playButton.setBounds(buttonWidth * 4.25, rowH * 7.5, buttonWidth, rowH * 0.3);
    pauseButton.setBounds(buttonWidth * 5, rowH * 7.5, buttonWidth, rowH * 0.3);
    stopButton.setBounds(buttonWidth * 5.75, rowH * 7.5, buttonWidth, rowH * 0.3);
}

//this function handles the eventlistener for button when it is clicked
void DeckGUI::buttonClicked(Button* button)
{
    //runs when the playButton is clicked
    if (button == &playButton) {
        player->start();
    }

    //runs when the pauseButton is clicked
    if (button == &pauseButton) {
        player->stop();
    }

    //runs when the stopButton is clicked
    if (button == &stopButton) {
        player->stop(); //stop playback

        //reset the audio source
        player->loadURL(URL{}); //clear the loaded audio file
        waveformDisplay.clear(); //clear the waveform display

        //reset all the slider positions
        speedSlider.setValue(1.0);
        volSlider.setValue(0);
        posSlider.setValue(0.0);
        trebleSlider.setValue(0.0);
        bassSlider.setValue(0.0);
        midSlider.setValue(0.0);
        
        //change the flag to false
        isAudioLoaded = false;

        //trigger the paint function again
        repaint();
    }
}

//this function handles the eventlistener for sliders when it is clicked
void DeckGUI::sliderValueChanged (Slider *slider)
{
    //runs when the volSlider is moved
    if (slider == &volSlider) {
        player->setVolume(slider->getValue());
    }

    //runs when speedslider is moved
    if (slider == &speedSlider) {
        player->setSpeed(slider->getValue());
    }
    
    //runs when posslider is moved
    if (slider == &posSlider) {
        player->setPositionRelative(slider->getValue());
    }

    //runs when trebleslider is moved
    if (slider == &trebleSlider) {
        player->setTreble(slider->getValue());
    }
    
    //runs when bassslider is moved
    if (slider == &bassSlider) {
        player->setBass(slider->getValue());
    }

    //runs when midslider is moved
    if (slider == &midSlider) {
        player->setMid(slider->getValue());
    }
}

//this function is to update the UI component especially for posSlider and WaveformDisplay
void DeckGUI::timerCallback()
{
    //std::cout << "DeckGUI::timerCallback" << std::endl;
    waveformDisplay.setPositionRelative(
            player->getPosition());

    // Get the total length and current position of the audio
    float totalLength = player->getLength();
    float relativePosition = player->getPosition();

    // Ensure the current position is within the total length to avoid out-of-bounds errors
    if (totalLength > 0) {
        // Update the slider's value
        posSlider.setValue(relativePosition);
    }

    //trigger repaint to update the display
    repaint();
}
    
//this function load the track from the playlist 
void DeckGUI::loadTrackFromPlaylist(juce::URL trackURL)
{
    if (trackURL.isEmpty()) {
        return;
    }

    player->loadURL(trackURL);
    waveformDisplay.loadURL(trackURL);
    isAudioLoaded = true;

    volSlider.setValue(100);
    speedSlider.setValue(1.0);
}

//this checks if the audio is loaded.
bool DeckGUI::CheckAudioLoaded() 
{
    return isAudioLoaded; 
}
