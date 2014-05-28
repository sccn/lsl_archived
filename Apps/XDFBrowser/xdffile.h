#ifndef XDFFILE_H
#define XDFFILE_H

#include<QFile>
#include<iostream>
#include<QDataStream>
#include<vector>
#include <boost/shared_ptr.hpp>
#include <QStringListModel>
#include <QtGui/QProgressBar>
#include <iostream>
#include <QMessageBox>

#include "exception"

class IOException : public std::exception {

	public:
		char * description;
		IOException(char * description) {
			this->description = description;
		}
	virtual const char* what() const throw()
	{
		return description;
	}
};


class StringList : public QStringListModel
{
public:
  void append (const QString& string){
    insertRows(rowCount(), 1);
    setData(index(rowCount()-1), string);
  }
  StringList& operator<<(const QString& string){
    append(string);
    return *this;
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
	QString readString(QDataStream &in, size_t length) {
		char * buf = (char *) malloc(length+1);

		in.readRawData(buf, length);
		buf[length] = 0;
		//printf("%s\n", buf);
		QString str(buf);

		free(buf);
		return str;
	}

public:

	XDFChunk(quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) 
		: thisChunkPosition(chunkPosition), thisNumLengthBytes(numLengthBytes), thisChunkLength(chunkLength) {
		
	}

	virtual QString getName()=0;

	virtual QString getText()=0;

	virtual void setText(const QString& text){QString q = text; /*to suppress warning*/};

	//call XDFChunk::writeChunk in all overriding functions.
	virtual void writeChunk(QDataStream &out){
				quint8 chunkLengthByte = 0;
				quint32 chunkLengthInt = 0;
				out << thisNumLengthBytes;
				switch(thisNumLengthBytes){
					case 1:
						chunkLengthByte = (quint8) thisChunkLength;
						out << chunkLengthByte;
						break;
					case 4:
						chunkLengthInt = (quint32) thisChunkLength;
						out << chunkLengthInt;
						break;
					case 8:				
						out << thisChunkLength;
						break;
					default:
						throw IOException("XDFFile is incorrectly formatted: numLengthBytes is an illegal value.\n"); 
						break;
				}

				
	};

	virtual ~XDFChunk(){};
};

class FileHeaderChunk : public XDFChunk
{
public:
	QString chunkData;

	FileHeaderChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength){
		
		chunkData = readString(in, chunkLength-2);
	}

	QString getName() {
		return QString("File Header");
	}

	QString getText() {
		return chunkData;
	}

	void setText(const QString& text) {
		chunkData = text;
		thisChunkLength = chunkData.size() + 2;
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << FILEHEADER_CHUNK;
		
		out.writeRawData(chunkData.toAscii(), thisChunkLength-2);
	}

	~FileHeaderChunk(){};
};

class StreamHeaderChunk : public XDFChunk
{
public:
	QString chunkData;
	qint32 streamID;

	StreamHeaderChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength){
		in >> streamID;
		chunkData = readString(in, chunkLength-6);
	}

	QString getName() {
		char str[256];
		sprintf(str,"Stream Header: %d", streamID);
		return QString(str);
	}

	QString getText() {
		return chunkData;
	}

	void setText(const QString& text) {
		chunkData = text;
		thisChunkLength = chunkData.size() + 6;
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << STREAMHEADER_CHUNK;
		out << streamID;
out.writeRawData(chunkData.toAscii(), thisChunkLength-6);
	}

	~StreamHeaderChunk(){};
};
/*
class SamplesChunk : public XDFChunk
{
public:
	//char * chunkData;
	qint32 streamID;
	quint8 numSamplesBytes;

	quint8 haveTimeStamp; //0 for false, 8 for true
	qdouble timeStamp; //in seconds

	SamplesChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength), chunkData(NULL){
	//	chunkData =  (char *) malloc(chunkLength);
		in >> streamID;
		in >> numSamplesBytes;

		quint64 numSamples;
		switch(numSamplesBytes){
			case 1:
				quint8 numSamplesByte;
				in >> numSampleByte;
				numSamples = numSamplesByte;
				break;
			case 4:
				quint32 numSamplesInt;
				in >> numSamplesInt;
				numSamples = numSamplesInt;
				break;
			case 8:
				quint64 numSamplesLong;
				in >> numSamplesLong;
				numSamples = numSamplesLong;
				break;
			default:
				throw IOException("XDFFile is incorrectly formatted: numLengthBytes is an illegal value.\n");  
				break;
		}


		in >> haveTimeStamp; 
		in >> timeStamp;
		in.readRawData(chunkData, chunkLength-5);
		
		
	}

	QString getName() {
		return QString("Chunk type not recognized.");
	}

	QString getText() {
		return QString("Chunk type not recognized.");
	}

	~SamplesChunk(){
		if(chunkData) free(chunkData);
		chunkData = NULL;
	};
};
*/

