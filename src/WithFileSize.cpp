#include <stdexcept>
#include "WithFileSize.h"
#include <ReadBuffer.h>


template <typename T>
static size_t getFileSize(T & in)
{
    if (auto * with_file_size = dynamic_cast<WithFileSize *>(&in))
    {
        return with_file_size->getFileSize();
    }

    throw std::runtime_error ("Cannot find out file size");
}

size_t getFileSizeFromReadBuffer(ReadBuffer & in)
{
//    if (auto * delegate = dynamic_cast<ReadBufferFromFileDecorator *>(&in))
//    {
//        return getFileSize(delegate->getWrappedReadBuffer());
//    }
//    else if (auto * compressed = dynamic_cast<CompressedReadBufferWrapper *>(&in))
//    {
//        return getFileSize(compressed->getWrappedReadBuffer());
//    }

    return getFileSize(in);
}

std::optional<size_t> tryGetFileSizeFromReadBuffer(ReadBuffer & in)
{
    try
    {
        return getFileSizeFromReadBuffer(in);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

bool isBufferWithFileSize(const ReadBuffer & in)
{
//    if (const auto * delegate = dynamic_cast<const ReadBufferFromFileDecorator *>(&in))
//    {
//        return delegate->isWithFileSize();
//    }
//    else if (const auto * compressed = dynamic_cast<const CompressedReadBufferWrapper *>(&in))
//    {
//        return isBufferWithFileSize(compressed->getWrappedReadBuffer());
//    }

    return dynamic_cast<const WithFileSize *>(&in) != nullptr;
}

size_t getDataOffsetMaybeCompressed(const ReadBuffer & in)
{
//    if (const auto * delegate = dynamic_cast<const ReadBufferFromFileDecorator *>(&in))
//    {
//        return getDataOffsetMaybeCompressed(delegate->getWrappedReadBuffer());
//    }
//    else if (const auto * compressed = dynamic_cast<const CompressedReadBufferWrapper *>(&in))
//    {
//        return getDataOffsetMaybeCompressed(compressed->getWrappedReadBuffer());
//    }
//    else if (const auto * peekable = dynamic_cast<const PeekableReadBuffer *>(&in))
//    {
//        return getDataOffsetMaybeCompressed(peekable->getSubBuffer());
//    }

    return in.count();
}
