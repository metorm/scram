/*
 * Copyright (C) 2015-2018 Wesley Ranger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// Generator to write rich text report to a HTML file

#ifndef HTMLREPORTGENERATOR_H
#define HTMLREPORTGENERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <locale>

namespace scram {
namespace gui {
namespace report {

// ========= class declearations ============

template<typename StringType>
class ReportItemBase{
public:
    typedef typename StringType::value_type CharType;
    typedef typename std::basic_stringstream<typename StringType::value_type> StringStreamType;

    ReportItemBase();
    ReportItemBase(const StringType & title);
    virtual ~ReportItemBase();

    static int ItemTitleHeaderLevel;
    void writeTitle(StringStreamType & s);

    void setTitle(const StringType & newTitle);

    void setWidthPixel(unsigned int width);
    void setWidthPercentage(unsigned int width);

    // 1: left    2: center    3: right    other: unset
    void setAlignStyle(unsigned int tag);

    virtual StringType toHTMLString() = 0;

protected:
    void copyStringVectorAndComplete(const std::vector<StringType> &sourceVec,
                                     std::vector<StringType> &targetVec,
                                     const size_t intendedSize);

    StringType title;
    StringType widthStytleString;
    StringType alignStytleString;
};

template<typename StringType>
class ReportItemTable: public ReportItemBase<StringType>{
public:
    ReportItemTable(const StringType & title,
                const std::vector<StringType> & header,
                const std::vector<std::vector<StringType> > & content,
                size_t cols = -1);
    ReportItemTable(size_t cols);
    ~ReportItemTable();

    void setHeader(const std::vector<StringType> & newHeader);
    void addRow(const std::vector<StringType> &row);

    void setBorderWidth(unsigned int w){borderWidth = w;}
    void setCellPadding(unsigned int p){cellPadding = p;}
    void setInterlaceColor(bool c){interlaceColor = c;}

    StringType toHTMLString();

    bool isValid();

private:
    std::vector<StringType> header;
    std::vector<std::vector<StringType> > content;

    size_t cols;

    bool interlaceColor = true;
    unsigned int borderWidth = 1;
    unsigned int cellPadding = 2;
};

template<typename StringType>
class ReportItemNormal : public ReportItemBase<StringType>
{
public:
    ReportItemNormal();
    ReportItemNormal(const StringType & title, const StringType & content);
	~ReportItemNormal();

    void setContent(const StringType & newContent);
    StringType toHTMLString();

private:
    StringType content;
};

template<typename StringType>
class ReportImages : public ReportItemBase<StringType>
{
public:
    ReportImages();
    ReportImages(const StringType & title);
	~ReportImages();

    void addImage(const StringType & path, const StringType & caption, const int width = -1, const int height = -1);

    void clearImage();

    StringType toHTMLString();

private:
    std::vector<StringType> imagePaths;
    std::vector<StringType> imageScales;
    std::vector<StringType> imageCaptions;
};

template<typename StringType>
class HTMLReportGenerator
{
public:
    HTMLReportGenerator(const std::string &reportPath);

    ~HTMLReportGenerator();

    void setReportTitle(const StringType & title);
    void addReportElement(std::shared_ptr<ReportItemBase<StringType>> elment);

    bool doWrite();

    void setLocale(std::locale newLocal);

private:
    std::basic_ofstream<typename ReportItemBase<StringType>::CharType> file;
    StringType reportTitle;
    std::vector<std::shared_ptr<ReportItemBase<StringType> > > contentElements;

    // write header & tail - automatically called by constructor and destructor
    void writeHeader();
    void writetail();

    // write content for all elements
    void writeContent();
};


// ========= class implementations ============


template<typename StringType>
int ReportItemBase<StringType>::ItemTitleHeaderLevel = 2;

template<typename StringType>
ReportItemBase<StringType>::ReportItemBase() : widthStytleString(StringType()) {}

template<typename StringType>
ReportItemBase<StringType>::~ReportItemBase() {}

template<typename StringType>
ReportItemBase<StringType>::ReportItemBase(const StringType &i_title) : widthStytleString(StringType())
{
    setTitle(i_title);
}

template<typename StringType>
void ReportItemBase<StringType>::writeTitle(StringStreamType &s)
{
    s << "<H" << std::max(1, std::min(6, ReportItemBase::ItemTitleHeaderLevel)) << ">"
      << title
      << "</H" << std::max(1, std::min(6, ReportItemBase::ItemTitleHeaderLevel)) << ">"
      << "<br>"
      << std::endl;
}

template<typename StringType>
void ReportItemBase<StringType>::setTitle(const StringType &newTitle)
{
    title = newTitle;
}

template<typename StringType>
void ReportItemBase<StringType>::setWidthPixel(unsigned int width)
{
    typename ReportItemBase::StringStreamType  ss;
    ss << "width:" << width << "px";
    widthStytleString = ss.str();
}

template<typename StringType>
void ReportItemBase<StringType>::setWidthPercentage(unsigned int width)
{
    typename ReportItemBase::StringStreamType  ss;
    ss << "width:" << std::min(width, 100u) << "%";
    widthStytleString = ss.str();
}

template<typename StringType>
void ReportItemBase<StringType>::setAlignStyle(unsigned int tag)
{
    const std::string LEFT = "left";
    const std::string RIGHT = "right";
    const std::string CENTER = "center";
    switch (tag) {
    case 1:
        alignStytleString = StringType(LEFT.begin(), LEFT.end());
        break;
    case 2:
        alignStytleString = StringType(CENTER.begin(), CENTER.end());
        break;
    case 3:
        alignStytleString = StringType(RIGHT.begin(), RIGHT.end());
        break;
    default:
        alignStytleString = StringType();
        break;
    }
}

template<typename StringType>
void ReportItemBase<StringType>::copyStringVectorAndComplete(const std::vector<StringType> &sourceVec,
        std::vector<StringType> &targetVec,
        const size_t intendedSize)
{
    targetVec.resize(intendedSize);
    for(size_t i = 0; i < std::min(intendedSize, sourceVec.size()); ++i)
    {
        targetVec[i] = sourceVec[i];
    }
    if(sourceVec.size() < intendedSize)
    {
        for(size_t i = sourceVec.size(); i < intendedSize; ++i)
        {
            targetVec.emplace_back();
        }
    }
}

template<typename StringType>
ReportItemTable<StringType>::ReportItemTable(const StringType &i_title, const std::vector<StringType> &i_header, const std::vector<std::vector<StringType> > &i_content, size_t i_cols)
    :ReportItemBase<StringType>(i_title)
{
    if(i_cols == -1)
    {
        // compute cols from input
        cols = 256;
        for(auto i = i_content.begin(); i != i_content.end(); ++i)
        {
            cols = std::min(cols, (*i).size());
        }
    }else {
        cols = i_cols;
    }

    // copy header
    setHeader(i_header);

    // copy content
    content.reserve(i_content.size());
    for(size_t r = 0; r < i_content.size(); ++r)
    {
        addRow(i_content[r]);
    }
}

template<typename StringType>
ReportItemTable<StringType>::ReportItemTable(size_t i_cols) : cols(i_cols) {}

template<typename StringType>
ReportItemTable<StringType>::~ReportItemTable() {}

template<typename StringType>
void ReportItemTable<StringType>::setHeader(const std::vector<StringType> &newHeader)
{
    ReportItemBase<StringType>::copyStringVectorAndComplete(newHeader, header, cols);
}

template<typename StringType>
void ReportItemTable<StringType>::addRow(const std::vector<StringType> &row)
{
    content.emplace_back();
    ReportItemBase<StringType>::copyStringVectorAndComplete(row, content.back(), cols);
}

template<typename StringType>
StringType ReportItemTable<StringType>::toHTMLString()
{
    if(!isValid()) return StringType();

    typename ReportItemBase<StringType>::StringStreamType  htmlStream;

    htmlStream << "<li>" << std::endl;

    ReportItemBase<StringType>::writeTitle(htmlStream);

    htmlStream << "<table align=\"" << ReportItemBase<StringType>::alignStytleString  << "\" "
               << "style=\"border-collapse: collapse; border:" << borderWidth + 1 << "px solid;"
               << "cellpadding:" << cellPadding << "\" "
               << "border=\"" << borderWidth <<"\""
               << " >" /* end of <table ... >*/<< std::endl;

    // header
    htmlStream << "<tr" << (interlaceColor ? " bgcolor=\"#D6DBDF\"" : "") <<">" << std::endl;
    for(size_t c = 0; c < cols; ++ c)
    {
        htmlStream << "<th>" << header[c] << "</th>" << std::endl;
    }
    htmlStream << "</tr>";

    // contents
    bool colorThis = true;
    for(auto i = content.begin(); i != content.end(); ++i)
    {
        colorThis = !colorThis;
        htmlStream << "<tr" << (colorThis ? " bgcolor=\"#EBEDEF\"" : "") <<">" << std::endl;
        for(size_t c = 0; c < cols; ++ c)
        {
            htmlStream << "<td>" << i->at(c) << "</td>" << std::endl;
        }
        htmlStream << "</tr>" << std::endl;
    }

    htmlStream << "</table>" << "<br>" << std::endl;
    htmlStream << "</li>" << std::endl;

    return htmlStream.str();
}