class SamplesChunk : public XDFChunk
{
public:
	bool loaded;
	char * chunkData;

	SamplesChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength, bool loadData) : XDFChunk(chunkPosition, numLengthBytes, chunkLength), chunkData(NULL) {
		if(loadData) {
			chunkData =  (char *) malloc(chunkLength-2);
	
			in.readRawData(chunkData, chunkLength-2);
			loaded = true;
		} else {
			chunkData = NULL;
			in.skipRawData(chunkLength-2);
			loaded = false;
		}
		
	}

	QString getName() {
		return QString("Samples Chunk");
	}

	QString getText() {
		return QString("Binary Data");
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << SAMPLES_CHUNK;
		out.writeRawData(chunkData, thisChunkLength -2);

	}

	quint64 loadData(QFile &file) {
		file.seek(thisChunkPosition);
		QDataStream in(&file);
		in.setByteOrder(QDataStream::LittleEndian);
		qint8 numLengthBytes;
		in >> numLengthBytes;
//		char buff[256];
		//std::cout << "numLengthBytes: " << _itoa(numLengthBytes, buff, 10) << "\n";
		thisChunkLength;
		switch(numLengthBytes){
			case 1:
				quint8 chunkLengthByte;
				in >> chunkLengthByte;
				thisChunkLength = chunkLengthByte;
				break;
			case 4:
				quint32 chunkLengthInt;
				in >> chunkLengthInt;
				thisChunkLength = chunkLengthInt;
				break;
			case 8:
				quint64 chunkLengthLong;
				in >> chunkLengthLong;
				thisChunkLength = chunkLengthLong;
				break;
			default:
				throw IOException("XDFFile is incorrectly formatted: numLengthBytes is an illegal value.\n");
				break;
		}
	//	printf("chunkLength: %ld\n", chunkLength);
		quint16 tagNumber;
		in >> tagNumber;

		if(chunkData) free(chunkData);
		chunkData =  (char *) malloc(thisChunkLength-2);
	
		in.readRawData(chunkData, thisChunkLength-2);
		loaded = true;
		return thisChunkLength;

	}

	quint64 unloadData() {
		if(chunkData) free(chunkData);
		chunkData = NULL;
		loaded = false;
		return thisChunkLength;
	}

	~SamplesChunk(){
		if(chunkData) free(chunkData);
		chunkData = NULL;
	};
};

class ClockOffsetChunk : public XDFChunk
{
public:
	char * chunkData;

	ClockOffsetChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength), chunkData(NULL){
		chunkData =  (char *) malloc(chunkLength-2);

		in.readRawData(chunkData, chunkLength-2);
		
	}

	QString getName() {
		return QString("Clock Offset Chunk");
	}

	QString getText() {
		return QString("Binary Data");
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << CLOCKOFFSET_CHUNK;
		out.writeRawData(chunkData, thisChunkLength -2);
	}

	~ClockOffsetChunk(){
		if(chunkData) free(chunkData);
		chunkData = NULL;
	};
};

class BoundaryChunk : public XDFChunk
{
public:
	char * chunkData;

	BoundaryChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength), chunkData(NULL) {
		chunkData =  (char *) malloc(chunkLength-2);

		in.readRawData(chunkData, chunkLength-2);	
	}

	QString getName() {
		return QString("Boundary Chunk");
	}

	QString getText() {
		return QString("Binary Data");
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << BOUNDARY_CHUNK;
		out.writeRawData(chunkData, thisChunkLength-2);
	}

	~BoundaryChunk(){
		if(chunkData) free(chunkData);
		chunkData = NULL;
	};
};

class StreamFooterChunk : public XDFChunk
{
public:
	QString chunkData;
	qint32 streamID;

	StreamFooterChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength) : XDFChunk(chunkPosition, numLengthBytes, chunkLength){
		in >> streamID;
		chunkData = readString(in, chunkLength-6);
	}

	QString getName() {
		char str[256];
		sprintf(str,"Stream Footer: %d", streamID);
		return QString(str);
	}

	QString getText() {
		return chunkData;
	}

	void setText(const QString& text) {
		chunkData = text;
		thisChunkLength = chunkData.size() + 6;
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << STREAMFOOTER_CHUNK;
		out << streamID;
		out.writeRawData(chunkData.toAscii(), thisChunkLength-6);
	}

	~StreamFooterChunk(){};
};

