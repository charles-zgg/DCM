#include "pch.h"
#include "DcmFile.h"

CDcmFile::CDcmFile(CWnd* _pwnd)
	:m_pShowImgWnd(_pwnd)
	,m_DcmFP(nullptr)
	, m_pBitmapInfo(nullptr)
	, m_pRGBdata(nullptr)
	, m_dcmFilePath("")
	, m_isLitteEndian(true)
	, m_isExplicitVR(true)
{
}
 
CDcmFile::CDcmFile(std::string _filePath, CWnd* _pwnd)
	: m_pShowImgWnd(_pwnd)
	, m_DcmFP(nullptr)
	, m_pBitmapInfo(nullptr)
	, m_pRGBdata(nullptr)
	, m_dcmFilePath(_filePath)
	, m_isLitteEndian(true)
	, m_isExplicitVR(true)
{
	OpenDcmFile();
}

CDcmFile::~CDcmFile()
{
	reset();
}

void CDcmFile::reset()
{
	if (m_DcmFP != nullptr)
	{
		fclose(m_DcmFP);
		m_DcmFP = nullptr;
	}

	if (m_MapElement.size() > 0)
	{
		for each (auto var in m_MapElement)
		{
			delete (DcmElementData*)(var.second);
			(DcmElementData*)(var.second) = nullptr;
		}
		m_MapElement.clear();
	}

	if (m_pBitmapInfo != nullptr)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = nullptr;
	}

	if (m_pRGBdata != nullptr)
	{
		delete[] m_pRGBdata;
		m_pRGBdata = nullptr;
	}
}




void CDcmFile::OpenDcmFile(std::string _filePath)
{
	m_dcmFilePath = _filePath;
	OpenDcmFile();
}

