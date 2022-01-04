/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Utils/CubismString.hpp>

 /**
  * @brief wav�ե�����ϥ�ɥ�
  * @attention 16bit wav �ե������i���z�ߤΤߌgװ�g��
  */
class WavFileHandler
{
public:
    /**
     * @brief ���󥹥ȥ饯��
     */
    WavFileHandler();

    /**
     * @brief �ǥ��ȥ饯��
     */
    ~WavFileHandler();

    /**
     * @brief wav�ե�����ϥ�ɥ���ڲ�״�B����
     *
     * @param[in]   deltaTimeSeconds    �ǥ륿�r�g[��]
     * @retval  true    ���¤���Ƥ���
     * @retval  false   ���¤���Ƥ��ʤ�
     */
    Csm::csmBool Update(Csm::csmFloat32 deltaTimeSeconds);

    /**
     * @brief ������ָ������wav�ե�������i���z�ߤ��_ʼ����
     *
     * @param[in] filePath wav�ե�����Υѥ�
     */
    void Start(const Csm::csmString& filePath);

    /**
     * @brief �F�ڤ�RMS��ȡ��
     *
     * @retval  csmFloat32 RMS��
     */
    Csm::csmFloat32 GetRms() const;

private:
    /**
     * @brief wav�ե�����Υ�`��
     *
     * @param[in] filePath wav�ե�����Υѥ�
     * @retval  true    �i���z�߳ɹ�
     * @retval  false   �i���z��ʧ��
     */
    Csm::csmBool LoadWavFile(const Csm::csmString& filePath);

    /**
     * @brief PCM�ǩ`���ν��
     */
    void ReleasePcmData();

    /**
     * @brief -1��1�ι����1����ץ�ȡ��
     * @retval    csmFloat32    ��Ҏ�����줿����ץ�
     */
    Csm::csmFloat32 GetPcmSample();

    /**
     * @brief �i���z���wavfile�����
     */
    struct WavFileInfo
    {
        /**
         * @brief ���󥹥ȥ饯��
         */
        WavFileInfo() : _fileName(""), _numberOfChannels(0),
            _bitsPerSample(0), _samplingRate(0), _samplesPerChannel(0)
        { }

        Csm::csmString _fileName; ///< �ե�������
        Csm::csmUint32 _numberOfChannels; ///< �����ͥ���
        Csm::csmUint32 _bitsPerSample; ///< ����ץ뤢����ӥå���
        Csm::csmUint32 _samplingRate; ///< ����ץ�󥰥�`��
        Csm::csmUint32 _samplesPerChannel; ///< 1�����ͥ뤢����t����ץ���
    } _wavFileInfo;

    /**
     * @brief �Х��ȥ�`��
     */
    struct ByteReader {
        /**
         * @brief ���󥹥ȥ饯��
         */
        ByteReader() : _fileByte(NULL), _fileSize(0), _readOffset(0)
        { }

        /**
         * @brief 8�ӥå��i���z��
         * @return Csm::csmUint8 �i��ȡ�ä�8�ӥåȂ�
         */
        Csm::csmUint8 Get8()
        {
            const Csm::csmUint8 ret = _fileByte[_readOffset];
            _readOffset++;
            return ret;
        }

        /**
         * @brief 16�ӥå��i���z�ߣ���ȥ륨��ǥ�����
         * @return Csm::csmUint16 �i��ȡ�ä�16�ӥåȂ�
         */
        Csm::csmUint16 Get16LittleEndian()
        {
            const Csm::csmUint16 ret = (_fileByte[_readOffset + 1] << 8) | _fileByte[_readOffset];
            _readOffset += 2;
            return ret;
        }

        /**
         * @brief 24�ӥå��i���z�ߣ���ȥ륨��ǥ�����
         * @return Csm::csmUint32 �i��ȡ�ä�24�ӥåȂ�����λ24�ӥåȤ��O����
         */
        Csm::csmUint32 Get24LittleEndian()
        {
            const Csm::csmUint32 ret =
                (_fileByte[_readOffset + 2] << 16) | (_fileByte[_readOffset + 1] << 8)
                | _fileByte[_readOffset];
            _readOffset += 3;
            return ret;
        }

        /**
         * @brief 32�ӥå��i���z�ߣ���ȥ륨��ǥ�����
         * @return Csm::csmUint32 �i��ȡ�ä�32�ӥåȂ�
         */
        Csm::csmUint32 Get32LittleEndian()
        {
            const Csm::csmUint32 ret =
                (_fileByte[_readOffset + 3] << 24) | (_fileByte[_readOffset + 2] << 16)
                | (_fileByte[_readOffset + 1] << 8) | _fileByte[_readOffset];
            _readOffset += 4;
            return ret;
        }

        /**
         * @brief �����ͥ����ȡ�äȲ��������ФȤ�һ�¥����å�
         * @param[in] reference �ʖˌ���Υ����ͥ���������
         * @retval  true    һ�¤��Ƥ���
         * @retval  false   һ�¤��Ƥ��ʤ�
         */
        Csm::csmBool GetCheckSignature(const Csm::csmString& reference)
        {
            Csm::csmChar getSignature[4] = { 0, 0, 0, 0 };
            const Csm::csmChar* referenceString = reference.GetRawString();
            if (reference.GetLength() != 4)
            {
                return false;
            }
            for (Csm::csmUint32 signatureOffset = 0; signatureOffset < 4; signatureOffset++)
            {
                getSignature[signatureOffset] = static_cast<Csm::csmChar>(Get8());
            }
            return (getSignature[0] == referenceString[0]) && (getSignature[1] == referenceString[1])
                && (getSignature[2] == referenceString[2]) && (getSignature[3] == referenceString[3]);
        }

        Csm::csmByte* _fileByte; ///< ��`�ɤ����ե�����ΥХ�����
        Csm::csmSizeInt _fileSize; ///< �ե����륵����
        Csm::csmUint32 _readOffset; ///< �ե��������λ��
    } _byteReader;

    Csm::csmFloat32** _pcmData; ///< -1����1�ι���Ǳ�F���줿�����ǩ`������
    Csm::csmUint32 _sampleOffset; ///< ����ץ����λ��
    Csm::csmFloat32 _lastRms; ///< �����Ӌ�y����RMS��
    Csm::csmFloat32 _userTimeSeconds; ///< �ǥ륿�r�g�ηe�ゎ[��]
 };
