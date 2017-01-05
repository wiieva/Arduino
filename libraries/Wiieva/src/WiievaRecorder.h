#pragma once

#include <WiievaWiring.h>
#include <Stream.h>
#include <cbuf.h>

class WiievaRecorder : public Stream
{
public:
    WiievaRecorder (size_t bufSize) : buf (bufSize)
    {}

    void start (int mode);
    void stop  ();
    bool run ();
    bool checkVad ();
    size_t recordedSize ();

//    String Post (const String &url,const String &contentType);
    int available();
    int read();
    size_t write(uint8_t);
    int peek();
    void flush();

protected:

    cbuf buf;
    int mode;
};
