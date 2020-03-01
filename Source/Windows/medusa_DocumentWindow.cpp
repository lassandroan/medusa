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

#include "medusa_DocumentWindow.h"
#include "../Application/medusa_Application.h"
#include "../Application/medusa_Commands.h"

medusa::DocumentWindow::DocumentWindow(
    medusa::Document document) :
        juce::DocumentWindow(
            document.getDocumentTitle(),
            juce::Colours::black,
            7
        ),
        imageProcessor(*this),
        document(document),
        editor(*this)
{
    document.addChangeListener(this);

    setUsingNativeTitleBar(true);

    setContentOwned(new DocumentComponent(*this), true);

    setResizable(true, false);

    closeButtonGuard = false;
}

void
medusa::DocumentWindow::closeButtonPressed()
{
    // FIXME (02/29/20): Even though the save dialog is modal, the close button
    //                   on macOS is still receiving clicks and triggering this
    //                   callback more times. It will even create a dialog each
    //                   time and cause all kinds of nasty issues.
    if (closeButtonGuard)
        return;

    juce::ScopedValueSetter<bool> closeGuard(closeButtonGuard, true, false);

    using SaveResult = juce::FileBasedDocument::SaveResult;

    DBG("clicky");

    switch (document.saveIfNeededAndUserAgrees())
    {
        case SaveResult::failedToWriteToFile:
            // TODO (02/29/20): Needs error handling
            jassertfalse;
            juce::AlertWindow::showMessageBox(
                juce::AlertWindow::WarningIcon,
                "Unable to save file",
                ""
            );
            return;

        case SaveResult::userCancelledSave:
            DBG("cancel");
            return;

        case SaveResult::savedOk:
            for (auto* const window : pluginWindows)
                window->removeFromDesktop();
            break;

        default:
            DBG("WTF");
            break;
    };

    medusa::Application::getInstance().documentWindows.removeObject(this);
}

juce::ApplicationCommandTarget*
medusa::DocumentWindow::getNextCommandTarget()
{
    return nullptr;
}

void
medusa::DocumentWindow::getAllCommands(
    juce::Array<juce::CommandID> &commands)
{
    commands.add(
        CommandIDs::Save,
        CommandIDs::SaveAs,
        CommandIDs::Close,
        CommandIDs::ZoomIn,
        CommandIDs::ZoomOut,
        CommandIDs::ActualSize
    );
}

void
medusa::DocumentWindow::getCommandInfo(
    juce::CommandID command,
    juce::ApplicationCommandInfo &info)
{
    using CommandFlags = juce::ApplicationCommandInfo::CommandFlags;

    switch (info.commandID)
    {
        case CommandIDs::Save:
            info.setInfo(
                "Save...",
                "",
                "",
                document.hasChangedSinceSaved() ? 0 : CommandFlags::isDisabled
            );
            break;

        case CommandIDs::SaveAs:
            info.setInfo(
                "Save As...",
                "",
                "",
                0
            );
            break;

        case CommandIDs::Close:
            info.setInfo(
                "Close",
                "",
                "",
                0
            );
            break;

        case CommandIDs::ZoomIn:
            info.setInfo(
                "Zoom In",
                "",
                "",
                0
            );
            break;

        case CommandIDs::ZoomOut:
            info.setInfo(
                "Zoom Out",
                "",
                "",
                0
            );
            break;

        case CommandIDs::ActualSize:
            info.setInfo(
                "Actual Size",
                "",
                "",
                0
            );
            break;
    };
}

bool
medusa::DocumentWindow::perform(
    const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
        case CommandIDs::Close:
            closeButtonPressed();
            break;
    }

    return false;
}

void
medusa::DocumentWindow::changeListenerCallback(
    juce::ChangeBroadcaster*)
{
    auto* const peer = getPeer();
    peer->setRepresentedFile(document.getFile());
    peer->setDocumentEditedStatus(document.hasChangedSinceSaved());
}

bool
medusa::DocumentWindow::keyPressed(
    const juce::KeyPress& k)
{
    if (k.getKeyCode() == juce::KeyPress::spaceKey)
    {
        imageProcessor.processImage(document.image);
        document.changed();
        return true;
    }

    return false;
}

void
medusa::DocumentWindow::resized()
{
    if (auto* const c = getContentComponent())
        c->setSize(getWidth(), getHeight());
}

void
medusa::DocumentWindow::refreshImage()
{
    juce::MessageManagerLock lock(&imageProcessor);
    imageProcessor.signalThreadShouldExit();
    repaint();
}
