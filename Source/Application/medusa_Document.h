/*
 *  Copyright Antonio Lassandro
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

class Document final : public juce::FileBasedDocument
{
public:
    Document();
    ~Document() = default;

    // TODO (02/29/20): This is very likely not compatible with the
    //                  ChangeBroadcaster ancestor of this class
    Document(const Document&);

    juce::Image image;

    juce::String getDocumentTitle() override;

protected:
    juce::Result loadDocument(const juce::File&) override;
    juce::Result saveDocument(const juce::File&) override;

    juce::File getLastDocumentOpened() override;
    void       setLastDocumentOpened(const juce::File&) override;
    juce::File getSuggestedSaveAsFile(const juce::File&) override;

private:
    juce::ValueTree state;
};

}
