/*====================================================================
PlaylistComponent.h
====================================================================*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>

class DeckGUI;

//this class represents a playlist UI element for loading and managing tracks in a DJ-style audio player interface
class PlaylistComponent  : public juce::Component, public TableListBoxModel, public Button::Listener
{
public:
    PlaylistComponent(DeckGUI& deck1, DeckGUI& deck2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;

    void resized() override;
    
    int getNumRows() override;

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    void buttonClicked(Button* button) override;

    void onRowSelected(int rowIndex);

    //functions to get, remove and search for tracks
    juce::URL getTrack(int index) const;

    void removeTrack(int rowNumber);

    void searchTracks();

    //to draw all the buttons for each cells
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;


private:
    //to choose the file
    juce::FileChooser fChooser{ "Select a file..." };

    //for users to search for audio
    juce::TextEditor searchBox;

    //table for the playlist
    TableListBox tableComponent;

    //vectors to store all the tracks loaded
    std::vector<juce::File> tracks;
    std::vector<juce::File> filteredTracks;


    DeckGUI& deckGUI1; //reference to the first DeckGUI instance
    DeckGUI& deckGUI2; //reference to the second DeckGUI instance
    DeckGUI* activeDeckGUI; //pointer to the currently active DeckGU

    //load button
    juce::TextButton loadButton{ "Load" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