void CDcmFile::OpenDcmFile()
{
	reset();

	fopen_s(&m_DcmFP,m_dcmFilePath.c_str(), "rb");
	if (m_DcmFP == NULL)
	{
		std::cout << m_dcmFilePath << "  open failed"<<std::endl;
		return;
	}
	fseek(m_DcmFP, 0, SEEK_END);//把文件内指针移到结尾
	int _FileSzie = ftell(m_DcmFP);//dicom格式文件字节长度

	fseek(m_DcmFP, 128, SEEK_SET);
	char head[4];
	memset(head, 0,4);//数组初始化为0
	if (fread(head, 1, 4, m_DcmFP) != 4)
	{
		fclose(m_DcmFP);
		return;
	}
	//读取文件标识
 	if (memcmp(head, "DICM",4)!=0)
	{
		fclose(m_DcmFP);
		std::cout << m_dcmFilePath << "  is not DICM" << std::endl;
		return;
	}

	//循环读取 DcmElement
	while (_FileSzie> ftell(m_DcmFP) + 6)
	{
		//读取tag
		DcmElementData* _pElementData = new DcmElementData();
		if (fread(&(_pElementData->Tag), 1, sizeof(_pElementData->Tag), m_DcmFP) != sizeof(_pElementData->Tag))
		{
			fclose(m_DcmFP);
			return;
		}


		if (_pElementData->Tag.GroupID == 0x0002)
		{
			//读取VR并判断 和 读取数据长度
			if (fread(_pElementData->_vr, 1, sizeof(_pElementData->_vr), m_DcmFP) != sizeof(_pElementData->_vr))
			{
				fclose(m_DcmFP);
				return;
			}
			if (memcmp(_pElementData->_vr, "OB", 2) == 0  || memcmp(_pElementData->_vr, "OW", 2) == 0 || memcmp(_pElementData->_vr, "OF", 2) == 0
				|| memcmp(_pElementData->_vr, "UT", 2) == 0 || memcmp(_pElementData->_vr, "SQ", 2) == 0 || memcmp(_pElementData->_vr, "UN", 2) == 0)
			{
				fseek(m_DcmFP, 2, SEEK_CUR);
				fread(&_pElementData->datalength,  1, sizeof(_pElementData->datalength), m_DcmFP);
			}
			else
			{
				unsigned short length = 0;
				fread(&length, 1, sizeof(length), m_DcmFP);
				_pElementData->datalength = length;
			}
		}
		else if (_pElementData->Tag.GroupID == 0xfffe)
		{
			if (_pElementData->Tag .ElementID== 0xe000 || _pElementData->Tag.ElementID == 0xe00d || _pElementData->Tag.ElementID == 0xe0dd)
			{
				fread(&_pElementData->datalength, 1, sizeof(_pElementData->datalength), m_DcmFP);
			}
		}
		else if (m_isExplicitVR)
		{
			//读取VR并判断 和 读取数据长度
			if (fread(_pElementData->_vr,  1, sizeof(_pElementData->_vr), m_DcmFP) != sizeof(_pElementData->_vr))
			{
				fclose(m_DcmFP);
				return;
			}
			if (memcmp(_pElementData->_vr, "OB", 2) == 0 || memcmp(_pElementData->_vr, "OW", 2) == 0 || memcmp(_pElementData->_vr, "OF", 2) == 0
				|| memcmp(_pElementData->_vr, "UT", 2) == 0 || memcmp(_pElementData->_vr, "SQ", 2) == 0 || memcmp(_pElementData->_vr, "UN", 2) == 0)
			{
				fseek(m_DcmFP, 2, SEEK_CUR);
				fread(&_pElementData->datalength, 1, sizeof(_pElementData->datalength), m_DcmFP);
			}
			else
			{
				unsigned short length = 0;
				fread(&length, 1, sizeof(length), m_DcmFP);
				_pElementData->datalength = length;
			}
		}
		else
		{
			fread(&_pElementData->datalength, 1, sizeof(_pElementData->datalength), m_DcmFP);
		}


		//读取元素节点的数据
		_pElementData->pData = new char[_pElementData->datalength];
		memset(_pElementData->pData, 0, _pElementData->datalength);
		fread(_pElementData->pData,  1, _pElementData->datalength, m_DcmFP);

		//根据数据判断 VR 字节序
		if (_pElementData->Tag.GroupID == 0x0002 && _pElementData->Tag.ElementID == 0x0010)
		{
			if (memcmp( _pElementData->getString().c_str(),"1.2.840.10008.1.2.1", _pElementData->datalength)==0)
			{
				m_isLitteEndian = true;
				m_isExplicitVR = true;
			}					    
			else if (memcmp(_pElementData->getString().c_str(), "1.2.840.10008.1.2.2", _pElementData->datalength) == 0 )
			{
				m_isLitteEndian = false;
				m_isExplicitVR = true;
			}
			else if (memcmp(_pElementData->getString().c_str(), "1.2.840.10008.1.2", _pElementData->datalength) == 0 )
			{
				m_isLitteEndian = true;
				m_isExplicitVR = false;
			}
			else
			{
				std::cout << _pElementData->getString() << "Not supported at the moment" << std::endl;
				reset();
				return;
			}
		}
		m_MapElement.insert(std::pair<DcmTagKey, DcmElementData*>(_pElementData->Tag, _pElementData));
	}
	fclose(m_DcmFP);
	GetRGB_data();
	ShowImg();
}

int  CDcmFile::Get_Int(DcmTagKey & _tag)
{
	if (m_MapElement.size() > 0)
	{
		std::map<DcmTagKey, DcmElementData*>::iterator  _it=m_MapElement.find(_tag);
		if (_it != m_MapElement.end())
		{
			 return _it->second->getInt();
		}
	}
	return -1;
}

int  CDcmFile::Get_String2Int(DcmTagKey & _tag)
{
	if (m_MapElement.size() > 0)
	{
		std::map<DcmTagKey, DcmElementData*>::iterator  _it = m_MapElement.find(_tag);
		if (_it != m_MapElement.end())
		{
			return _it->second->getString2Int();
		}
	}
	return -1;
}

std::string  CDcmFile::Get_String(DcmTagKey & _tag)
{
	if (m_MapElement.size() > 0)
	{
		std::map<DcmTagKey, DcmElementData*>::iterator  _it = m_MapElement.find(_tag);
		if (_it != m_MapElement.end())
		{
			return _it->second->getString();
		}
	}
	return "";
}

DcmElementData * CDcmFile::Get_DcmElement(DcmTagKey & _tag)
{
	if (m_MapElement.size() > 0)
	{
		std::map<DcmTagKey, DcmElementData*>::iterator  _it = m_MapElement.find(_tag);
		if (_it != m_MapElement.end())
		{
			return _it->second;
		}
	}
	return nullptr;
}

