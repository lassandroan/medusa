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

medusa::ImageViewport::ImageViewport(juce::Image& image) : viewportImage(image)
{

    setInterceptsMouseClicks(true, false);

    int width  = viewportImage.getWidth();
    int height = viewportImage.getHeight();

    imageContainer.setSize(width, height);
    imageContainer.setTopLeftPosition(0, 0);
    addChildComponent(imageContainer);

    setSize(width, height);

}

void medusa::ImageViewport::zoomContainerIn()
{

    int width  = imageContainer.getWidth();
    int height = imageContainer.getHeight();

    int maxWidth  = viewportImage.getWidth() * 4;
    int maxHeight = viewportImage.getHeight() * 4;

    if (imageContainer.getWidth() * 2 >= maxWidth || imageContainer.getHeight() * 2 >= maxHeight)
        return;

    juce::Point<int> centre = imageContainer.getPosition().translated(width / 2, height / 2);
    imageContainer.setSize(width * 2, height * 2);
    imageContainer.setCentrePosition(centre);

}

void medusa::ImageViewport::zoomContainerOut()
{

    int width  = imageContainer.getWidth();
    int height = imageContainer.getHeight();

    int minWidth  = viewportImage.getWidth() / 4;
    int minHeight = viewportImage.getHeight() / 4;

    if (imageContainer.getWidth() / 2 <= minWidth || imageContainer.getHeight() / 2 <= minHeight)
        return;

    juce::Point<int> centre = imageContainer.getPosition().translated(width / 2, height / 2);
    imageContainer.setSize(width / 2, height / 2);
    imageContainer.setCentrePosition(centre);

}

void medusa::ImageViewport::resetContainer()
{

    imageContainer.centreWithSize(viewportImage.getWidth(), viewportImage.getHeight());

}

void medusa::ImageViewport::mouseDown(const juce::MouseEvent& e)
{

    dragger.startDraggingComponent(&imageContainer, e);

}

void medusa::ImageViewport::mouseDrag(const juce::MouseEvent& e)
{

    dragger.dragComponent(&imageContainer, e, nullptr);
    repaint();

}

void medusa::ImageViewport::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{

    if (e.mods.isCommandDown())
    {

        int maxWidth  = viewportImage.getWidth() * 4;
        int maxHeight = viewportImage.getHeight() * 4;
        int minWidth  = viewportImage.getWidth() / 4;
        int minHeight = viewportImage.getHeight() / 4;

        float diff = (1.0f - w.deltaY);

        if (diff < 1.0f)
        {

            if (imageContainer.getWidth() * diff <= minWidth || imageContainer.getHeight() * diff <= minHeight)
                return;

        }
        else
        {

            if (imageContainer.getWidth() * diff >= maxWidth || imageContainer.getHeight() * diff >= maxHeight)
                return;

        }

        imageContainer.setSize(imageContainer.getWidth() * diff, imageContainer.getHeight() * diff);

    }
    else
    {

        imageContainer.setTopLeftPosition
        (
            imageContainer.getPosition().translated(-w.deltaX * 128, -w.deltaY * 128)
        );

    }

    repaint();

}

void medusa::ImageViewport::paint(juce::Graphics& g)
{

    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    g.drawImage
    (
        viewportImage,
        imageContainer.getX(), imageContainer.getY(),
        imageContainer.getWidth(), imageContainer.getHeight(),
        0, 0,
        viewportImage.getWidth(), viewportImage.getHeight()
    );

}

void medusa::ImageViewport::resized()
{

    

}
