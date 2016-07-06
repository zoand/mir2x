/*
 * =====================================================================================
 *
 *       Filename: editormap.hpp
 *        Created: 02/08/2016 22:17:08
 *  Last Modified: 07/06/2016 09:21:13
 *
 *    Description: EditorMap has no idea of ImageDB, WilImagePackage, etc..
 *                 Use function handler to handle draw, cache, etc
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */
#pragma once

#include "mir2map.hpp"
#include "mir2xmap.hpp"

#include <string>
#include "wilimagepackage.hpp"
#include <cstdint>
#include <functional>
#include <vector>
#include <utility>

// TODO I defined an OBJGRID_NA state here, but seems I can't solve the ``tre problem"
// to avoid some null drawing operation, for the object
//      texture for trees, old logic try to 
//
enum ObjectGridType: int{
    OBJGRID_NA          = 0,
    OBJGRID_GROUND      = 1,
    OGJGRID_OBERGROUND  = 2,
    OBJGRID_ROOF        = 3,
};

class EditorMap
{
    private:
        int             m_W;
        int             m_H;
        bool            m_Valid;
        uint32_t        m_dwAniSaveTime[8];
        uint8_t         m_bAniTileFrame[8][16];

    private:
        Mir2Map        *m_OldMir2Map;
        Mir2xMap       *m_Mir2xMap;

    private:
        // buffers
        std::vector<std::vector<int>>                             m_BufLightMark;
        std::vector<std::vector<uint16_t>>                        m_BufLight;

        std::vector<std::vector<int>>                             m_BufTileMark;
        std::vector<std::vector<uint32_t>>                        m_BufTile;

        std::vector<std::vector<std::array<int, 2>>>              m_BufObjMark;
        std::vector<std::vector<std::array<int, 2>>>              m_BufGroundObjMark;
        std::vector<std::vector<std::array<int, 2>>>              m_BufAlphaObjMark;
        std::vector<std::vector<std::array<int, 2>>>              m_BufAniObjMark;
        std::vector<std::vector<std::array<uint32_t, 2>>>         m_BufObj;
        std::vector<std::vector<std::array<std::vector<int>, 2>>> m_BufObjGridTag; // this is the new attribute I introduced

        std::vector<std::vector<std::array<uint8_t, 4>>>          m_BufGround;
        std::vector<std::vector<std::array<int, 4>>>              m_BufGroundMark;
        std::vector<std::vector<std::array<int, 4>>>              m_BufGroundSelectMark;

    private:
        // for ground select
        std::vector<std::pair<int, int>> m_SelectPointV;

    public:
        EditorMap();
        ~EditorMap();

    public:
        bool LoadMir2Map(const char *);
        bool LoadMir2xMap(const char *);

    public:
        // fast api
        // user's responsability to maintain parameters and states
        bool Valid()
        {
            return m_Valid;
        }

        bool ValidC(int nX, int nY)
        {
            return nX >= 0 && nX < W() && nY >= 0 && nY < H();
        }

        bool ValidP(int nX, int nY)
        {
            return nX >= 0 && nX < 48 * W() && nY >= 0 && nY < 32 * H();
        }

        int W()
        {
            return m_W;
        }

        int H()
        {
            return m_H;
        }

        int TileValid(int nX, int nY)
        {
            return m_BufTileMark[nX / 2][nY / 2];
        }

        uint32_t Tile(int nX, int nY)
        {
            return m_BufTile[nX / 2][nY / 2];
        }

        int ObjectValid(int nX, int nY, int nIndex)
        {
            return m_BufObjMark[nX][nY][nIndex];
        }

        uint32_t Object(int nX, int nY, int nIndex)
        {
            return m_BufObj[nX][nY][nIndex];
        }

        int GroundObjectValid(int nX, int nY,  int nIndex)
        {
            return m_BufGroundObjMark[nX][nY][nIndex];
        }

        int AlphaObjectValid(int nX, int nY,  int nIndex)
        {
            return m_BufAlphaObjMark[nX][nY][nIndex];
        }

        int GroundSelect(int nX, int nY, int nIndex)
        {
            return m_BufGroundSelectMark[nX][nY][nIndex];
        }

        bool AniObjectValid(int nX, int nY, int nIndex)
        {
            return m_BufAniObjMark[nX][nY][nIndex];
        }

        int LightValid(int nX, int nY)
        {
            return m_BufLightMark[nX][nY];
        }

