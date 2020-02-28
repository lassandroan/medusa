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

#include "medusa_DocumentComponent.h"

#include "../Application/medusa_DocumentWindow.h"

medusa::DocumentComponent::DocumentComponent(medusa::DocumentWindow& parent) : parentWindow(parent)
{
    imageViewport = std::make_unique<medusa::ImageViewport>(
        parent.getDocumentImage()
    );
    addAndMakeVisible(imageViewport.get());

    int width  = imageViewport->getWidth();
    int height = imageViewport->getHeight();

    if (width < 300)
        width = 300;

    if (height < 300)
        height = 300;

    pluginListModel = std::make_unique< medusa::DocumentPluginList>(
        parent.getDocumentProcessor(), pluginList
    );
    pluginList.setModel(pluginListModel.get());
    pluginList.setSize(150, height);
    pluginList.setTopLeftPosition(width, 0);
    addAndMakeVisible(pluginList);

    setSize(imageViewport->getWidth() + 150, height);

}

void medusa::DocumentComponent::resized()
{

    int width  = getWidth();
    int height = getHeight();

    imageViewport->setSize(width - 100, height);
    pluginList.setSize(150, height);
    pluginList.setTopLeftPosition(width - 100, 0);

}
