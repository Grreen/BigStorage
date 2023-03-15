#ifndef CBIGDATA_H
#define CBIGDATA_H

#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <iostream>

namespace BigStorage
{
#define WHOLEPART 12
#define FRACTIONALPART 6
#define DOUBLELENGTH (FRACTIONALPART + 1 + WHOLEPART)
#define INTLENGTH WHOLEPART

	class FStream : public std::wfstream
	{
	public:
		FStream(const std::wstring& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out)
		    : std::wfstream(_Str, _Mode), m_wsFileName(_Str) {}

		static bool Valid(const FStream* pFStream)
		{
			return (NULL != pFStream && pFStream->is_open() && !pFStream->fail());
		}

		static FStream* CreateFile()
		{
			srand(time(0));
			const std::wstring wsFileName(std::to_wstring(rand()));
			FStream *pFStream = new FStream(wsFileName, std::ios::out | std::ios::in | std::ios::trunc);

			if (NULL == pFStream)
				return NULL;

			if (!pFStream->is_open())
			{
				delete pFStream;
				return NULL;
			}

			return pFStream;
		}

		static void RemoveFile(FStream* pFStream)
		{
			if (NULL != pFStream)
			{
				pFStream->close();

				if (!pFStream->m_wsFileName.empty())
					_wremove(pFStream->m_wsFileName.c_str());

				delete pFStream;
				pFStream = NULL;
			}
		}

	private:
		std::wstring m_wsFileName;
	};

	template <typename ValueType>
	class CBigValue
	{
	public:
		CBigValue(FStream* pFStream, unsigned int unIndex)
		    : m_pFStream(pFStream), m_unIndex(unIndex)
		{}

		CBigValue& operator=(const ValueType& oValue)
		{
			if (!FStream::Valid(m_pFStream))
				return *this;

			std::wstringstream wsValue;
			unsigned int unLength;

			wsValue << std::fixed;

			if (std::is_floating_point_v<ValueType>)
			{
				unLength = DOUBLELENGTH;
				wsValue << std::setprecision(FRACTIONALPART);
			}
			else if (std::is_null_pointer_v<ValueType>)
				unLength = INTLENGTH;

			wsValue << std::setw(unLength) << std::setfill(L'0') << oValue;

			m_pFStream->seekp(m_unIndex * unLength);
			m_pFStream->write((wchar_t*)wsValue.str().c_str(), unLength);

			return *this;
		}

		operator ValueType()
		{
			if (!FStream::Valid(m_pFStream))
				return 0.0;

			unsigned int unLength = 0;

			if (std::is_floating_point_v<ValueType>)
				unLength = DOUBLELENGTH;
			else if (std::is_null_pointer_v<ValueType>)
				unLength = INTLENGTH;

			wchar_t *arValue = new wchar_t[unLength];

			m_pFStream->seekp(m_unIndex * unLength);
			m_pFStream->read(arValue, unLength);

			ValueType oTempValue;

			if (std::is_floating_point_v<ValueType>)
				oTempValue = std::stod(arValue);
			else if (std::is_null_pointer_v<ValueType>)
				oTempValue = std::stoi(arValue);

			delete [] arValue;

			return oTempValue;
		}
	private:
		FStream     *m_pFStream;
		unsigned int m_unIndex;
	};

	template <typename ValueType>
	class CArray
	{
	public:
		CArray(unsigned int unSize)
		    : m_pFStream(FStream::CreateFile()), m_bExternalStream(false), m_unSize(unSize), m_unIndex(0)
		{
			if (FStream::Valid(m_pFStream))
			{
				if (std::is_floating_point_v<ValueType>)
					*m_pFStream << std::wstring(DOUBLELENGTH * m_unSize, L'0');
				else if (std::is_null_pointer_v<ValueType>)
					*m_pFStream << std::wstring(INTLENGTH * m_unSize, L'0');
			}
		}

		CArray(FStream* pFStream, unsigned int unSize, unsigned int unIndex)
		    : m_pFStream(pFStream), m_bExternalStream(true), m_unSize(unSize), m_unIndex(unIndex)
		{}

		~CArray()
		{
			if (!m_bExternalStream)
				FStream::RemoveFile(m_pFStream);
		}

		CBigValue<ValueType> operator[](int nIndex)
		{
			return CBigValue<ValueType>(m_pFStream, m_unIndex * m_unSize + nIndex);
		}
	private:
		FStream     *m_pFStream;
		bool         m_bExternalStream;
		unsigned int m_unSize;
		unsigned int m_unIndex;
	};

	template <typename ValueType>
	class CMatrix
	{
	public:
		CMatrix(unsigned int unWidth, unsigned int unHeight)
		    : m_pFStream(FStream::CreateFile()), m_unWidth(unWidth), m_unHeight(unHeight)
		{
			if (FStream::Valid(m_pFStream))
			{
				if (std::is_floating_point_v<ValueType>)
					*m_pFStream << std::wstring(DOUBLELENGTH * m_unWidth * m_unHeight, L'0');
				else if (std::is_null_pointer_v<ValueType>)
					*m_pFStream << std::wstring(INTLENGTH * m_unWidth * m_unHeight, L'0');
			}
		}
		~CMatrix()
		{
			FStream::RemoveFile(m_pFStream);
		}
		CArray<ValueType> operator[](int nIndex)
		{
			return CArray<ValueType>(m_pFStream, m_unWidth, nIndex);
		}
	private:
		FStream     *m_pFStream;
		unsigned int m_unWidth;
		unsigned int m_unHeight;
	};
}

#endif // CBIGDATA_H
