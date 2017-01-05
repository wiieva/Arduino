#include <Arduino.h>
#include <WiievaRecorder.h>

using namespace wiieva;

void WiievaRecorder::start (int _mode)
{
    sendCommand (AIO_CMD_AUDIO_IN_MODE,0,_mode);
    mode = _mode;
    buf.flush ();
}

void WiievaRecorder::stop ()
{
    sendCommand (AIO_CMD_AUDIO_IN_MODE,0,AIO_AUDIO_OFF);
}

bool WiievaRecorder::run ()
{
    // For speex payload we will read 20-byte chunks
    uint8_t readSize = (mode == AIO_AUDIO_IN_SPEEX)?20:128;

    while (buf.room () > readSize+1) {
        uint8_t tmpBuf[128];
        uint8_t sz = recvBuffer (AIO_CMD_RD_STREAM,readSize,tmpBuf);
        // Nothing to read - bailing out
        if (!sz)
            return true;

        // Speex header byte - for google speech recognision
        if (mode == AIO_AUDIO_IN_SPEEX)
            buf.write (sz);

        buf.write ((char *)tmpBuf,sz);
        yield();
    }
    return false;
}


bool WiievaRecorder::checkVad () 
{
    return bool (sendCommand (AIO_CMD_GET_VAD_STATE,SEND_CMD_FL_ANSWER));
}

size_t WiievaRecorder::recordedSize ()
{
    return buf.available ();
}

int WiievaRecorder::available()
{
    return buf.available ()?buf.available():-1;
}

int WiievaRecorder::read()
{
    return buf.read ();

}
size_t WiievaRecorder::write(uint8_t)
{
    return 0;
}

int WiievaRecorder::peek()
{
    return buf.peek ();
}

void WiievaRecorder::flush()
{
    return buf.flush ();
}

