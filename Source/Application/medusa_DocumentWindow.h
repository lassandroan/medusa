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

#include "medusa_ImageProcessor.h"
#include "../Components/medusa_DocumentComponent.h"

namespace medusa
{

class DocumentWindow : public juce::DocumentWindow
{
public:
    DocumentWindow(
        const juce::String& name,
        const juce::File& documentFile,
        juce::Image image
    );

    ~DocumentWindow() = default;

    void closeButtonPressed() override;

    // handlers
    juce::Image& getDocumentImage();
    const juce::File& getDocumentFile();
    ImageProcessor& getDocumentProcessor();

    void setDocumentStatus(bool isDirty);
    bool isDocumentDirty();

    void zoomIn();
    void zoomOut();
    void zoomReset();

    void updateImage();
    void refreshImage();

private:
    bool keyPressed(const juce::KeyPress& k) override;

    void resized() override;

    bool dirty;

    const juce::File& documentFile;
    juce::Image documentImage;
    ImageProcessor imageProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DocumentWindow)
};

}
