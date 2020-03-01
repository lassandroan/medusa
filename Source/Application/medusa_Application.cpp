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

#include "medusa_Application.h"
#include "medusa_Commands.h"

medusa::Application&
medusa::Application::getInstance()
{
    return *dynamic_cast<medusa::Application*>(JUCEApplication::getInstance());
}

const juce::String
medusa::Application::getApplicationName()
{
    return ProjectInfo::projectName;
}

const juce::String
medusa::Application::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

bool
medusa::Application::moreThanOneInstanceAllowed()
{
    return false;
}

void
medusa::Application::initialise(
    const juce::String& commandLineParameters)
{
    // TODO (02/29/20): Allow initial file to open in parameters
    commandManager.registerAllCommandsForTarget(this);

    juce::MenuBarModel::setMacMainMenu(&menuModel);

    scanForPlugins();
}

void
medusa::Application::shutdown()
{
    juce::MenuBarModel::setMacMainMenu(nullptr);
}

void
medusa::Application::systemRequestedQuit()
{
    for (auto* documentWindow : documentWindows)
        documentWindow->closeButtonPressed();

    juce::JUCEApplication::systemRequestedQuit();
}

void
medusa::Application::createMenu(
    juce::PopupMenu& menu,
    const juce::String& menuName)
{
    if (menuName == "File")
        createFileMenu(menu);

    else if (menuName == "View")
        createViewMenu(menu);
}

void
medusa::Application::createFileMenu(
    juce::PopupMenu& menu)
{
    menu.addCommandItem(&commandManager, CommandIDs::New);
    menu.addCommandItem(&commandManager, CommandIDs::Open);

    menu.addSeparator();

    menu.addCommandItem(&commandManager, CommandIDs::Save);
    menu.addCommandItem(&commandManager, CommandIDs::SaveAs);

    menu.addSeparator();

    menu.addCommandItem(&commandManager, CommandIDs::Close);
}

void
medusa::Application::createViewMenu(
    juce::PopupMenu& menu)
{
    menu.addCommandItem(&commandManager, CommandIDs::ZoomIn);
    menu.addCommandItem(&commandManager, CommandIDs::ZoomOut);
    menu.addCommandItem(&commandManager, CommandIDs::ActualSize);
}

juce::ApplicationCommandTarget*
medusa::Application::getNextCommandTarget()
{
    for (auto* const window : documentWindows)
        if (window->isActiveWindow())
            return window;

    return nullptr;
}

void
medusa::Application::getAllCommands(
    juce::Array<juce::CommandID>& commands)
{
    commands.add(
        CommandIDs::New,
        CommandIDs::Open
    );
    JUCEApplication::getAllCommands(commands);
}

void
medusa::Application::getCommandInfo(
    const juce::CommandID commandID,
    juce::ApplicationCommandInfo& info)
{
    using Flags = juce::ApplicationCommandInfo::CommandFlags;

    switch (commandID)
    {
        case CommandIDs::New:
            info.setInfo("New...", "", "", 0);
            break;

        case CommandIDs::Open:
            info.setInfo("Open...", "", "", 0);
            break;

        default:
            JUCEApplication::getCommandInfo(commandID, info);
            break;
    }
}

bool
medusa::Application::perform(
    const juce::ApplicationCommandTarget::InvocationInfo &info)
{
    switch (info.commandID)
    {
        case CommandIDs::New:
        {
            juce::FileChooser fileChooser(
                "Select a source image",
                juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                "*.png, *.jpeg, *.jpg"
            );

            if (not fileChooser.browseForFileToOpen())
                break;

            auto image = juce::ImageFileFormat::loadFrom(
                fileChooser.getResult()
            );

            if (image.isNull())
            {
                juce::AlertWindow::showMessageBox(
                    juce::AlertWindow::WarningIcon,
                    "Error",
                    "Could not load image!"
                );

                break;
            }

//            if (image.getFormat() == juce::Image::ARGB)
//                image = image.convertedToFormat(juce::Image::RGB);

            medusa::Document document;
            document.image = image;
            auto* const documentWindow = new medusa::DocumentWindow(document);
            commandManager.registerAllCommandsForTarget(documentWindow);
            documentWindow->setVisible(true);
            documentWindows.add(documentWindow);
        }
        break;

        case CommandIDs::Open:
        {
            medusa::Document document;
            if (document.loadFromUserSpecifiedFile(true).wasOk())
            {
                auto* const documentWindow = new medusa::DocumentWindow(document);
                commandManager.registerAllCommandsForTarget(documentWindow);
                documentWindow->setVisible(true);
                documentWindows.add(documentWindow);
            }
        }
        break;

        default:
            return juce::JUCEApplication::perform(info);
    }

    return true;
}

void
medusa::Application::scanForPlugins()
{
    const juce::File appData = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory
    );

    const juce::File medusaFolder(
        appData.getFullPathName() + "/Application Support/medusa"
    );

    if (!medusaFolder.exists())
        medusaFolder.createDirectory();

    const juce::File XmlCache(
        medusaFolder.getFullPathName() + "/plugincache.xml"
    );

    if (XmlCache.exists())
    {
        std::cout << "Loading plugin list from cache file" << std::endl;

        knownPluginList.recreateFromXml(
            *juce::XmlDocument(XmlCache).getDocumentElement()
        );
    }
    else
    {
        juce::VSTPluginFormat format;
        juce::PluginDirectoryScanner vstScanner(
            knownPluginList,
            format,
            {
                "/Library/Audio/Plug-Ins/VST;"
                "~/Library/Audio/Plug-Ins/VST;"
            },
            true,
            juce::File()
        );

        juce::String nextPlugin;
        do
        {
            nextPlugin = vstScanner.getNextPluginFileThatWillBeScanned();
            std::cout << nextPlugin << std::endl;
        }
        while (vstScanner.scanNextFile(true, nextPlugin));

        /* AU REMOVED UNTIL MENU NAMING IS CORRECTED */

//        juce::FileSearchPath auSearchPaths("/Library/Audio/Plug-Ins/Components;~/Library/Audio/Plug-Ins/Components");
//        juce::AudioUnitPluginFormat au;
//        juce::PluginDirectoryScanner auScanner(knownPluginList, au, auSearchPaths, true, juce::File());
//
//        scanning = true;
//        while (scanning)
//        {
//
//            juce::String nextPlugin = vstScanner.getNextPluginFileThatWillBeScanned();
//            std::cout << nextPlugin << std::endl;
//
//            scanning = auScanner.scanNextFile(true, nextPlugin);
//
//        }

        XmlCache.create();
        XmlCache.replaceWithText(knownPluginList.createXml()->toString());
    }

    pluginListCache = knownPluginList.createXml();
}
