#ifndef VIDEO_H
#define VIDEO_H

// Credit: https://en.sfml-dev.org/forums/index.php?topic=20593.0

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

typedef unsigned int nuint;

#define __STDC_CONSTANT_MACROS
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

class Video
{

private:
//Variables
    sf::Texture     m_Texture;
    bool            m_bVideoLoaded;
    bool            m_bImageNeedsUpdate;
    AVFormatContext *m_pFormatCtx;
    std::uint8_t    m_iVideoStream;
    std::uint32_t   m_iFrameSize;
    AVCodecContext  *m_pCodecCtx;
    AVFrame         *m_pFrame;
    AVFrame         *m_pFrameRGB;
    std::uint8_t    *m_pBuffer;
    AVPacket        m_Packet;
    AVCodec         *m_pCodec;
    SwsContext      *img_convert_ctx;
    string          m_sFilename;
    float           m_fSecondsPerFrame;
    float           m_fTimePassedSinceLastFrameUpdate;

//Functions
    void UpdateImage();
    void LoadNextFrame();
public:
    Video();
    explicit Video(const string& filename = "");
    bool LoadFromFile(const string& filename);
    void Update(float time);

    sf::Vector2i Size() const { return sf::Vector2i(GetWidth(), GetHeight()); }

    nuint GetWidth() const { return m_pCodecCtx->width; }
    nuint GetHeight() const { return m_pCodecCtx->height; }

    float GetFrameRate() const { return 1/m_fSecondsPerFrame; }

    operator const sf::Texture&() const {return m_Texture;}

    ~Video();

    sf::Color GetPixel(nuint x, nuint y) const;

    void CloseVideo();

private:
    Video(const Video& rhs);
    Video& operator=(const Video& rhs);
};

#endif