template<typename StringType>
bool ReportItemTable<StringType>::isValid()
{
    if(header.size() < cols) return false;

    for(size_t r = 0; r < content.size(); ++r)
    {
        if(content[r].size() < cols) return false;
    }

    return true;
}

template<typename StringType>
ReportItemNormal<StringType>::ReportItemNormal() {}

template<typename StringType>
ReportItemNormal<StringType>::~ReportItemNormal() {}

template<typename StringType>
ReportItemNormal<StringType>::ReportItemNormal(const StringType &i_title, const StringType &i_content)
    :ReportItemBase<StringType>(i_title), content(i_content)
{}

template<typename StringType>
void ReportItemNormal<StringType>::setContent(const StringType & newContent)
{
    content = newContent;
}

template<typename StringType>
StringType ReportItemNormal<StringType>::toHTMLString()
{
    typename ReportItemBase<StringType>::StringStreamType  htmlStream;

    htmlStream << "<li>" << std::endl;
    ReportItemBase<StringType>::writeTitle(htmlStream);
    htmlStream << content << "<br>" << std::endl;
    htmlStream << "</li>" << std::endl;

    return htmlStream.str();
}

template<typename StringType>
ReportImages<StringType>::ReportImages() {}

template<typename StringType>
ReportImages<StringType>::~ReportImages() {}

