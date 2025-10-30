/*====================================================================
MainComponent.cpp
This is a basic framework for an audio application that handles both the UI and audio processing components.
These codes are from the tutorial from the course that I have updated with my own codes
====================================================================*/


#include "MainComponent.h"

MainComponent::MainComponent()
{
    //resize the window
    setSize (1200, 800);

    //some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio)) {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else {
        //specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }  

    addAndMakeVisible(deckGUI1); 
    addAndMakeVisible(deckGUI2);  
    
    addAndMakeVisible(playlistComponent);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    //this shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//this function ensures that the necessary audio components are ready to process and play audio at the specified sample rate and block size
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);

 }

//this function is responsible for processing and applying any audio effects to the audio data during playback
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

//this function is used to release or clean up any resources that were previously allocated for audio playback
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//this function paints the playlist component (background and color)
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colour::fromRGB(29, 22, 22));
}

//this function lays out the child component and resize it
void MainComponent::resized()
{
    deckGUI1.setBounds(0, 0, getWidth() / 2,getHeight() * 2 / 3);
    deckGUI2.setBounds(getWidth()/2, 0, getWidth() / 2, getHeight() * 2 / 3);
    
    playlistComponent.setBounds(0, getHeight() - getHeight() * 1 / 3, getWidth(), getHeight() / 3);
}

