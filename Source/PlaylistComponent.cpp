/*====================================================================
PlaylistComponent.cpp
I have created this with the help of the tutorial from this course for framework of this file. I update and added on functions which are extensions.
This class is designed to handle a playlist interface in a JUCE-based audio application. It allows users to load, search, play, and remove tracks in a playlist. And when they press play, it will load into deckGUI.
====================================================================*/


#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "DeckGUI.h" 

PlaylistComponent::PlaylistComponent(DeckGUI& deck1, DeckGUI& deck2)
    : deckGUI1(deck1), deckGUI2(deck2), activeDeckGUI(&deck1)
{
    //initializing the loadbutton
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    //initializing the table with the table content
    tableComponent.getHeader().addColumn("Track title", 1, 200); //for the first column
    tableComponent.getHeader().addColumn("", 2, 200); //for the second column
    tableComponent.getHeader().addColumn("", 3, 200); //for the third column
    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
    //initializing the search box
    addAndMakeVisible(searchBox);

    //styling the table background
    tableComponent.setColour(juce::TableListBox::backgroundColourId, juce::Colours::transparentBlack);

    //styling the load button
    loadButton.setColour(TextButton::buttonColourId, juce::Colours::darkcyan); //button background
    loadButton.setColour(TextButton::textColourOffId, juce::Colours::white); //text color
    loadButton.setColour(TextButton::textColourOnId, juce::Colours::white); //text when pressed

    //styling the search text box
    searchBox.setTextToShowWhenEmpty("Search...", juce::Colour::fromRGB(157, 178, 191)); //set placeholder for the textbox
    searchBox.onTextChange = [this]() { searchTracks(); };  //trigers the serchTracks() function when user interacts with the search box
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::white); //set text color
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromRGB(39, 55, 77)); //set background color

}

PlaylistComponent::~PlaylistComponent()
{
}

//This function manages the logic when a row is selected. when the user presses the play button on the selected row, the program will choose between two deckGUI to load the audio. If both deckGUI have an audio loaded, it will show a message instead and nothing will happen
void PlaylistComponent::onRowSelected(int rowIndex)
{
    //variables to check if audio is loaded in either deckGUI
    bool audioLoaded1 = deckGUI1.CheckAudioLoaded();
    bool audioLoaded2 = deckGUI2.CheckAudioLoaded();

    //gets track url from getTrack function
    juce::URL track = getTrack(rowIndex);

    //switch activeDeckGUI based on the row selection logic. (if audioloaded1/audioloaded2 is false) then it will proceed
    if (!audioLoaded1)
    {
        activeDeckGUI = &deckGUI1;
    }
    else if (!audioLoaded2) {
        activeDeckGUI = &deckGUI2;
    }
    else {
        activeDeckGUI = nullptr;
        std::cout << "both tracks are in use right now! " << std::endl;
    }

    //checks if the variable activeDeckGUI is null, it will only go through if it is not null. If not it will not load the track.
    if (activeDeckGUI != nullptr) {
        //function to load the track into deckGUI
        activeDeckGUI->loadTrackFromPlaylist(track);
    }
}

//this function paints the playlist component (background and color)
void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (Colour::fromRGB(29, 22, 22));//colors the background to fit the color theme
}

//this function lays out the child component and resize it
void PlaylistComponent::resized()
{
    //variable to calculates the height
    float searchloadbarheight = getHeight() / 8; 

    //resizing the table
    tableComponent.setBounds(0, searchloadbarheight, getWidth(), searchloadbarheight * 7);

    //variable to calculate the width of each column
    int tableWidth = tableComponent.getWidth();

    //adjust the column width dynamically as a percentage of the table width.
    tableComponent.getHeader().setColumnWidth(1, tableWidth * 0.8); //80%
    tableComponent.getHeader().setColumnWidth(2, tableWidth * 0.1); //10%
    tableComponent.getHeader().setColumnWidth(3, tableWidth * 0.1); //10%

    //resizing the searchBox
    searchBox.setBounds(0, 0, getWidth() / 8 * 7, searchloadbarheight);

    //resizing the load button
    loadButton.setBounds(getWidth() - getWidth() / 8, 0, getWidth() / 8, searchloadbarheight);
}

//this function gets the size of the filtereed tracks to determine and return the number of rows
int PlaylistComponent::getNumRows() 
{
    return filteredTracks.size();
}

