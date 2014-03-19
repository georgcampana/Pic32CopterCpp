

#include "streamparser.h"

// false = no errors
bool StreamParser::parse(InStream& src, Char* separators) {

    bool errorfound = false;

    if(separators!=NULL) { src.setSeparators(separators); }

    while(firstelement) {
        firstelement->parse(src);
        firstelement = firstelement->next;

    }

    if(src.lasterror != InStream::IS_OK) {   errorfound = true;}

    return errorfound;
}


