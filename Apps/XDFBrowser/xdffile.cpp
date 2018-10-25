/********************************************************************

C++ XDF File implementation. See https://code.google.com/p/xdf/wiki/Specifications for XDF file specification.

This particular implementation is focused on low level operations with respect to the file. The individual chunks 
are preserved. Headers can be inspected and edited. This implementation defaults to require only 
50 MB of memory, regardless of file size. 

Written by Matthew Grivich, Swartz Center for Computation Neuroscience, Summer 2014.

*********************************************************************/

#include "xdffile.h"

/********************************************************************

XDF Chunks Implementation

*********************************************************************/

qint8 num_length_bytes(quint64 num) {
	if(num > std::numeric_limits<quint32>::max()) return 8;
	else if(num > std::numeric_limits<quint8>::max()) return 4;
	else return 1;
}

quint64 read_varlen_int(QDataStream& in) {
	qint8 numLengthBytes;
	in >> numLengthBytes;

	switch(numLengthBytes){
	    case 1:
		    quint8 chunkLengthByte;
			in >> chunkLengthByte;
		    return chunkLengthByte;
	    case 4:
		    quint32 chunkLengthInt;
			in >> chunkLengthInt;
		    return chunkLengthInt;
	    case 8:
		    quint64 chunkLengthLong;
			in >> chunkLengthLong;
		    return chunkLengthLong;
	    default:
		    throw std::runtime_error(
		        "XDF File is incorrectly formatted: numLengthBytes is an illegal value: " +
		        std::to_string(numLengthBytes));
	}
}

qint8 XDFChunk::thisNumLengthBytes()
{
	return num_length_bytes(thisChunkLength);
}

QString XDFChunk::readString(QDataStream &in, size_t length) {
	char * buf = (char *) malloc(length+1);

	in.readRawData(buf, length);
	buf[length] = 0;
	QString str(buf);

	free(buf);
	return str;
}

XDFChunk::~XDFChunk()
{
}


void XDFChunk::writeChunk(QDataStream &out){
	quint8 chunkLengthByte = 0;
	quint32 chunkLengthInt = 0;
	out << thisNumLengthBytes();
	switch(thisNumLengthBytes()){
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

			
}

/********************************************************************

File Header Chunk Implementation

*********************************************************************/


FileHeaderChunk::FileHeaderChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength) : XDFChunk(chunkPosition, chunkLength){
	
	chunkData = readString(in, chunkLength-2);
}

QString FileHeaderChunk::getName() {
	return QString("File Header");
}

QString FileHeaderChunk::getText() {
	return chunkData;
}

void FileHeaderChunk::setText(const QString& text) {
	chunkData = text;
	thisChunkLength = chunkData.size() + 2;
}

void FileHeaderChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << FILEHEADER_CHUNK;
	out.writeRawData(chunkData.toLatin1(), thisChunkLength-2);
}

FileHeaderChunk::~FileHeaderChunk(){};

/********************************************************************

Stream Header Chunk Implementation

*********************************************************************/


StreamHeaderChunk::StreamHeaderChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength) : XDFChunk(chunkPosition, chunkLength){
	in >> streamID;
	chunkData = readString(in, chunkLength-6);
}

QString StreamHeaderChunk::getName() {
	char str[256];
	sprintf(str,"Stream Header: %d", streamID);
	return QString(str);
}

QString StreamHeaderChunk::getText() {
	return chunkData;
}

void StreamHeaderChunk::setText(const QString& text) {
	chunkData = text;
	thisChunkLength = chunkData.size() + 6;
}

void StreamHeaderChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << STREAMHEADER_CHUNK;
	out << streamID;
	out.writeRawData(chunkData.toLatin1(), thisChunkLength-6);
}

StreamHeaderChunk::~StreamHeaderChunk(){};

/********************************************************************

Samples Chunk Implementation

*********************************************************************/

SamplesChunk::SamplesChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength, bool loadData) : XDFChunk(chunkPosition, chunkLength), chunkData(nullptr) {
	in >> streamID;
	if(loadData) {
		chunkData =  (char *) malloc(chunkLength-6);

		in.readRawData(chunkData, chunkLength-6);
		loaded = true;
	} else {
		chunkData = nullptr;
		in.skipRawData(chunkLength-6);
		loaded = false;
	}
	
}


QString SamplesChunk::getName() {
	if(loaded)
		return QStringLiteral("Samples Chunk, Stream #%1").arg(streamID);
	else
		return QStringLiteral("Samples Chunk");
}

QString SamplesChunk::getText() {
	if(loaded) {
		auto text = QStringLiteral("Binary Data, Stream %1, %2 bytes\n\n").arg(streamID).arg(thisChunkLength);
		return text;
	}
	else
		return QStringLiteral("Binary Data, not loaded");
}

void SamplesChunk::setText(const QString& text){}

void SamplesChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << SAMPLES_CHUNK;
	out << streamID;
	out.writeRawData(chunkData, thisChunkLength - 6);

}

quint64 SamplesChunk::loadData(QFile &file) {
	file.seek(thisChunkPosition);
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	thisChunkLength = read_varlen_int(in);

	quint16 tagNumber;
	in >> tagNumber;

	in >> streamID;

	if(chunkData) free(chunkData);
	chunkData =  (char *) malloc(thisChunkLength-6);

	in.readRawData(chunkData, thisChunkLength-6);
	loaded = true;
	return thisChunkLength;

}

quint64 SamplesChunk::unloadData() {
	if(chunkData) free(chunkData);
	chunkData = nullptr;
	loaded = false;
	return thisChunkLength;
}

SamplesChunk::~SamplesChunk(){
	if(chunkData) free(chunkData);
	chunkData = nullptr;
}

/********************************************************************

Clock Offset Chunk Implementation

*********************************************************************/

ClockOffsetChunk::ClockOffsetChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength) : XDFChunk(chunkPosition, chunkLength){

	in >> streamID;
	in >> collectionTime;
	in >> offsetValue;
	
}

QString ClockOffsetChunk::getName() {
	return QString("Clock Offset Chunk");
}

QString ClockOffsetChunk::getText() {
	QString str;
	QTextStream(&str) << "streamID: " << streamID << "\n"
		<< "collectionTime: " << collectionTime << "\n"
		<< "offsetValue: " << offsetValue << "\n";
	return str;
}

void ClockOffsetChunk::setText(const QString &text){}

void ClockOffsetChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << CLOCKOFFSET_CHUNK;
	out << streamID;
	out << collectionTime;
	out << offsetValue;
}

ClockOffsetChunk::~ClockOffsetChunk(){

}

/********************************************************************

Boundary Chunk Implementation

*********************************************************************/


BoundaryChunk::BoundaryChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength) : XDFChunk(chunkPosition, chunkLength), chunkData(nullptr) {
	chunkData =  (quint8 *) malloc(chunkLength-2);
	for(int i=0; i<chunkLength-2; i++) {
		in >> chunkData[i];
	}
	if(!(
		chunkData[0] ==  0x43 &&
		chunkData[1] ==  0xA5 &&
		chunkData[2] == 0x46 &&
		chunkData[3] == 0xDC &&
		chunkData[4] == 0xCB &&
		chunkData[5] == 0xF5 &&
		chunkData[6] == 0x41 &&
		chunkData[7] == 0x0F &&
		chunkData[8] == 0xB3 &&
		chunkData[9] == 0x0E &&
		chunkData[10] == 0xD5 &&
		chunkData[11] == 0x46 &&
		chunkData[12] == 0x73 &&
		chunkData[13] == 0x83 &&
		chunkData[14] == 0xCB &&
		chunkData[15] == 0xE4)) {
		throw IOException("Boundary chunk has the wrong signature.\n");
	}

}

QString BoundaryChunk::getName() {
	return QString("Boundary Chunk");
}

QString BoundaryChunk::getText() {
	return QString("0x43 0xA5 0x46 0xDC 0xCB 0xF5 0x41 0x0F 0xB3 0x0E 0xD5 0x46 0x73 0x83 0xCB 0xE4");
}

void BoundaryChunk::setText(const QString& text){}

void BoundaryChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << BOUNDARY_CHUNK;
	for(int i=0; i< thisChunkLength-2; i++) {
		out << chunkData[i];
	}
}

BoundaryChunk::~BoundaryChunk(){
	if(chunkData) free(chunkData);
	chunkData = nullptr;
}


/********************************************************************

Stream Footer Chunk Implementation

*********************************************************************/

StreamFooterChunk::StreamFooterChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength) : XDFChunk(chunkPosition, chunkLength){
	in >> streamID;
	chunkData = readString(in, chunkLength-6);
}

QString StreamFooterChunk::getName() {
	char str[256];
	sprintf(str,"Stream Footer: %d", streamID);
	return QString(str);
}

QString StreamFooterChunk::getText() {
	return chunkData;
}

void StreamFooterChunk::setText(const QString& text) {
	chunkData = text;
	thisChunkLength = chunkData.size() + 6;
}

void StreamFooterChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << STREAMFOOTER_CHUNK;
	out << streamID;
	out.writeRawData(chunkData.toLatin1(), thisChunkLength-6);
}

StreamFooterChunk::~StreamFooterChunk(){};

/********************************************************************

Unrecognized Chunk Implementation

*********************************************************************/

UnrecognizedChunk::UnrecognizedChunk(QDataStream &in, quint64 chunkPosition, quint64 chunkLength, quint16 chunkTag) : XDFChunk(chunkPosition, chunkLength), chunkData(nullptr){
	chunkData =  (char *) malloc(chunkLength-2);
	thisChunkTag = chunkTag;

	in.readRawData(chunkData, chunkLength-2);
	
	
}

