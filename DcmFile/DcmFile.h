#pragma once
#include <iostream>
#include <string>
#include <map>
#include "dcdeftag.h"
#include "./rgb2jpeg/toojpeg.h"

class DcmTagKey
{
public:
	DcmTagKey()
		: GroupID(0)
		, ElementID(0)
	{}
	DcmTagKey(const DcmTagKey& _other)
		: GroupID(_other.GroupID)
		, ElementID(_other.ElementID)
	{}
	DcmTagKey(unsigned short gid, unsigned short eid)
		: GroupID(gid)
		, ElementID(eid)
	{}
	bool operator==(const DcmTagKey& _other)  
	{
		return  _other.GroupID == this->GroupID && _other.ElementID == this->ElementID;
	}
	bool operator < (const DcmTagKey& _other)const 
	{
		return _other.GroupID < this->GroupID || (_other.GroupID == this->GroupID && _other.ElementID < this->ElementID);
	}
	unsigned short GroupID;
	unsigned short ElementID;
};


class  DcmElementData
{
public:
	DcmElementData()
		:pData(nullptr)
		, datalength(-1)
	{
		memset(_vr, 0, 2);
	}
	~DcmElementData() 
	{
		if (pData != nullptr)
			delete pData;
	}
 
	DcmTagKey Tag;
	unsigned int datalength;
	char _vr[2];
	char* pData;
public:
	std::string getString() 
	{
		return std::string(pData, datalength);
	}

	unsigned int getString2Int()
	{
 		return atoi(std::string(pData, datalength).c_str());
 	}

	unsigned int getInt()
	{
		unsigned int res = 0;
		if (datalength == 2)
		{
			res = *((unsigned short*)pData);
		}
		else if (datalength == 4)
		{
			res = *((unsigned int*)pData);
		}
		return res;
	}
};





class CDcmFile
{
public:
	CDcmFile(CWnd* _pwnd = nullptr);
	CDcmFile(std::string _filePath, CWnd* _pwnd=nullptr);
	~CDcmFile();
	void OpenDcmFile(std::string _filePath);
	void OpenDcmFile();
	int Get_Int(DcmTagKey & _tag);
	int Get_String2Int(DcmTagKey & _tag);
	std::string Get_String(DcmTagKey & _tag);
	DcmElementData* Get_DcmElement(DcmTagKey & _tag);

	void Save_bmp(std::string _BmpPath);
	void Save_Jpeg(std::string _JpegPath);

private:
	void reset();
	void ShowImg();
	void GetRGB_data();
	unsigned char* FlipVerticallyImg();

private:
	unsigned char* m_pRGBdata;
	bool m_isLitteEndian = true;//是否小字节序（小端在前 、大端在前）
	bool m_isExplicitVR = true;//有无VR
	int m_ImgHeight;
	int m_ImgWidth;
	int m_ImgChannle;
	std::string m_PatientName;



	PBITMAPINFO m_pBitmapInfo;
	int m_BitmapinfoSize;

	CWnd* m_pShowImgWnd;
	std::string m_dcmFilePath;

	FILE* m_DcmFP;
	std::map<DcmTagKey, DcmElementData*>  m_MapElement;
};