        uint16_t Light(int nX, int nY)
        {
            return m_BufLight[nX][nY];
        }

        int GroundValid(int nX, int nY, int nIndex)
        {
            return m_BufGroundMark[nX][nY][nIndex];
        }

        uint8_t Ground(int nX, int nY, int nIndex)
        {
            return m_BufGround[nX][nY][nIndex];
        }

        uint16_t ObjectOff(int nAniType, int nAniCnt)
        {
            return (uint16_t)(m_bAniTileFrame[nAniType][nAniCnt]);
        }

        void SetGround(int nX, int nY, int nIndex, bool bValid, uint8_t nDesc)
        {
            m_BufGroundMark[nX][nY][nIndex] = ((bValid) ? 1 : 0);
            m_BufGround[nX][nY][nIndex]     = nDesc;
        }

        void SetObject(int nX, int nY, int nIndex, bool bValid, uint32_t nDesc)
        {
            m_BufObjMark[nX][nY][nIndex] = ((bValid) ? 1 : 0);
            m_BufObj[nX][nY][nIndex]     = nDesc;
        }

        void SetGroundObject(int nX, int nY, int nIndex, int nGroundObj)
        {
            m_BufGroundObjMark[nX][nY][nIndex] = nGroundObj;
        }

    public:
        // for map resource extraction
        void ExtractOneTile(int, int, std::function<void(uint8_t, uint16_t)>);
        void ExtractTile(std::function<void(uint8_t, uint16_t)>);

        void ExtractOneObject(int, int, int, std::function<void(uint8_t, uint16_t, uint32_t)>);
        void ExtractObject(std::function<void(uint8_t, uint16_t, uint32_t)>);

    public:
        // draw map
        // external class will provide handlers for physical draw function
        void DrawTile(int, int, int, int, std::function<void(uint8_t, uint16_t, int, int)>);
        void DrawObject(int, int, int, int, bool, std::function<void(uint8_t, uint16_t, int, int)>, std::function<void(int, int)>);
        void DrawLight(int, int, int, int, std::function<void(int, int)>);
        void DrawSelectGround(int, int, int, int, std::function<void(int, int, int)>);
        void DrawSelectPoint(std::function<void(const std::vector<std::pair<int, int>> &)>);

    public:
        // selection operation
        void AddSelectPoint(int, int);
        void SetGroundSelect(int, int, int, int);
        void ClearGroundSelect();

    public:
        void CompressLight(std::vector<bool> &, std::vector<uint8_t> &);
        void CompressGround(std::vector<bool> &, std::vector<uint8_t> &);
        void CompressTile(std::vector<bool> &, std::vector<uint8_t> &);
        void CompressObject(std::vector<bool> &, std::vector<uint8_t> &, int);

    public:
        void DoCompressGround(int, int, int, std::vector<bool> &, std::vector<uint8_t> &);
        void DoCompressLight(int, int, int, std::vector<bool> &, std::vector<uint8_t> &);
        void DoCompressTile(int, int, int, std::vector<bool> &, std::vector<uint8_t> &);
        void DoCompressObject(int, int, int, int, std::vector<bool> &, std::vector<uint8_t> &);

    public:
        void RecordGround(std::vector<uint8_t> &, int, int, int);
        void RecordLight(std::vector<uint8_t> &, int, int);
        void RecordObject(std::vector<bool> &, std::vector<uint8_t> &, int, int, int);
        void RecordTile(std::vector<uint8_t> &, int, int);

    public:
        void UpdateFrame(int);
        bool Resize(int, int, int, int, int, int, int, int);

    public:
        int ObjectBlockType(int, int, int, int);
        int GroundBlockType(int, int, int, int);
        int LightBlockType(int, int, int);
        int TileBlockType(int, int, int);

    public:
        // save to mir2x compact format
        bool Save(const char *);

    private:
        void PushData(const std::vector<bool> &, const std::vector<uint8_t> &, std::vector<uint8_t> &);
        void PushBit(const std::vector<bool> &, std::vector<uint8_t> &);

    public:
        void Optimize();
        void OptimizeTile(int, int);
        void OptimizeCell(int, int);

    private:
        void ClearBuf();
        void MakeBuf(int, int);
        bool InitBuf();

        void SetBufTile(int, int);
        void SetBufLight(int, int);
        void SetBufObj(int, int, int);
        void SetBufGround(int, int, int);

    public:
        std::string MapInfo();

    public:
        bool LocateObject(int, int, int *, int *, int *, int, int);
};
