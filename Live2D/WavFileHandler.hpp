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
  * @brief wavファイルハンドラ
  * @attention 16bit wav ファイル読み込みのみ実装済み
  */
class WavFileHandler
{
public:
    /**
     * @brief コンストラクタ
     */
    WavFileHandler();

    /**
     * @brief デストラクタ
     */
    ~WavFileHandler();

    /**
     * @brief wavファイルハンドラの内部状態更新
     *
     * @param[in]   deltaTimeSeconds    デルタ時間[秒]
     * @retval  true    更新されている
     * @retval  false   更新されていない
     */
    Csm::csmBool Update(Csm::csmFloat32 deltaTimeSeconds);

    /**
     * @brief 引数で指定したwavファイルの読み込みを開始する
     *
     * @param[in] filePath wavファイルのパス
     */
    void Start(const Csm::csmString& filePath);

    /**
     * @brief 現在のRMS値取得
     *
     * @retval  csmFloat32 RMS値
     */
    Csm::csmFloat32 GetRms() const;

private:
    /**
     * @brief wavファイルのロード
     *
     * @param[in] filePath wavファイルのパス
     * @retval  true    読み込み成功
     * @retval  false   読み込み失敗
     */
    Csm::csmBool LoadWavFile(const Csm::csmString& filePath);

    /**
     * @brief PCMデータの解放
     */
    void ReleasePcmData();

    /**
     * @brief -1～1の範囲の1サンプル取得
     * @retval    csmFloat32    正規化されたサンプル
     */
    Csm::csmFloat32 GetPcmSample();

    /**
     * @brief 読み込んだwavfileの情報
     */
    struct WavFileInfo
    {
        /**
         * @brief コンストラクタ
         */
        WavFileInfo() : _fileName(""), _numberOfChannels(0),
            _bitsPerSample(0), _samplingRate(0), _samplesPerChannel(0)
        { }

        Csm::csmString _fileName; ///< ファイル名
        Csm::csmUint32 _numberOfChannels; ///< チャンネル数
        Csm::csmUint32 _bitsPerSample; ///< サンプルあたりビット数
        Csm::csmUint32 _samplingRate; ///< サンプリングレート
        Csm::csmUint32 _samplesPerChannel; ///< 1チャンネルあたり総サンプル数
    } _wavFileInfo;

    /**
     * @brief バイトリーダ
     */
    struct ByteReader {
        /**
         * @brief コンストラクタ
         */
        ByteReader() : _fileByte(NULL), _fileSize(0), _readOffset(0)
        { }

        /**
         * @brief 8ビット読み込み
         * @return Csm::csmUint8 読み取った8ビット値
         */
        Csm::csmUint8 Get8()
        {
            const Csm::csmUint8 ret = _fileByte[_readOffset];
            _readOffset++;
            return ret;
        }

        /**
         * @brief 16ビット読み込み（リトルエンディアン）
         * @return Csm::csmUint16 読み取った16ビット値
         */
        Csm::csmUint16 Get16LittleEndian()
        {
            const Csm::csmUint16 ret = (_fileByte[_readOffset + 1] << 8) | _fileByte[_readOffset];
            _readOffset += 2;
            return ret;
        }

        /**
         * @brief 24ビット読み込み（リトルエンディアン）
         * @return Csm::csmUint32 読み取った24ビット値（下位24ビットに設定）
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
         * @brief 32ビット読み込み（リトルエンディアン）
         * @return Csm::csmUint32 読み取った32ビット値
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
         * @brief シグネチャの取得と参照文字列との一致チェック
         * @param[in] reference 検査対象のシグネチャ文字列
         * @retval  true    一致している
         * @retval  false   一致していない
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

        Csm::csmByte* _fileByte; ///< ロードしたファイルのバイト列
        Csm::csmSizeInt _fileSize; ///< ファイルサイズ
        Csm::csmUint32 _readOffset; ///< ファイル参照位置
    } _byteReader;

    Csm::csmFloat32** _pcmData; ///< -1から1の範囲で表現された音声データ配列
    Csm::csmUint32 _sampleOffset; ///< サンプル参照位置
    Csm::csmFloat32 _lastRms; ///< 最後に計測したRMS値
    Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
 };