template<typename StringType>
ReportImages<StringType>::ReportImages(const StringType &i_title) : ReportItemBase<StringType>(i_title) {}

template<typename StringType>
void ReportImages<StringType>::addImage(const StringType &path, const StringType &caption, const int width, const int height)
{
    imagePaths.push_back(path);
    imageCaptions.push_back(caption);

    // string to set scale:
    // width=1024 height=512
    // this is for MS Office compatible
    typename ReportItemBase<StringType>::StringStreamType ss;
    if(width != -1)
    {
        ss << "width=" << width << ' ';
    }
    if(height != -1)
    {
        ss << "height=" << height;
    }
    imageScales.push_back(ss.str());
}

template<typename StringType>
void ReportImages<StringType>::clearImage()
{
    imagePaths.clear();
    imageCaptions.clear();
}

template<typename StringType>
StringType ReportImages<StringType>::toHTMLString()
{
    if(imagePaths.size() != imageCaptions.size()) return StringType();

    typename ReportItemBase<StringType>::StringStreamType  htmlStream;

    htmlStream << "<li>" << std::endl;
    ReportItemBase<StringType>::writeTitle(htmlStream);

    for(size_t i = 0; i < imagePaths.size(); ++ i)
    {
        htmlStream << "<div style=\"text-align:" << ReportItemBase<StringType>::alignStytleString << "\">"
                   <<"<img src=\"" << imagePaths[i] <<"\" "
                   << "alt=\"" << imageCaptions[i] <<"\" " << imageScales[i] << " style=\"display:block; margin: 0 auto\"/>"
                   << "</div>" << std::endl;
        htmlStream << "<div style=\"text-align:" << ReportItemBase<StringType>::alignStytleString <<"\">"
                   << "<strong>" << imageCaptions[i] << "</strong>" << "<br>"
                   << "</div>"
                   << std::endl;
    }

    htmlStream << "</li>" << std::endl;

    return htmlStream.str();
}

template<typename StringType>
HTMLReportGenerator<StringType>::HTMLReportGenerator(const std::string & reportPath)
{
    file.open(reportPath);
    if (!file) {
        std::cerr << "Can not open file " << reportPath << std::endl;
    }
    else {
        writeHeader();
    }
}

template<typename StringType>
HTMLReportGenerator<StringType>::~HTMLReportGenerator()
{
    if (!file) {
        std::cerr << "No opened file ..." << std::endl;
    }
    else {
        writetail();
        file.close();
    }
}

template<typename StringType>
void HTMLReportGenerator<StringType>::setReportTitle(const StringType & title)
{
    reportTitle = title;
}

template<typename StringType>
void HTMLReportGenerator<StringType>::addReportElement(std::shared_ptr<ReportItemBase<StringType>> elment)
{
    contentElements.push_back(elment);
}

template<typename StringType>
bool HTMLReportGenerator<StringType>::doWrite()
{
    if (!file) {
        std::cerr << "No opened file ..." << std::endl;
        return false;
    }
    else {
        writeContent();
        return true;
    }
}

template<typename StringType>
void HTMLReportGenerator<StringType>::setLocale(std::locale newLocal)
{
    file.imbue(newLocal);
}

template<typename StringType>
void HTMLReportGenerator<StringType>::writeHeader()
{
    file << "<!DOCTYPE html>"<< std::endl
         << "<html lang=\"zh-cn\">" << std::endl
         << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl
         << "<meta charset=\"utf-8\">" << std::endl
         << "<body>" << std::endl;
}

template<typename StringType>
void HTMLReportGenerator<StringType>::writetail()
{
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
}

template<typename StringType>
void HTMLReportGenerator<StringType>::writeContent()
{
    if (!file) {
        std::cerr << "No opened file ..." << std::endl;
    }
    else {
        file << "<h1 style=\"text-align:center\">" << reportTitle << "</h1>" << std::endl;
        file << "<ol>" << std::endl;
        for (size_t i = 0; i < contentElements.size(); i++) {
            StringType content_item = contentElements[i]->toHTMLString();
            file << content_item << std::endl;
        }
        file << "</ol>" << std::endl;
    }
}


}
}
}

#endif // HTMLREPORTGENERATOR_H
