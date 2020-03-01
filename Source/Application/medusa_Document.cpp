/*
 *  Copyright Antonio Lassandro
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

#include "medusa_Document.h"

medusa::Document::Document() :
    juce::FileBasedDocument(".medusa", "*.medusa", "Open file", "Save file")
{

}

medusa::Document::Document(const medusa::Document& other) :
    juce::FileBasedDocument(".medusa", "*.medusa", "Open file", "Save file")
{
    jassert(other.image.isValid());
    image = other.image;
    state = other.state;
}

juce::String
medusa::Document::getDocumentTitle()
{
    return getFile().getFileNameWithoutExtension();
}

juce::Result
medusa::Document::loadDocument(
    const juce::File& file)
{
    state = juce::ValueTree::fromXml(file.loadFileAsString());

    if (not state.isValid())
        return juce::Result::fail("Unable to read file");

    if (not state.hasProperty("image"))
        return juce::Result::fail("Unable to read file");

    {
        auto var = state.getProperty("image");
    }

    /// TODO (02/29/20): Not even sure this does anything on macOS, as
    //                   CoreGraphics always gives JUCE ARGB
    if (image.getFormat() == juce::Image::ARGB)
        image = image.convertedToFormat(juce::Image::RGB);

    return juce::Result::ok();
}

juce::Result
medusa::Document::saveDocument(
    const juce::File&)
{
    return juce::Result::ok();
}

juce::File
medusa::Document::getLastDocumentOpened()
{
    // TODO (02/29/20): juce::RecentlyOpenedFileList
    return {};
}

void
medusa::Document::setLastDocumentOpened(
    const juce::File&)
{
    // TODO (02/29/20): juce::RecentlyOpenedFileList
}

juce::File
medusa::Document::getSuggestedSaveAsFile(
    const juce::File &defaultFile)
{
    return defaultFile;
}
