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

#include "medusa_DocumentWindow.h"
#include "medusa_Application.h"
#include "medusa_CommandIDs.h"

medusa::DocumentWindow::DocumentWindow(const juce::String& name, const juce::File& file, juce::Image image)
                        : juce::DocumentWindow(name, juce::Colours::black, 7), documentFile(file), imageProcessor(*this)
{

    setUsingNativeTitleBar(true);

    documentImage = image.convertedToFormat(juce::Image::RGB);

    setContentOwned(new DocumentComponent(*this), true);

    setResizable(true, false);

    dirty = false;

}

medusa::DocumentWindow::~DocumentWindow()
{

}

void medusa::DocumentWindow::closeButtonPressed()
{

    medusa::Application& application = medusa::Application::getApplication();

    if (dirty)
    {

        int choice = juce::NativeMessageBox::showYesNoCancelBox(juce::AlertWindow::WarningIcon, "Image has been modified", "Would you like to save this image?");

        if (choice == 0)
        {

            return;

        }
        else if (choice == 1)
        {

            application.saveDocument(this);

        }

    }

    application.closeDocument(this);

}

juce::Image& medusa::DocumentWindow::getDocumentImage()
{

    return documentImage;

}

const juce::File& medusa::DocumentWindow::getDocumentFile()
{

    return documentFile;

}

medusa::ImageProcessor& medusa::DocumentWindow::getDocumentProcessor()
{

    return imageProcessor;

}

void medusa::DocumentWindow::setDocumentStatus(bool isDirty)
{

    dirty = isDirty;

}

bool medusa::DocumentWindow::isDocumentDirty()
{

    return dirty;

}

void medusa::DocumentWindow::zoomIn()
{


    medusa::DocumentComponent* c = dynamic_cast<medusa::DocumentComponent*>(getContentComponent());
    c->imageViewport->zoomContainerIn();
    c->imageViewport->repaint();

}

void medusa::DocumentWindow::zoomOut()
{

    medusa::DocumentComponent* c = dynamic_cast<medusa::DocumentComponent*>(getContentComponent());
    c->imageViewport->zoomContainerOut();
    c->imageViewport->repaint();

}

void medusa::DocumentWindow::zoomReset()
{

    medusa::DocumentComponent* c = dynamic_cast<medusa::DocumentComponent*>(getContentComponent());
    c->imageViewport->resetContainer();
    c->imageViewport->repaint();
    
}

bool medusa::DocumentWindow::keyPressed(const juce::KeyPress& k)
{

    if (k.getKeyCode() == juce::KeyPress::spaceKey)
    {

        updateImage();

        return true;

    }

    return false;

}

void medusa::DocumentWindow::resized()
{

    if (juce::Component* c = getContentComponent())
        c->setSize(getWidth(), getHeight());

}

void medusa::DocumentWindow::updateImage()
{

    imageProcessor.processImage(documentImage);
    dirty = true;

}

void medusa::DocumentWindow::refreshImage()
{

    std::cout << "Refreshing image" << std::endl;

    juce::MessageManagerLock lock(&imageProcessor);

    imageProcessor.signalThreadShouldExit();

    repaint();

}
