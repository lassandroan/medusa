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

    class DocumentWindow;

class ImageProcessor : public juce::AudioProcessorGraph,
                       public juce::Thread

{

public:

    ImageProcessor(medusa::DocumentWindow& parent);
    ~ImageProcessor();

    void processImage(juce::Image& image);

    juce::ScopedPointer<juce::AudioProcessorGraph::Node> inputNode, outputNode;

private:

    void run() override;

    juce::Image* imagePtr;

    medusa::DocumentWindow& parentWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageProcessor)

};

}
