cmake_minimum_required(VERSION 3.15...3.23)

add_library(KeyValuePairExtractorLib KeyValuePairExtractor.h KeyValuePairExtractorBuilder.cpp KeyValuePairExtractorBuilder.h
        impl/state/StateHandlerImpl.h
        impl/Configuration.cpp
        EscapeSequenceParser.cpp
        EscapeSequenceParser.h
        BufferBase.cpp
        BufferBase.h
        ReadBuffer.cpp
        ReadBuffer.cpp
        ReadBuffer.h
        ReadBufferFromMemory.cpp
        ReadBufferFromMemory.h
        SeekableReadBuffer.cpp
        SeekableReadBuffer.h
        WithFileSize.cpp
        WithFileSize.h)
