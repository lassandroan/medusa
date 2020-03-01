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

#include "../Application/medusa_Document.h"
#include "../Application/medusa_ImageProcessor.h"
#include "../Components/medusa_DocumentComponent.h"
#include "medusa_PluginWindow.h"

namespace medusa
{
    class DocumentWindow;
}

class medusa::DocumentWindow final : public  juce::DocumentWindow,
                                     public  juce::ApplicationCommandTarget,
                                     private juce::ChangeListener
{
public:
    DocumentWindow(medusa::Document);
    ~DocumentWindow() = default;

    void closeButtonPressed() override;

    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>&) override;
    void getCommandInfo(juce::CommandID, juce::ApplicationCommandInfo&) override;
    bool perform(const InvocationInfo&) override;

    void refreshImage();

    medusa::ImageProcessor imageProcessor;
    medusa::Document document;
    juce::OwnedArray<medusa::PluginWindow> pluginWindows;

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

    bool keyPressed(const juce::KeyPress& k) override;

    void resized() override;

    medusa::DocumentComponent editor;
    bool closeButtonGuard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DocumentWindow)
};
