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
    juce::Image& image) :
        viewportImage(image)
{
    imageBounds = viewportImage.getBounds();
    setBounds(imageBounds);
}

void
medusa::ImageViewport::zoomContainerIn()
{
    const int width  = imageBounds.getWidth();
    const int height = imageBounds.getHeight();

    const int maxWidth  = viewportImage.getWidth() * 4;
    const int maxHeight = viewportImage.getHeight() * 4;

    if (width * 2 < maxWidth and height * 2 < maxHeight)
    {
        imageBounds *= 2.0f;
        imageBounds.setCentre(getLocalBounds().getCentre());
        repaint();
    }
}

void
medusa::ImageViewport::zoomContainerOut()
{
    const int width  = imageBounds.getWidth();
    const int height = imageBounds.getHeight();

    const int minWidth  = viewportImage.getWidth() / 4;
    const int minHeight = viewportImage.getHeight() / 4;

    if (width / 2 > minWidth and height / 2 > minHeight)
    {
        imageBounds /= 2.0f;
        imageBounds.setCentre(getLocalBounds().getCentre());
        repaint();
    }
}

void
medusa::ImageViewport::resetContainer()
{
    imageBounds = viewportImage.getBounds();
    imageBounds.setCentre(getLocalBounds().getCentre());
    repaint();
}

void
medusa::ImageViewport::mouseDown(
    const juce::MouseEvent& e)
{
    preDragPosition = imageBounds.getPosition();
}

void
medusa::ImageViewport::mouseDrag(const juce::MouseEvent& e)
{
    imageBounds.setPosition(
        preDragPosition.x + e.getDistanceFromDragStartX(),
        preDragPosition.y + e.getDistanceFromDragStartY()
    );
    repaint();
}

void medusa::ImageViewport::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    if (e.mods.isCommandDown())
    {
        const auto maxSize = viewportImage.getBounds() * 4;
        const auto minSize = viewportImage.getBounds() / 4;

        const float diff = (1.0f - w.deltaY);

        const auto newSize = imageBounds.withZeroOrigin() * diff;

        if (not newSize.contains(minSize) or not maxSize.contains(newSize))
            return;

        imageBounds = newSize.withPosition(imageBounds.getPosition());
    }
    else
    {
        imageBounds.translate(
            juce::roundToInt(w.deltaX * 128),
            juce::roundToInt(w.deltaY * 128)
        );
    }

    repaint();
}

void
medusa::ImageViewport::paint(
    juce::Graphics& g)
{
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
    g.drawImage(viewportImage, imageBounds.toFloat());
}
