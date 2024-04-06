cmake_minimum_required(VERSION 3.15...3.23)

add_library(KeyValuePairExtractorLib KeyValuePairExtractor.h KeyValuePairExtractorBuilder.cpp KeyValuePairExtractorBuilder.h
        impl/state/StateHandlerImpl.h
        impl/Configuration.cpp
        impl/EscapeSequenceParser.cpp
        impl/EscapeSequenceParser.h
        util/BufferBase.cpp
        util/ReadBufferFromMemory.cpp
        util/SeekableReadBuffer.cpp
        util/WithFileSize.cpp)
