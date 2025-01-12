#include "ReadBufferFromMemory.h"

off_t ReadBufferFromMemory::seek(off_t offset, int whence)
{
    if (whence == SEEK_SET)
    {
        if (offset >= 0 && internal_buffer.begin() + offset <= internal_buffer.end())
        {
            pos = internal_buffer.begin() + offset;
            working_buffer = internal_buffer; /// We need to restore `working_buffer` in case the position was at EOF before this seek().
            return static_cast<size_t>(pos - internal_buffer.begin());
        }
        else
            throw std::runtime_error ("Seek position is out of bounds");
//            throw Exception(ErrorCodes::SEEK_POSITION_OUT_OF_BOUND, "Seek position is out of bounds. Offset: {}, Max: {}",
//                            offset, std::to_string(static_cast<size_t>(internal_buffer.end() - internal_buffer.begin())));
    }
    else if (whence == SEEK_CUR)
    {
        Position new_pos = pos + offset;
        if (new_pos >= internal_buffer.begin() && new_pos <= internal_buffer.end())
        {
            pos = new_pos;
            working_buffer = internal_buffer; /// We need to restore `working_buffer` in case the position was at EOF before this seek().
            return static_cast<size_t>(pos - internal_buffer.begin());
        }
        else
            throw std::runtime_error ("Seek position is out of bounds");
    }
    else
        throw std::runtime_error ("Only SEEK_SET and SEEK_CUR seek modes allowed.");
}

off_t ReadBufferFromMemory::getPosition()
{
    return pos - internal_buffer.begin();
}
