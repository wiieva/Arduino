
#pragma once

#include <WiievaWiring.h>
#include <Stream.h>
#include <cbuf.h>

class WiievaPlayer /*: public Stream*/
{
public:
    WiievaPlayer (size_t bufSize) : buf (bufSize)
    {}

    void start (int _mode);
    void stop  ();
    bool run (Stream &stream);
    bool run (int freq);

protected:
    bool run ();

    cbuf buf;
    int mode;
    bool bufFilled;
};
