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

#include "../Windows/medusa_DocumentWindow.h"
#include "../Windows/medusa_PluginWindow.h"

namespace medusa
{

class Application : public juce::JUCEApplication
{
public:
    Application()  = default;
    ~Application() = default;

    // application
    static Application& getApplication();
    static juce::ApplicationCommandManager& getCommandManager();

    const juce::String getApplicationName() override    { return ProjectInfo::projectName;   }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override          { return false;                      }

    void initialise(const juce::String& commandLineParameters) override;
    void registerKeyMap();
    void shutdown() override;

    void systemRequestedQuit() override;

    // menubar
    void createMenu(juce::PopupMenu& menu, const juce::String& menuName);
    void createFileMenu(juce::PopupMenu& menu);
    void createViewMenu(juce::PopupMenu& menu);

    // commands
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& info) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

    void updateCommands();

    // document/window handlers
    void createNewDocument();
    void saveDocument(DocumentWindow* window);
    void saveDocumentAs(DocumentWindow* window);
    void closeDocument(DocumentWindow* window);
    void closeAllDocuments();

    DocumentWindow* getActiveWindow();

    void scanForPlugins();
    void closePlugin(PluginWindow* window);

    std::unique_ptr<juce::ApplicationCommandManager> commandManager;

    juce::OwnedArray<DocumentWindow> documentWindows;
    juce::OwnedArray<PluginWindow> pluginWindows;

    std::unique_ptr<juce::XmlElement> pluginListCache;
    juce::KnownPluginList knownPluginList;

private:
    struct MenuModel : public juce::MenuBarModel
    {
        MenuModel()
        {
            setApplicationCommandManagerToWatch(&getCommandManager());
        }

        juce::StringArray getMenuBarNames() override
        {
            const char* const names [] = { "File", "View", nullptr };
            return juce::StringArray(names);
        }

        juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override
        {
            juce::PopupMenu menu;
            getApplication().createMenu(menu, menuName);
            return menu;
        }

        void menuItemSelected(int menuItemID, int topLevelMenuIndex) override {}
    };
    
    std::unique_ptr<MenuModel> menuModel;
    
    juce::TooltipWindow tooltipWindow;
};

}