class UnrecognizedChunk : public XDFChunk
{
public:
	char * chunkData;
	quint16 thisChunkTag;

	UnrecognizedChunk(QDataStream &in, quint64 chunkPosition, qint8 numLengthBytes, quint64 chunkLength, quint16 chunkTag) : XDFChunk(chunkPosition, numLengthBytes, chunkLength), chunkData(NULL){
		chunkData =  (char *) malloc(chunkLength-2);
		thisChunkTag = chunkTag;

		in.readRawData(chunkData, chunkLength-2);
		
		
	}

	QString getName() {
		return QString("Chunk type not recognized.");
	}

	QString getText() {
		return QString("Chunk type not recognized.");
	}

	void writeChunk(QDataStream &out) {
		XDFChunk::writeChunk(out);
		out << thisChunkTag;
		out.writeRawData(chunkData, thisChunkLength-2);
	}

	~UnrecognizedChunk(){
		if(chunkData) free(chunkData);
		chunkData = NULL;
	};
};
 
class XDFfile
{
private:
	static const quint64 DEFAULT_MAX_BYTES = 52428800; //50 megabytes
    QFile file;
	quint64 bytesLoaded;
	quint64 maxBytes;
	std::vector<boost::shared_ptr<XDFChunk> > chunks;
	std::list<int> loadedChunks; //currently only samples chunks can be optionally loaded.

public:

    XDFfile(const QString& fileName): file(fileName), bytesLoaded(0), maxBytes(DEFAULT_MAX_BYTES){
		
    }

	QString getFileName() {
		return file.fileName();
	}

	int getNChunks() {
		return chunks.size();
	}

	QString getChunkName(int element) {
		return chunks[element]->getName();
	}

	QString getChunkText(int element) {
		return chunks[element]->getText();
	}

	void setChunkText(int element, const QString& text) {
		chunks[element]->setText(text);
	}

	~XDFfile() {
		file.close();	
	};


	void close() {

	}

	//saves as .copy, then deletes old, renames .copy to replace old.
	void save(QProgressBar *progressBar) {
		QString currentFileName = this->file.fileName();
		QString tempFileName = currentFileName + QString(".copy");
		saveAs(tempFileName, progressBar);
		this->file.close();
		QFile currentFile(currentFileName);
		currentFile.close();
		currentFile.remove();
		QFile tempFile(tempFileName);
		tempFile.rename(currentFileName);
		this->file.setFileName(currentFileName);
		if(!this->file.open(QIODevice::ReadOnly)) throw IOException("Unable to reopen file for reading.\n");
	

	}