//this function changes the background of the rows.
void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) 
{
    if (rowIsSelected) {
        g.fillAll(Colour::fromRGB(82, 109, 130)); //changes color when the row is selected
    }
    else if (rowNumber % 2 == 0) {
        g.fillAll(Colour::fromRGB(39, 55, 77)); //changes even column color
    }
    else {
        g.fillAll(Colours::transparentBlack); //changes odd column number
    }
}

//this function styles the text inside each cell for the table
void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) 
{
    const auto& track = filteredTracks[rowNumber];
    //checks if column id = 1
    if (columnId == 1) {
        g.setColour(Colours::white);
        g.drawText(track.getFileNameWithoutExtension(), 2, 0, width - 4, height, Justification::centredLeft, true); //draw the title of the file loaded
    }
}

//this function refreshes and updates components for specific cells in a table for buttons
Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) 
{
    //check if its column id = 2
    if (columnId == 2) {
        //it will only draw the button if the cell is null
        if (existingComponentToUpdate == nullptr) {
            auto* btn = new TextButton{ "Play" };
            btn->setColour(TextButton::buttonColourId, juce::Colours::darkcyan); //button background
            btn->setColour(TextButton::textColourOffId, juce::Colours::white); //text color
            btn->setColour(TextButton::textColourOnId, juce::Colours::white); //text when pressed
            btn->onClick = [this, rowNumber]() { onRowSelected(rowNumber);; }; //capture row number and runs removeTrack() function when clicked
            existingComponentToUpdate = btn; //updates existingComponentToUpdate to the button
        }
    }
    //check if its column id = 2
    if (columnId == 3) {
        //it will only draw the button if the cell is null
        if (existingComponentToUpdate == nullptr) {
            auto* btn = new TextButton{ "Remove" };
            btn->setColour(TextButton::buttonColourId, juce::Colours::darkcyan); //button background
            btn->setColour(TextButton::textColourOffId, juce::Colours::white); //text color
            btn->setColour(TextButton::textColourOnId, juce::Colours::white); //text when pressed
            btn->onClick = [this, rowNumber]() { removeTrack(rowNumber); }; //capture row number and runs removeTrack() when clicked
            existingComponentToUpdate = btn; //updates existingComponentToUpdate to the button
        }
    }
    return existingComponentToUpdate;
}

//this function handles the eventlistener for button when it is clicked
void PlaylistComponent::buttonClicked(Button* button) 
{
    //runs when loadbutton is clicked
    if (button == &loadButton) {
        auto fileChooserFlags =
            FileBrowserComponent::canSelectFiles;
        //launching the file chooser
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                if (chosenFile.exists()) {
                    //adds entry to tracks
                    tracks.push_back(chosenFile);
                    //append tracks to filteredTracks to show the entry immediately after loading it
                    filteredTracks = tracks;
                    tableComponent.updateContent();
                }
            });

    }
}

//this function gets the track url from the filterTrack vector and return it.
juce::URL PlaylistComponent::getTrack(int index) const
{
    //checks if the index is within the range of the vector
    if (index >= 0 && index < filteredTracks.size()) {
        return URL{filteredTracks[index]}; //returns the URL
    }
    else {
        return juce::URL(); //returns nothing if the index is not in range
    }
}

//this function removes the track from both vectors (tracks and filteredTracks)
void PlaylistComponent::removeTrack(int rowNumber)
{
    //checks if the index is within the range of the vector
    if (rowNumber >= 0 && rowNumber < filteredTracks.size()) {
        //get the track to be removed
        juce::File trackToRemove = filteredTracks[rowNumber];

        //find the track in the original tracks vector and remove it
        auto it = std::find(tracks.begin(), tracks.end(), trackToRemove);
        if (it != tracks.end()) {
            tracks.erase(it); //remove from tracks
        }

        //now remove it from filteredTracks as well
        filteredTracks.erase(filteredTracks.begin() + rowNumber); //remove from filteredTracks

        //update the table to reflect the changes
        tableComponent.updateContent();
    }
}

//this function searches for the tracks based on the user input in the search box
void PlaylistComponent::searchTracks() 
{
    String searchText = searchBox.getText(); //get the text from the search box

    //if search box is empty, show all tracks in the playlist
    if (searchText.isEmpty()) {
        filteredTracks = tracks; //reset filteredTracks to show all tracks
    }
    else {
        //clear the current tracks that match the search text
        filteredTracks.clear();

        //loop through the tracks and check if they match the search text
        for (const auto& track : tracks) {
            //case-insensitive search
            if (track.getFileName().containsIgnoreCase(searchText)) {
                filteredTracks.push_back(track);  //add matching tracks to filtered list
            }
        }
    }

    //update the table with the filtered list
    tableComponent.updateContent();
}