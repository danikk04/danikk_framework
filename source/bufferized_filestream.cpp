#include <danikk_framework/bufferized_filestream.h>

namespace danikk_framework
{
	BufferizedFileStream::BufferizedFileStream()
	{
		buffer.reserve(buffer_capacity);
	}

	void BufferizedFileStream::flushBuffer()
	{
		FileStream::write(buffer.data(), buffer.size());
		buffer.clear();
	}

    void BufferizedFileStream::flush()
    {
		flushBuffer();
    	FileStream::flush();
    }

    LineFileWriter::LineFileWriter()
	{
		line_buffer.reserve(2 * 1024);
	}

	void LineFileWriter::setLineCapacity(uint32 line_capacity)
	{
		this->line_capacity = line_capacity;
	}

	bool LineFileWriter::hasBeenFlushed()
	{
		return line_buffer.size() == 0;
	}

    LineFileWriter& LineFileWriter::operator<<(char chr)
	{
    	line_buffer << chr;
    	if(chr == '\n')
    	{
    		line_counter++;
    		if(line_counter == line_capacity)
    		{
    			line_counter = 0;
    			FileStream::write(line_buffer.data(), line_buffer.size());
    			FileStream::flush();
    			line_buffer.resize(0);
    		}
    	}
    	return *this;
	}

    LineFileWriter& LineFileWriter::operator<<(const char* str)
    {
    	while(*str)
    	{
    		*this << *str++;
    	}
    	return *this;
    }

    LineFileReader::LineFileReader()
    {
    	buffer.resize(4 * 1024);
    	buffer_offset = buffer.capacity();
    }

    int LineFileReader::eof()
    {
    	return buffer_offset == buffer.size() && FileStream::eof();
    }

	bool LineFileReader::read(String& str)
	{
		str.clear();
		while(true)
		{
			if(buffer_offset == buffer.capacity() || buffer.size() == 0)				//если буффер прочитан, то нужно читать из файла
			{
				buffer_offset = 0;														//обнуляем смещение в буффере
				size_t read_size = FileStream::read(buffer.data(), buffer.capacity());	//читаем из файла в буффер
				if(read_size != buffer.capacity())										//если прочитали меньше чем вместимость буффера, то нужно изменить размер буффера
				{
					buffer.resize(read_size);
				}
			}
			if(buffer_offset < buffer.size())
			{
				str << buffer.data()[buffer_offset++];
			}
			else
			{
				return false;
			}
		}
		str.resize(str.size() - 1);
		return true;
	}
}
