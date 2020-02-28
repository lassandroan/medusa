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
#include "medusa_CommandIDs.h"

medusa::Application::Application()
{

}

medusa::Application::~Application()
{

}

medusa::Application& medusa::Application::getApplication()
{

    medusa::Application* const application = dynamic_cast<medusa::Application*>(juce::JUCEApplication::getInstance());
    jassert(application != nullptr);
    return *application;

}

juce::ApplicationCommandManager& medusa::Application::getCommandManager()
{

    auto& manager = medusa::Application::getApplication().commandManager;
    jassert(manager.get() != nullptr);
    return *manager.get();

}

void medusa::Application::initialise(const juce::String& commandLineParameters)
{
    commandManager = std::make_unique<juce::ApplicationCommandManager>();
    commandManager->registerAllCommandsForTarget(this);

    registerKeyMap();

    menuModel = std::make_unique<MenuModel>();
    juce::MenuBarModel::setMacMainMenu(menuModel.get());

    scanForPlugins();
}

void medusa::Application::registerKeyMap()
{

    auto keyPressMappingSet = commandManager->getKeyMappings();

    keyPressMappingSet->addKeyPress(CommandIDs::open,       juce::KeyPress::createFromDescription("CMD + O"));
    keyPressMappingSet->addKeyPress(CommandIDs::save,       juce::KeyPress::createFromDescription("CMD + S"));
    keyPressMappingSet->addKeyPress(CommandIDs::saveAs,     juce::KeyPress::createFromDescription("CMD + SHIFT + S"));
    keyPressMappingSet->addKeyPress(CommandIDs::close,      juce::KeyPress::createFromDescription("CMD + W"));

    keyPressMappingSet->addKeyPress(CommandIDs::zoomIn,     juce::KeyPress::createFromDescription("CMD + +"));
    keyPressMappingSet->addKeyPress(CommandIDs::zoomOut,    juce::KeyPress::createFromDescription("CMD + -"));
    keyPressMappingSet->addKeyPress(CommandIDs::actualSize, juce::KeyPress::createFromDescription("CMD + 0"));

}

void medusa::Application::shutdown()
{

    juce::MenuBarModel::setMacMainMenu(nullptr);

    commandManager = nullptr;
    menuModel = nullptr;

}

void medusa::Application::systemRequestedQuit()
{

    closeAllDocuments();

    juce::JUCEApplication::systemRequestedQuit();

}

void medusa::Application::createMenu(juce::PopupMenu &menu, const juce::String &menuName)
{

    if (menuName == "File")
        createFileMenu(menu);

    else if (menuName == "View")
        createViewMenu(menu);

}

void medusa::Application::createFileMenu(juce::PopupMenu &menu)
{

    menu.addCommandItem(commandManager.get(), CommandIDs::open);

    menu.addSeparator();

    menu.addCommandItem(commandManager.get(), CommandIDs::save);
    menu.addCommandItem(commandManager.get(), CommandIDs::saveAs);

    menu.addSeparator();

    menu.addCommandItem(commandManager.get(), CommandIDs::close);

}

void medusa::Application::createViewMenu(juce::PopupMenu& menu)
{

    menu.addCommandItem(commandManager.get(), CommandIDs::zoomIn);
    menu.addCommandItem(commandManager.get(), CommandIDs::zoomOut);
    menu.addCommandItem(commandManager.get(), CommandIDs::actualSize);

}

juce::ApplicationCommandTarget* medusa::Application::getNextCommandTarget()
{

    return nullptr;

}

void medusa::Application::getAllCommands(juce::Array<juce::CommandID>& commands)
{

    JUCEApplication::getAllCommands(commands);

    const juce::CommandID ids [] =
    {

        CommandIDs::open,
        CommandIDs::save,
        CommandIDs::saveAs,
        CommandIDs::close,

        CommandIDs::zoomIn,
        CommandIDs::zoomOut,
        CommandIDs::actualSize,

    };

    commands.addArray(ids, juce::numElementsInArray(ids));

}

void medusa::Application::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{

    int documentsOpen = (documentWindows.size() > 0) ? 0 : 1;

    switch (commandID)
    {

        case CommandIDs::open:
            result.setInfo("Open...", "Open an image to process", "", 0);
            break;

        case CommandIDs::save:
            result.setInfo("Save", "Save the current image", "", documentsOpen);
            break;

        case CommandIDs::saveAs:
            result.setInfo("Save as...", "Save the current image as a new file", "", documentsOpen);
            break;

        case CommandIDs::close:
            result.setInfo("Close", "Close the current image", "", documentsOpen);
            break;

        case CommandIDs::zoomIn:
            result.setInfo("Zoom In", "Zoom in 2x on the current image", "", documentsOpen);
            break;

        case CommandIDs::zoomOut:
            result.setInfo("Zoom Out", "Zoom out 2x on the current image", "", documentsOpen);
            break;

        case CommandIDs::actualSize:
            result.setInfo("Actual Size", "Default to 100% zoom", "", documentsOpen);
            break;

        default:
            JUCEApplication::getCommandInfo(commandID, result);
            break;

    }

}

