/* 
 * File:   streamparser.h
 * Author: georg
 *
 * Created on 19 marzo 2014, 10.29
 */

#ifndef STREAMPARSER_H
#define	STREAMPARSER_H

#include "instream.h"

class ParserElement {
public:
    ParserElement* next;

    virtual void parse(InStream& src) {}
    ParserElement(ParserElement* mynext): next(mynext) { }
};

class Int32Element : public ParserElement{
public:
    Int32 data;
    Int32 defaultval;

    void parse(InStream& src) { src >> data; }
    Int32Element(ParserElement* mynext=NULL, Int32 defval=0) : defaultval(defval), ParserElement(mynext) {}
};

template<Int32 I>
class StrElement : public ParserElement {
public:
    Char data[I];
    Char const* defaultval;

    void parse(InStream& src) {
        src.setMaxStringLen(I);
        src >> data;
    }
    StrElement(ParserElement* mynext=NULL, Char const* defval=NULL) : defaultval(defval), ParserElement(mynext) {}

};

class SkipElement : public ParserElement {
public:
    const InStream::NilClazz nil;
    void parse(InStream& src) { src >> nil; }
    SkipElement(ParserElement* mynext=NULL) : ParserElement(mynext) {}
};

class StreamParser {
    ParserElement* firstelement;
public:
    StreamParser(ParserElement* first) : firstelement(first) {}

    bool parse(InStream& src, Char* separators=NULL );
};



#endif	/* STREAMPARSER_H */
