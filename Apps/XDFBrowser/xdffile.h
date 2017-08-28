/********************************************************************

C++ XDF File implementation. See https://code.google.com/p/xdf/wiki/Specifications for XDF file specification.

This particular implementation is focused on low level operations with respect to the file. The individual chunks 
are preserved. Headers can be inspected and edited. This implementation defaults to require only 
50 MB of memory, regardless of file size.

Written by Matthew Grivich, Swartz Center for Computation Neuroscience, Summer 2014.

*********************************************************************/

#ifndef XDFFILE_H
#define XDFFILE_H

#include <iostream>
#include <vector>
#include <iostream>
#include <memory>
#include <exception>

#include <QMessageBox>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include <QProgressBar>


class IOException : public std::exception {

	public:
        const char * description;
        IOException(const char * description): description(description) {}
	virtual const char* what() const throw()
	{
		return description;
	}
};


class XDFChunk
{
public:

	static const quint16 FILEHEADER_CHUNK = 1;
	static const quint16 STREAMHEADER_CHUNK = 2;
	static const quint16 SAMPLES_CHUNK = 3;
	static const quint16 CLOCKOFFSET_CHUNK = 4;
	static const quint16 BOUNDARY_CHUNK = 5;
	static const quint16 STREAMFOOTER_CHUNK = 6;
	quint64 thisChunkPosition; //in file

protected:
	qint8 thisNumLengthBytes;
	quint64 thisChunkLength;

protected:
	QString readString(QDataStream &in, size_t length);

public:

	XDFChunk(quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) 
		: thisChunkPosition(chunkPosition), thisNumLengthBytes(numLengthBytes), thisChunkLength(chunkLength) {}

	virtual QString getName()=0;

	virtual QString getText()=0;

	virtual void setText(const QString& text)=0;

	virtual void writeChunk(QDataStream &out)=0;

	virtual ~XDFChunk(){};
};

class FileHeaderChunk : public XDFChunk
{
public:
	QString chunkData;

	FileHeaderChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength);
	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~FileHeaderChunk();
};

class StreamHeaderChunk : public XDFChunk
{
public:
	QString chunkData;
	qint32 streamID;

	StreamHeaderChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength);
	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~StreamHeaderChunk();
};


class SamplesChunk : public XDFChunk
{
public:
	bool loaded;
	char * chunkData;

	SamplesChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength, bool loadData);

	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	quint64 loadData(QFile &file);

	quint64 unloadData();

	~SamplesChunk();
};

class ClockOffsetChunk : public XDFChunk
{
public:
	qint32 streamID;
	double collectionTime;
	double offsetValue;

	ClockOffsetChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength);

	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~ClockOffsetChunk();
};

class BoundaryChunk : public XDFChunk
{
public:
	quint8 * chunkData;

	BoundaryChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength);

	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~BoundaryChunk();
};

class StreamFooterChunk : public XDFChunk
{
public:
	QString chunkData;
	qint32 streamID;

	StreamFooterChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength);

	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~StreamFooterChunk();
};

class UnrecognizedChunk : public XDFChunk
{
public:
	char * chunkData;
	quint16 thisChunkTag;

	UnrecognizedChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength, quint16 chunkTag);

	QString getName();

	QString getText();

	void setText(const QString& text);

	void writeChunk(QDataStream &out);

	~UnrecognizedChunk();
};
 
class XDFfile
{
private:
	static const quint64 DEFAULT_MAX_BYTES = 52428800; //50 megabytes. If the file is larger than this, it will be partially loaded, and new chunks will be loaded as requested (with old chunks unloaded to make room).
    QFile file;
	quint64 bytesLoaded; //total number of bytes loaded.
	quint64 maxBytes; //Maximum number of bytes to load at once.
    //TODO: unique_ptr?
    std::vector<std::shared_ptr<XDFChunk> > chunks; //vector containing file data, one element for each chunk.
	std::list<int> loadedChunks; //A list of all chunks that are currently loaded

public:

    XDFfile(const QString& fileName): file(fileName), bytesLoaded(0), maxBytes(DEFAULT_MAX_BYTES){}

	~XDFfile() {
		file.close();	
	};

    void open(QProgressBar *progressBar);

	void save(QProgressBar *progressBar);

	void saveAs(const QString& fileName, QProgressBar *progressBar);

	void close();

	QString getFileName();

	int getNChunks();

	QString getChunkName(int element);

	QString getChunkText(int element);

	void setChunkText(int element, const QString& text);
};

#endif // XDFFILE_H
