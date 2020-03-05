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

#include "medusa_ImageViewport.h"

medusa::ImageViewport::ImageViewport(
    juce::Image& source)
{
    image.setInterceptsMouseClicks(false, false);
    image.setOpaque(true);
    image.setImage(source);
    image.setImagePlacement(juce::RectanglePlacement::fillDestination);
    image.setBounds(source.getBounds());
    addAndMakeVisible(image);

    setBounds(source.getBounds());
}

void
medusa::ImageViewport::zoomContainerIn()
{
    const int width  = image.getWidth();
    const int height = image.getHeight();

    const int maxWidth  = image.getImage().getWidth() * 4;
    const int maxHeight = image.getImage().getHeight() * 4;

    if (width * 2 < maxWidth and height * 2 < maxHeight)
        image.centreWithSize(width * 2, height * 2);
}

void
medusa::ImageViewport::zoomContainerOut()
{
    const int width  = image.getWidth();
    const int height = image.getHeight();

    const int minWidth  = image.getImage().getWidth() / 4;
    const int minHeight = image.getImage().getHeight() / 4;

    if (width / 2 > minWidth and height / 2 > minHeight)
        image.centreWithSize(width / 2, height / 2);
}

void
medusa::ImageViewport::resetContainer()
{
    image.centreWithSize(
        image.getImage().getWidth(),
        image.getImage().getHeight()
    );
}

void
medusa::ImageViewport::mouseDown(
    const juce::MouseEvent& e)
{
    dragger.startDraggingComponent(&image, e);
}

void
medusa::ImageViewport::mouseDrag(
    const juce::MouseEvent& e)
{
    dragger.dragComponent(&image, e, nullptr);
}

void medusa::ImageViewport::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    if (e.mods.isCommandDown())
    {
        const auto maxSize = image.getImage().getBounds() * 4;
        const auto minSize = image.getImage().getBounds() / 4;

        const float diff = (1.0f - w.deltaY);

        const auto newSize = image.getLocalBounds() * diff;

        if (not newSize.contains(minSize) or not maxSize.contains(newSize))
            return;

        image.setSize(newSize.getWidth(), newSize.getHeight());
    }
    else
    {
        image.setBounds(image.getBounds().translated(
            juce::roundToInt(w.deltaX * 128),
            juce::roundToInt(w.deltaY * 128)
        ));
    }

    repaint();
}
