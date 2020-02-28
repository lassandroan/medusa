/*
 *  Copyright 2017 - 2018 Antonio Lassandro
 *
 *  medusa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  medusa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

namespace medusa
{

class DocumentPluginList : public juce::ListBoxModel
{
public:
    DocumentPluginList(juce::AudioProcessorGraph&, juce::ListBox&);

    int getNumRows() override;

private:
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;
    void backgroundClicked(const juce::MouseEvent& e) override;

    void paintListBoxItem(
        int rowNumber,
        juce::Graphics&,
        int width,
        int height,
        bool rowIsSelected) override;

    juce::AudioProcessorGraph& parentGraph;
    juce::ListBox& parentListBox;

    juce::AudioPluginFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DocumentPluginList)
};

}