void CDcmFile::GetRGB_data()
{
	m_PatientName = Get_String(DCM_PatientName);
// 	std::string Sex = Get_String(DCM_PatientSex);
	int window_center = Get_String2Int(DCM_WindowCenter);
	int window_width = Get_String2Int(DCM_WindowWidth);
	m_ImgHeight = Get_Int(DCM_Rows);
	m_ImgWidth = Get_Int(DCM_Columns);
	int bitCount = Get_Int(DCM_BitsStored);	//位深
	m_ImgChannle = Get_Int(DCM_SamplesPerPixel);	 //采样点像素.rgb通道 一般为1，灰色图像
	int pixelType = Get_Int(DCM_PixelRepresentation);
	int rescaleIntercept = Get_String2Int(DCM_RescaleIntercept);	//截距
	int rescaleSlope = Get_String2Int(DCM_RescaleSlope);  //斜率

	std::string imgmode = Get_String(DCM_PhotometricInterpretation);
	DcmElementData* _pImgElement = Get_DcmElement(DCM_PixelData);	 //获取图像数据 元素
 
	//开始处理图像
	if (_pImgElement != nullptr)
	{
		int min = window_center - window_width / 2.0 + 0.5;
		int max = window_center + window_width / 2.0 + 0.5;
		double fCtA = (double)256 / window_width;
		double fCtB = 128 - 256 * (double)window_center / window_width;
		if (fCtB < 0)
		{
			fCtB = 0;
		}
		if (fCtB > 255)
		{
			fCtB = 255;
		}
		m_pRGBdata = new unsigned char[m_ImgHeight*m_ImgWidth*m_ImgChannle];
		memset(m_pRGBdata, 0, m_ImgHeight*m_ImgWidth*m_ImgChannle);

		if (m_ImgChannle == 1)
		{
			int _currentPixel = 0;
			for (int index=0;index< _pImgElement->datalength;index+=2)
			{
				unsigned char _data[2] = { 0 };
				unsigned short _uGray = 0;
				short _Gray = 0;
				memcpy_s(_data, 2, _pImgElement->pData + index, 2);
				if (pixelType == 0)
				{
					if (bitCount <= 8)
					{  
						_uGray = m_isLitteEndian ? _data[0] : _data[1];
					}
					else
					{
						long temp = 0;
						if (m_isLitteEndian)
						{
							_uGray = *(unsigned short*)_data;
							temp = _uGray * rescaleSlope + rescaleIntercept;
							temp = temp * fCtA + fCtB;
						}
						else
						{
							_uGray = _data[1] + _data[0] * 256;
						}

 						if (temp > 0xff)
						{
							temp = 0xff;
						}
						else if (temp < 0)
						{
							temp = 0;
						}
						_currentPixel = temp;
					}
				}
				else if (pixelType == 1)
				{
					if (bitCount <= 8)
					{
						_Gray = m_isLitteEndian ? _data[0] : _data[1];
					}
					else
					{
						long temp = 0;
						if (m_isLitteEndian)
						{
							_Gray = *(short*)_data;
							temp = _Gray * rescaleSlope + rescaleIntercept;
							temp = temp * fCtA + fCtB;
						}
						else
						{
							_Gray = _data[1] + _data[0] * 256;
						}
						if (temp > 0xff)
						{
							temp = 0xff;
						}
						else if (temp < 0)
						{
							temp = 0;
						}
						_currentPixel = temp;
					}
				}
				if (imgmode=="MONOCHROME1")
				{
					_currentPixel = 255 - _currentPixel;
				}
				m_pRGBdata[index / 2] = _currentPixel;
			}
 
		}
		else if(m_ImgChannle ==3)
		{
			for (int index = 0; index < _pImgElement->datalength; index += 3)
			{
				char _data[3] = { 0 };
				memcpy_s(_data, 3,_pImgElement->pData + index, 3);
				m_pRGBdata[index / 3 + 0] = _data[2];
				m_pRGBdata[index / 3 + 1] = _data[1];
				m_pRGBdata[index / 3 + 2] = _data[1];
			}
		}
 	}
}

unsigned char * CDcmFile::FlipVerticallyImg()
{
	//图像垂直反转
	unsigned char * _data = new unsigned char[m_ImgHeight*m_ImgWidth * m_ImgChannle];
	for (int i = 0; i < m_ImgWidth; i++)
	{
		for (int j = 0; j < m_ImgHeight; j++)
		{
			_data[i*m_ImgWidth + j] = m_pRGBdata[m_ImgWidth*(m_ImgHeight - 1 - i) + j];
		}
	}
	return _data;
}

