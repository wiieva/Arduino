
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
    void setVolume (int volume);

protected:
    bool run ();

    cbuf buf;
    int mode;
    bool bufFilled;
    uint32_t ph_acc;
    int32_t shift;

};
