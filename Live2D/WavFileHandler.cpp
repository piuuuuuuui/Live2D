/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "WavFileHandler.hpp"
#include <cmath>
#include <cstdint>
#include "Utilities.hpp"

WavFileHandler::WavFileHandler()
    : _pcmData(NULL)
    , _userTimeSeconds(0.0f)
    , _lastRms(0.0f)
    , _sampleOffset(0)
{
}

WavFileHandler::~WavFileHandler()
{
    if (_pcmData != NULL)
    {
        ReleasePcmData();
    }
}

Csm::csmBool WavFileHandler::Update(Csm::csmFloat32 deltaTimeSeconds)
{
    Csm::csmUint32 goalOffset;
    Csm::csmFloat32 rms;

    // �ǩ`����`��ǰ/�ե�����ĩβ���_�������Ϥϸ��¤��ʤ�
    if ((_pcmData == NULL)
        || (_sampleOffset >= _wavFileInfo._samplesPerChannel))
    {
        _lastRms = 0.0f;
        return false;
    }

    // �U�^�r�g���״�B�򱣳�
    _userTimeSeconds += deltaTimeSeconds;
    goalOffset = static_cast<Csm::csmUint32>(_userTimeSeconds * _wavFileInfo._samplingRate);
    if (goalOffset > _wavFileInfo._samplesPerChannel)
    {
        goalOffset = _wavFileInfo._samplesPerChannel;
    }

    // RMSӋ�y
    rms = 0.0f;
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
    {
        for (Csm::csmUint32 sampleCount = _sampleOffset; sampleCount < goalOffset; sampleCount++)
        {
            Csm::csmFloat32 pcm = _pcmData[channelCount][sampleCount];
            rms += pcm * pcm;
        }
    }
    rms = sqrt(rms / (_wavFileInfo._numberOfChannels * (goalOffset - _sampleOffset)));

    _lastRms = rms;
    _sampleOffset = goalOffset;
    return true;
}

void WavFileHandler::Start(const Csm::csmString& filePath)
{
    // WAV�ե�����Υ�`��
    if (!LoadWavFile(filePath))
    {
        return;
    }

    // ����ץ����λ�ä���ڻ�
    _sampleOffset = 0;
    _userTimeSeconds = 0.0f;

    // RMS����ꥻ�å�
    _lastRms = 0.0f;
}

Csm::csmFloat32 WavFileHandler::GetRms() const
{
    return _lastRms;
}

Csm::csmBool WavFileHandler::LoadWavFile(const Csm::csmString& filePath)
{
    Csm::csmBool ret;

    // �Ȥ�wav�ե������`�ɜg�ߤʤ���I���_��
    if (_pcmData != NULL)
    {
        ReleasePcmData();
    }

    // �ե������`��
    _byteReader._fileByte = Utilities::LoadFileAsBytes(filePath.GetRawString(), &(_byteReader._fileSize));
    _byteReader._readOffset = 0;

    // �ե������`�ɤ�ʧ�����Ƥ��뤫�����^�Υ����ͥ���"RIFF"�����륵������ʤ����Ϥ�ʧ��
    if ((_byteReader._fileByte == NULL) || (_byteReader._fileSize < 4))
    {
        return false;
    }

    // �ե�������
    _wavFileInfo._fileName = filePath;

    do {
        // �����ͥ��� "RIFF"
        if (!_byteReader.GetCheckSignature("RIFF"))
        {
            ret = false;
            break;
        }
        // �ե����륵����-8���i���w�Ф���
        _byteReader.Get32LittleEndian();
        // �����ͥ��� "WAVE"
        if (!_byteReader.GetCheckSignature("WAVE"))
        {
            ret = false;
            break;
        }
        // �����ͥ��� "fmt "
        if (!_byteReader.GetCheckSignature("fmt "))
        {
            ret = false;
            break;
        }
        // fmt����󥯥�����
        const Csm::csmUint32 fmtChunkSize = _byteReader.Get32LittleEndian();
        // �ե��`�ޥå�ID��1����˥�PCM�������ܤ������ʤ�
        if (_byteReader.Get16LittleEndian() != 1)
        {
            ret = false;
            break;
        }
        // �����ͥ���
        _wavFileInfo._numberOfChannels = _byteReader.Get16LittleEndian();
        // ����ץ�󥰥�`��
        _wavFileInfo._samplingRate = _byteReader.Get32LittleEndian();
        // �ǩ`���ٶ�[byte/sec]���i���w�Ф���
        _byteReader.Get32LittleEndian();
        // �֥�å����������i���w�Ф���
        _byteReader.Get16LittleEndian();
        // ���ӻ��ӥå���
        _wavFileInfo._bitsPerSample = _byteReader.Get16LittleEndian();
        // fmt����󥯤Β������֤��i���w�Ф�
        if (fmtChunkSize > 16)
        {
            _byteReader._readOffset += (fmtChunkSize - 16);
        }
        // "data"����󥯤����F����ޤ��i���w�Ф�
        while (!(_byteReader.GetCheckSignature("data"))
            && (_byteReader._readOffset < _byteReader._fileSize))
        {
            _byteReader._readOffset += _byteReader.Get32LittleEndian();
        }
        // �ե������ڤ�"data"����󥯤����F���ʤ��ä�
        if (_byteReader._readOffset >= _byteReader._fileSize)
        {
            ret = false;
            break;
        }
        // ����ץ���
        {
            const Csm::csmUint32 dataChunkSize = _byteReader.Get32LittleEndian();
            _wavFileInfo._samplesPerChannel = (dataChunkSize * 8) / (_wavFileInfo._bitsPerSample * _wavFileInfo._numberOfChannels);
        }
        // �I��_��
        _pcmData = static_cast<Csm::csmFloat32**>(CSM_MALLOC(sizeof(Csm::csmFloat32*) * _wavFileInfo._numberOfChannels));
        for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
        {
            _pcmData[channelCount] = static_cast<Csm::csmFloat32*>(CSM_MALLOC(sizeof(Csm::csmFloat32) * _wavFileInfo._samplesPerChannel));
        }
        // ���Υǩ`��ȡ��
        for (Csm::csmUint32 sampleCount = 0; sampleCount < _wavFileInfo._samplesPerChannel; sampleCount++)
        {
            for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
            {
                _pcmData[channelCount][sampleCount] = GetPcmSample();
            }
        }

        ret = true;

    }  while (false);

    // �ե������_��
    Utilities::ReleaseBytes(_byteReader._fileByte);
    _byteReader._fileByte = NULL;
    _byteReader._fileSize = 0;

    return ret;
}

Csm::csmFloat32 WavFileHandler::GetPcmSample()
{
    Csm::csmInt32 pcm32;

    // 32�ӥåȷ��˒������Ƥ���-1��1�ι��������
    switch (_wavFileInfo._bitsPerSample)
    {
    case 8:
        pcm32 = static_cast<Csm::csmInt32>(_byteReader.Get8()) - 128;
        pcm32 <<= 24;
        break;
    case 16:
        pcm32 = _byteReader.Get16LittleEndian() << 16;
        break;
    case 24:
        pcm32 = _byteReader.Get24LittleEndian() << 8;
        break;
    default:
        // ���ꤷ�Ƥ��ʤ��ӥåȷ�
        pcm32 = 0;
        break;
    }

    return static_cast<Csm::csmFloat32>(pcm32) / INT32_MAX;
}

void WavFileHandler::ReleasePcmData()
{
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
    {
        CSM_FREE(_pcmData[channelCount]);
    }
    CSM_FREE(_pcmData);
    _pcmData = NULL;
}
