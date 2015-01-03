#pragma once

#include <common/common.h>
#include "common/Exception.h"

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <vector>


class CodecMissingException :public Exception
{
public:
    CodecMissingException() :Exception("can't find the suitable codec"){}
};


class MediaStream
{
public:
protected:
};