bool medusa::Application::perform(const juce::ApplicationCommandTarget::InvocationInfo &info)
{

    medusa::DocumentWindow* activeWindow = getActiveWindow();

    switch (info.commandID)
    {

        case CommandIDs::open:       createNewDocument();                 break;
        case CommandIDs::save:       saveDocument(activeWindow);          break;
        case CommandIDs::saveAs:     saveDocumentAs(activeWindow);        break;
        case CommandIDs::close:      activeWindow->closeButtonPressed();  break;

        case CommandIDs::zoomIn:     activeWindow->zoomIn();              break;
        case CommandIDs::zoomOut:    activeWindow->zoomOut();             break;
        case CommandIDs::actualSize: activeWindow->zoomReset();           break;

        default:                    return juce::JUCEApplication::perform(info);

    }

    return true;

}

void medusa::Application::createNewDocument()
{

    juce::FileChooser fileChooser("Open...", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.png, *.jpeg, *.jpg");

    if (fileChooser.browseForFileToOpen())
    {

        juce::File file(fileChooser.getResult());

        juce::Image image = juce::ImageFileFormat::loadFrom(file);

        if (image.isNull())
        {

            juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", "Could not load image!");

            return;

        }

        if (image.getFormat() == juce::Image::ARGB)
        {

            image = image.convertedToFormat(juce::Image::RGB);
            
        }

        medusa::DocumentWindow* documentWindow = new medusa::DocumentWindow(file.getFileName(), file, image);

        documentWindow->setVisible(true);

        documentWindows.add(documentWindow);

    }

}

void medusa::Application::saveDocument(medusa::DocumentWindow* window)
{

    if (window->isDocumentDirty())
    {

        const juce::File& file = window->getDocumentFile();
        juce::FileOutputStream outputStream(file);

        juce::Image image = window->getDocumentImage();

        bool success = false;

        if (file.getFileExtension() == ".png")
        {

            juce::PNGImageFormat png;
            success = png.writeImageToStream(image, outputStream);

        }
        else if (file.getFileExtension() == ".jpeg")
        {

            juce::JPEGImageFormat jpeg;
            success = jpeg.writeImageToStream(image, outputStream);

        }

        if (!success)
            juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error", "Could not save file");

    }

}

void medusa::Application::saveDocumentAs(medusa::DocumentWindow* window)
{

    juce::FileChooser fileChooser("Save As...", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.png");

    if (fileChooser.browseForFileToSave(true))
    {

        juce::File file(fileChooser.getResult().withFileExtension(".png"));
        juce::FileOutputStream outputStream(file);

        juce::Image image = window->getDocumentImage();
        juce::PNGImageFormat png;
        bool success = png.writeImageToStream(image, outputStream);

        if (!success)
            juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error", "Could not save file");

    }
    
}

void medusa::Application::closeDocument(medusa::DocumentWindow *window)
{

    for (int i = 0; i < pluginWindows.size(); ++i)
    {

        medusa::PluginWindow* pluginWindow = pluginWindows[i];

        medusa::DocumentWindow* target = pluginWindow->getParentWindow();

        if (target == window)
        {

            pluginWindows[i]->removeFromDesktop();
            pluginWindows.remove(i);

        }

    }

    documentWindows.removeObject(window);

}

void medusa::Application::closeAllDocuments()
{

    for (int i = 0; i < pluginWindows.size(); ++i)
    {

        pluginWindows[i]->removeFromDesktop();

    }

    pluginWindows.clear();
    
    for (int i = 0; i < documentWindows.size(); ++i)
    {

        documentWindows[i]->closeButtonPressed();
        
    }
    
}

medusa::DocumentWindow* medusa::Application::getActiveWindow()
{

    for (int i = 0; i < documentWindows.size(); ++i)
    {

        if (documentWindows[i]->isActiveWindow())
            return documentWindows[i];

    }

    return nullptr;

}

void medusa::Application::scanForPlugins()
{

    juce::File appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

    juce::File medusaFolder = juce::File(appData.getFullPathName() + "/Application Support/medusa");

    if (!medusaFolder.exists())
    {

        medusaFolder.createDirectory();

    }

    juce::File XmlCache = juce::File(medusaFolder.getFullPathName() + "/plugincache.xml");

    if (XmlCache.exists())
    {
        std::cout << "Loading plugin list from cache file" << std::endl;

        knownPluginList.recreateFromXml(
            *juce::XmlDocument(XmlCache).getDocumentElement()
        );
    }
    else
    {
        juce::FileSearchPath vstSearchPaths("/Library/Audio/Plug-Ins/VST;~/Library/Audio/Plug-Ins/VST");
        juce::VSTPluginFormat vst;
        juce::PluginDirectoryScanner vstScanner(knownPluginList, vst, vstSearchPaths, true, juce::File());

        bool scanning = true;
        while (scanning)
        {

            juce::String nextPlugin = vstScanner.getNextPluginFileThatWillBeScanned();
            std::cout << nextPlugin << std::endl;

            scanning = vstScanner.scanNextFile(true, nextPlugin);

        }

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

void medusa::Application::closePlugin(medusa::PluginWindow *window)
{

    window->removeFromDesktop();
    pluginWindows.removeObject(window);

}
