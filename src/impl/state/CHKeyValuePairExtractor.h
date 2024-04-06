#pragma once

/*
 * Handle state transitions and a few states like `FLUSH_PAIR` and `END`.
 * */
#include <stdexcept>
#include "KeyValuePairExtractor.h"

template <typename StateHandler>
class CHKeyValuePairExtractor : public KeyValuePairExtractor
{
    using State = typename StateHandler::State;
    using NextState = StateHandler::NextState;

public:
    explicit CHKeyValuePairExtractor(StateHandler state_handler_, uint64_t max_number_of_pairs_)
            : state_handler(std::move(state_handler_)), max_number_of_pairs(max_number_of_pairs_)
    {}

    Response extract(const std::string & file) override
    {
        return extract(std::string_view {file});
    }

    Response extract(std::string_view data)
    {
        auto state =  State::WAITING_KEY;

        Response response;

        auto key_writer = typename StateHandler::StringWriter();
        auto value_writer = typename StateHandler::StringWriter();

        uint64_t row_offset = 0;

        while (state != State::END)
        {
            auto next_state = processState(data, state, key_writer, value_writer, row_offset, response);

            if (next_state.position_in_string > data.size() && next_state.state != State::END)
            {
                throw std::runtime_error ("Attempt to move read pointer past end of available data");
            }

            data.remove_prefix(next_state.position_in_string);
            state = next_state.state;
        }

        // below reset discards invalid keys and values
        reset(key_writer, value_writer);

        return response;
    }

private:

    NextState processState(std::string_view file, State state, auto & key, auto & value, uint64_t & row_offset, Response & response)
    {
        switch (state)
        {
            case State::WAITING_KEY:
            {
                return state_handler.waitKey(file);
            }
            case State::READING_KEY:
            {
                return state_handler.readKey(file, key);
            }
            case State::READING_QUOTED_KEY:
            {
                return state_handler.readQuotedKey(file, key);
            }
            case State::READING_KV_DELIMITER:
            {
                return state_handler.readKeyValueDelimiter(file);
            }
            case State::WAITING_VALUE:
            {
                return state_handler.waitValue(file);
            }
            case State::READING_VALUE:
            {
                return state_handler.readValue(file, value);
            }
            case State::READING_QUOTED_VALUE:
            {
                return state_handler.readQuotedValue(file, value);
            }
            case State::FLUSH_PAIR:
            {
                return flushPair(file, key, value, row_offset, response);
            }
            case State::END:
            {
                return {0, state};
            }
        }
    }

    NextState flushPair(const std::string_view & file, auto & key,
                        auto & value, uint64_t & row_offset, Response & response)
    {
        row_offset++;

        if (row_offset > max_number_of_pairs)
        {
            throw std::runtime_error ("Number of pairs produced exceeded the limit of " + std::to_string(max_number_of_pairs));
        }

        response[std::string(key.commit())] = std::string(value.commit());

        return {0, file.empty() ? State::END : State::WAITING_KEY};
    }

    void reset(auto & key, auto & value)
    {
        key.reset();
        value.reset();
    }

    StateHandler state_handler;
    uint64_t max_number_of_pairs;
};
