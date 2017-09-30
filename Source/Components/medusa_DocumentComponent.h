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

#include "medusa_DocumentPluginList.h"
#include "medusa_ImageViewport.h"

namespace medusa
{

    class DocumentWindow;

class DocumentComponent : public juce::Component
{

    friend medusa::DocumentWindow;

public:

    DocumentComponent(DocumentWindow& parent);

private:

    void resized() override;

    juce::ScopedPointer<ImageViewport> imageViewport;

    juce::ScopedPointer<DocumentPluginList> pluginListModel;
    juce::ListBox pluginList;

    medusa::DocumentWindow& parentWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DocumentComponent)

};

}
