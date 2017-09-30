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

namespace medusa
{

class ImageViewport : public juce::Component
{

public:

    ImageViewport(juce::Image& image);

    void zoomContainerIn();
    void zoomContainerOut();
    void resetContainer();

private:

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::ComponentDragger dragger;

    juce::Component imageContainer;
    juce::Image& viewportImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageViewport);

};

}
