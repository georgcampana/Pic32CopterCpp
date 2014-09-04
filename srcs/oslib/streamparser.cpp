

#include "streamparser.h"

// false = no errors
bool StreamParser::parse(InStream& src, Char* separators) {

    bool errorfound = false;

    if(separators!=NULL) { src.setSeparators(separators); }

    ParserElement* element = firstelement;
    while(element) {
        element->parse(src);
        element = element->next;

    }

    if(src.lasterror != InStream::IS_OK) {   errorfound = true;}

    return errorfound;
}