	void saveAs(const QString& fileName, QProgressBar *progressBar) {
		if(fileName == this->file.fileName()) {
			save(progressBar);
			return;
		}
		QFile fileToWrite(fileName);
		if(!fileToWrite.open(QIODevice::WriteOnly)) throw IOException("Unable to open file for writing\n");
		QDataStream out(&fileToWrite);
		out.setByteOrder(QDataStream::LittleEndian);
		out.writeRawData("XDF:", 4);
		for(quint32 i=0; i<chunks.size(); i++) {
			
			SamplesChunk *sc = dynamic_cast<SamplesChunk*>(chunks[i].get());
			if(sc && !sc->loaded) {
				bytesLoaded += sc->loadData(file);
				loadedChunks.push_back(i);
				while(loadedChunks.size() > 1 && bytesLoaded > maxBytes) {
					int front = loadedChunks.front();
					loadedChunks.pop_front();
					SamplesChunk *sc_cur = dynamic_cast<SamplesChunk*>(chunks[front].get());
					bytesLoaded -= sc_cur->unloadData();
				}
			} 
			chunks[i]->thisChunkPosition = fileToWrite.pos();
			chunks[i]->writeChunk(out);
			
			//update progress bar
			QMetaObject::invokeMethod(progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, (i*100)/chunks.size()));     

		}
		QMetaObject::invokeMethod(progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));     

		this->file.close();
		this->file.setFileName(fileName);
		if(!this->file.open(QIODevice::ReadOnly)) throw IOException("Unable to open new file for reading.\n");
	}

    void open(QProgressBar *progressBar)  {
		
		if(!file.open(QIODevice::ReadOnly)) throw IOException("Unable to open file.\n");		

		qint64 fileSize = file.size();
		QDataStream in(&file);
		in.setByteOrder(QDataStream::LittleEndian);
		char buf[5];
		in.readRawData(buf, 4);
		buf[4] = 0;
	//	printf("buf: %s\n", buf);
		//std::cout << str.toStdString()<< "\n";
		if(QString(buf).compare(QString("XDF:"))){
			//printf("string: %s\n", str.toStdString().c_str());
			throw IOException("File is not an XDF file\n");
			return; 
		} else {
			int count = 0;
			while(true) {
				count++;
				quint64 chunkPosition = file.pos();
			//	printf("\nchunkPosition: %d\n", chunkPosition);
				if(chunkPosition >= (quint64) file.size()) break;

				qint8 numLengthBytes;
				in >> numLengthBytes;
		//		char buff[256];
				//std::cout << "numLengthBytes: " << _itoa(numLengthBytes, buff, 10) << "\n";
				quint64 chunkLength;
				switch(numLengthBytes){
					case 1:
						quint8 chunkLengthByte;
						in >> chunkLengthByte;
						chunkLength = chunkLengthByte;
						break;
					case 4:
						quint32 chunkLengthInt;
						in >> chunkLengthInt;
						chunkLength = chunkLengthInt;
						break;
					case 8:
						quint64 chunkLengthLong;
						in >> chunkLengthLong;
						chunkLength = chunkLengthLong;
						break;
					default:
						throw IOException("XDFFile is incorrectly formatted: numLengthBytes is an illegal value.\n");
						break;
				}
			//	printf("chunkLength: %ld\n", chunkLength);
				quint16 tagNumber;
				in >> tagNumber;
				//printf("tagNumber: %s\n\n", _itoa(tagNumber, buff, 10));

				switch(tagNumber){
					case XDFChunk::FILEHEADER_CHUNK: {
						boost::shared_ptr<FileHeaderChunk> chunk(new FileHeaderChunk(in, chunkPosition, numLengthBytes, chunkLength));
						chunks.push_back(chunk);
						bytesLoaded += chunkLength;
						break;
						}
					case XDFChunk::STREAMHEADER_CHUNK: {
						boost::shared_ptr<StreamHeaderChunk> chunk(new StreamHeaderChunk(in, chunkPosition, numLengthBytes, chunkLength));
						chunks.push_back(chunk);
						bytesLoaded += chunkLength;
						break;
					}
					case XDFChunk::SAMPLES_CHUNK: {
						bool loadData = bytesLoaded < maxBytes;					
						boost::shared_ptr<SamplesChunk> chunk(new SamplesChunk(in, chunkPosition, numLengthBytes, chunkLength, loadData));
						chunks.push_back(chunk);
						if(loadData) {
							bytesLoaded += chunkLength;
							loadedChunks.push_back(chunks.size()-1);
						}
						break;
					}
					case XDFChunk::CLOCKOFFSET_CHUNK: {
						boost::shared_ptr<ClockOffsetChunk> chunk(new ClockOffsetChunk(in, chunkPosition, numLengthBytes, chunkLength));
						chunks.push_back(chunk);
						bytesLoaded+= chunkLength;
						break;
					}
					case XDFChunk::BOUNDARY_CHUNK: {
						boost::shared_ptr<BoundaryChunk> chunk(new BoundaryChunk(in, chunkPosition, numLengthBytes, chunkLength));
						chunks.push_back(chunk);
						bytesLoaded += chunkLength;
						break;
					}
					case XDFChunk::STREAMFOOTER_CHUNK: {
						boost::shared_ptr<StreamFooterChunk> chunk(new StreamFooterChunk(in, chunkPosition, numLengthBytes, chunkLength));
						chunks.push_back(chunk);
						bytesLoaded += chunkLength;
						break;
					}

					default: {
						boost::shared_ptr<UnrecognizedChunk> chunk(new UnrecognizedChunk(in, chunkPosition, numLengthBytes, chunkLength, tagNumber));
						chunks.push_back(chunk);
						bytesLoaded += chunkLength;
						break;
					}

				
				}
				//update progress bar
			    QMetaObject::invokeMethod(progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, (chunkPosition*100)/fileSize));     

				
			}
			//set progress bar to zero
		    QMetaObject::invokeMethod(progressBar,  "setValue", Qt::QueuedConnection, Q_ARG(int, 0));    

			
		}

    }
};

#endif // XDFFILE_H