QString UnrecognizedChunk::getName() {
	return QString("Chunk type not recognized.");
}

QString UnrecognizedChunk::getText() {
	return QString("Chunk type not recognized.");
}

void UnrecognizedChunk::setText(const QString& text) {}

void UnrecognizedChunk::writeChunk(QDataStream &out) {
	XDFChunk::writeChunk(out);
	out << thisChunkTag;
	out.writeRawData(chunkData, thisChunkLength-2);
}

UnrecognizedChunk::~UnrecognizedChunk(){
	if(chunkData) free(chunkData);
	chunkData = nullptr;
}

 
/********************************************************************

  XDFFile Implementation

*********************************************************************/

void XDFfile::open(QProgressBar *progressBar)  {
	
	if(!file.open(QIODevice::ReadOnly)) throw IOException("Unable to open file.\n");		

	qint64 fileSize = file.size();
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	char buf[5];
	in.readRawData(buf, 4);
	buf[4] = 0;
	if(QString(buf).compare(QString("XDF:"))){
		throw IOException("File is not an XDF file\n");
	} else {
		int count = 0;
		while(true) {
			quint64 chunkPosition = file.pos();
			try {
			if(chunkPosition >= (quint64) file.size()) break;

			quint64 chunkLength = read_varlen_int(in);

			count++;
			quint16 tagNumber;
			in >> tagNumber;


			switch(tagNumber){
				case XDFChunk::FILEHEADER_CHUNK: {
				    std::shared_ptr<FileHeaderChunk> chunk(new FileHeaderChunk(in, chunkPosition, chunkLength));
					chunks.push_back(chunk);
					bytesLoaded += chunkLength;
					break;
					}
				case XDFChunk::STREAMHEADER_CHUNK: {
				    std::shared_ptr<StreamHeaderChunk> chunk(new StreamHeaderChunk(in, chunkPosition, chunkLength));
					chunks.push_back(chunk);
					bytesLoaded += chunkLength;
					break;
				}
				case XDFChunk::SAMPLES_CHUNK: {
					bool loadData = bytesLoaded < maxBytes;					
					std::shared_ptr<SamplesChunk> chunk(new SamplesChunk(in, chunkPosition, chunkLength, loadData));
					chunks.push_back(chunk);
					if(loadData) {
						bytesLoaded += chunkLength;
						loadedChunks.push_back(chunks.size()-1);
					}
					break;
				}
				case XDFChunk::CLOCKOFFSET_CHUNK: {
				    std::shared_ptr<ClockOffsetChunk> chunk(new ClockOffsetChunk(in, chunkPosition, chunkLength));
					chunks.push_back(chunk);
					bytesLoaded+= chunkLength;
					break;
				}
				case XDFChunk::BOUNDARY_CHUNK: {
				    std::shared_ptr<BoundaryChunk> chunk(new BoundaryChunk(in, chunkPosition, chunkLength));
					chunks.push_back(chunk);
					bytesLoaded += chunkLength;
					break;
				}
				case XDFChunk::STREAMFOOTER_CHUNK: {
				    std::shared_ptr<StreamFooterChunk> chunk(new StreamFooterChunk(in, chunkPosition, chunkLength));
					chunks.push_back(chunk);
					bytesLoaded += chunkLength;
					break;
				}

				default: {
				    std::shared_ptr<UnrecognizedChunk> chunk(new UnrecognizedChunk(in, chunkPosition, chunkLength, tagNumber));
					chunks.push_back(chunk);
					bytesLoaded += chunkLength;
					break;
				}
			    }
			} catch(const std::exception& e) {
				throw std::runtime_error("Error in chunk loading (chunk #" + std::to_string(count) +
				                         " starting at byte " + std::to_string(chunkPosition) +
				                         "): " + e.what());
			}
			//update progress bar
			QMetaObject::invokeMethod(progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, (chunkPosition*100)/fileSize));

			
		}
		//set progress bar to zero
		QMetaObject::invokeMethod(progressBar,  "setValue", Qt::QueuedConnection, Q_ARG(int, 0));

		
	}

}

//saves as .copy, then deletes old, renames .copy to replace old.
void XDFfile::save(QProgressBar *progressBar) {
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

void XDFfile::saveAs(const QString& fileName, QProgressBar *progressBar) {
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
		//if chunk is valid and it is not currently in memory, free up not recently used chunks and load current chunk.
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

void XDFfile::close() {
	file.close();
}

QString XDFfile::getFileName() {
	return file.fileName();
}

int XDFfile::getNChunks() {
	return chunks.size();
}

QString XDFfile::getChunkName(int element) {
	return chunks[element]->getName();
}

QString XDFfile::getChunkText(int element) {
	return chunks[element]->getText();
}

void XDFfile::setChunkText(int element, const QString& text) {
	chunks[element]->setText(text);
}


const char*IOException::what() const noexcept
{
	return description;
}