void CDcmFile::ShowImg()
{
	//填充信息头
	BITMAPINFOHEADER  BitmapInfoHeader;
	BitmapInfoHeader.biBitCount = 8;
	BitmapInfoHeader.biClrImportant = 0;
	BitmapInfoHeader.biClrUsed = 0;
	BitmapInfoHeader.biCompression = BI_RGB;
	BitmapInfoHeader.biPlanes = 1;
	BitmapInfoHeader.biHeight = m_ImgHeight;
	BitmapInfoHeader.biWidth = m_ImgWidth;
	BitmapInfoHeader.biXPelsPerMeter = 0;
	BitmapInfoHeader.biYPelsPerMeter = 0;
	BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfoHeader.biSizeImage = 0;

	//确定调色板数量
	int szPalette = 0;
	switch (BitmapInfoHeader.biBitCount)
	{
	case 1:
		szPalette = 2;
	case 4:
		szPalette = 16;
	case 8:
		szPalette = 256;
	}

	//填充调色板
	RGBQUAD * pColorTable = new RGBQUAD[szPalette];
	for (int i = 0; i < szPalette; i++)
	{
		pColorTable[i].rgbBlue = i;
		pColorTable[i].rgbGreen = i;
		pColorTable[i].rgbRed = i;
		pColorTable[i].rgbReserved = 0;
	}

	//填充bitmap文件信息
	m_BitmapinfoSize = sizeof(BITMAPINFOHEADER) + szPalette * sizeof(RGBQUAD);
	char *bitmapInfo = new char[m_BitmapinfoSize];
	memcpy(bitmapInfo, &BitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	memcpy(bitmapInfo + sizeof(BITMAPINFOHEADER), pColorTable, szPalette * sizeof(RGBQUAD));
	m_pBitmapInfo = (PBITMAPINFO)bitmapInfo;
	delete[]  pColorTable;

	//绘制图像
	if (m_pShowImgWnd != nullptr)
	{
		m_pShowImgWnd->SetWindowText(m_PatientName.c_str());
		CRect clientrc;
		m_pShowImgWnd->GetClientRect(&clientrc);
		CRect windowrc;
		m_pShowImgWnd->GetWindowRect(&windowrc);
		int borderwidth = (windowrc.right - windowrc.left) - (clientrc.right - clientrc.left);
		int borderheight = (windowrc.bottom - windowrc.top) - (clientrc.bottom - clientrc.top);
 		unsigned char* pdata = FlipVerticallyImg();
 		SetWindowPos(m_pShowImgWnd->m_hWnd, HWND_TOP,0, 0, m_ImgWidth+ borderwidth, m_ImgHeight+ borderheight, SWP_NOMOVE | SWP_SHOWWINDOW);
		m_pShowImgWnd->GetDC()->SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(m_pShowImgWnd->GetDC()->GetSafeHdc(), 0, 0, m_ImgWidth, m_ImgHeight, 0, 0, m_ImgWidth, m_ImgHeight, pdata, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		delete[] pdata;
	}
}

void CDcmFile::Save_bmp(std::string _BmpPath)
{
	//填充文件头信息
	BITMAPFILEHEADER  BitmapFileHeader;
	BitmapFileHeader.bfReserved1 = 0;
	BitmapFileHeader.bfReserved2 = 0;
	BitmapFileHeader.bfSize = 2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_ImgWidth * m_ImgHeight*m_ImgChannle;
	BitmapFileHeader.bfOffBits = 0x36;
	BitmapFileHeader.bfType = 0x4d42;
 
	FILE * _bmpfile;
	fopen_s(&_bmpfile, _BmpPath.c_str(), "wb");
	if (_bmpfile == NULL)
	{
		std::cout << _BmpPath << "  open failed" << std::endl;
		return;
	}
	unsigned char* pdata = FlipVerticallyImg();
	fwrite(&BitmapFileHeader, 1, sizeof(BITMAPFILEHEADER), _bmpfile);
 	fwrite(m_pBitmapInfo, 1, m_BitmapinfoSize, _bmpfile);
 	fwrite(pdata, 1, m_ImgWidth*m_ImgHeight*m_ImgChannle, _bmpfile);
	fclose(_bmpfile);
	delete[] pdata;
 }



void CDcmFile::Save_Jpeg(std::string _JpegPath)
{
 	TooJpeg::writeJpeg(_JpegPath, m_pRGBdata, m_ImgWidth, m_ImgHeight, false, 90);
}


