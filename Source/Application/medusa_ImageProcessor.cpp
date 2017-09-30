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

#include "medusa_ImageProcessor.h"
#include "medusa_DocumentWindow.h"

medusa::ImageProcessor::ImageProcessor(medusa::DocumentWindow& parent) : juce::Thread("ImageProcessor"), parentWindow(parent)
{

    setPlayConfigDetails(1, 1, 44100, 512);
    setProcessingPrecision(juce::AudioProcessor::singlePrecision);

    addNode(new juce::AudioProcessorGraph::AudioGraphIOProcessor(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    addNode(new juce::AudioProcessorGraph::AudioGraphIOProcessor(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

}

medusa::ImageProcessor::~ImageProcessor()
{

    releaseResources();
    stopThread(200);

}

void medusa::ImageProcessor::processImage(juce::Image& image)
{

    if (getNumNodes() == 2)
        return;

    imagePtr = &image;

    startThread();

}

void medusa::ImageProcessor::run()
{

    std::cout << "Entering thread" << std::endl;

    if (!imagePtr)
    {

        signalThreadShouldExit();

    }
    else
    {

        juce::ScopedPointer<juce::Image::BitmapData> data = new juce::Image::BitmapData(*imagePtr, juce::Image::BitmapData::readWrite);

        int totalSamples = (data->width * data->height) * data->pixelStride;

        juce::AudioBuffer<float> tempbuffer(2, 512);
        juce::MidiBuffer midibuff;

        int offset = 0;

        prepareToPlay(44100, 512);

        while (offset < totalSamples)
        {

            int numSamples = 512;

            if (totalSamples - offset >= 512)
            {
                numSamples = 512;
            }
            else
            {
                numSamples = totalSamples - offset;
            }

            for (int i = offset; i < numSamples + offset; ++i)
            {

                tempbuffer.setSample(0, i - offset, ((float)data->data[i] / (float)255));
                
            }

            processBlock(tempbuffer, midibuff);

            for (int i = offset; i < numSamples + offset; ++i)
            {

                data->data[i] = (int)(tempbuffer.getSample(0, i - offset) * 255.0f);

            }

            offset += numSamples;

        }

        parentWindow.refreshImage();

    }

    while(!threadShouldExit())
    {

        sleep(50);

    }

    std::cout << "Exiting thread" << std::endl;
    
}
