import string
import random
import json
import threading

alfa_characters = string.ascii_lowercase + string.ascii_uppercase
alnum_characters = alfa_characters + string.digits
punctuation_characters = string.punctuation.replace('"', '').replace(":", "").replace("\\", "")


def randomStringWithCharacters(length, characters):
    letters = list(characters)
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str


def randomString(length):
    return randomStringWithCharacters(length, alnum_characters + punctuation_characters)


def randomAlfaString(length):
    return randomStringWithCharacters(length, alfa_characters)

def randomAlnumString(length):
    return randomStringWithCharacters(length, alnum_characters)


def randomNotAlnumString(length):
    return randomStringWithCharacters(length, punctuation_characters)


def shouldEnclose():
    return random.random() < 0.3


def shouldMessUp():
    return random.random() < 0.3


def shouldUnnecessarySpace():
    return random.random() < 0.4


def validDictionary(length, d):
    for i in range(length):
        k = randomAlfaString(1) + randomAlnumString(random.randint(1, 64))
        v = randomAlfaString(1) + randomAlnumString(random.randint(0, 64))
        d[k] = v


def invalidDictionary(length, d):
    for i in range(length):
        print(i if i % 50 else "")
        d[randomNotAlnumString(random.randint(1, 64))] = randomAlnumString(random.randint(0, 64))


def validDictionaryParallel(length, threadCount):
    if threadCount > length:
        threadCount = length

    chunkSize = length // threadCount

    threads = []

    result = dict()


    for i in range(threadCount):
        dd = dict()
        thread = threading.Thread(target=validDictionary, args=(chunkSize, dd))
        threads.append((thread, dd))
        thread.start()

    for thread in threads:
        thread[0].join()
        result = (result | thread[1])

    return result

def invalidDictionaryParallel(length, threadCount):
    if threadCount > length:
        threadCount = length

    chunkSize = length // threadCount

    threads = []

    result = dict()


    for i in range(threadCount):
        dd = dict()
        thread = threading.Thread(target=invalidDictionary, args=(chunkSize, dd))
        threads.append((thread, dd))
        thread.start()

    for thread in threads:
        thread[0].join()
        result = (result | thread[1])

    return result

def spaceCharacters(length):
    sc = ''
    for i in range(length):
        sc = sc + ' '
    return sc


validDictSize = 10000000

valid_dictionary = validDictionaryParallel(validDictSize, 24)
expected_output_json = json.dumps(valid_dictionary)

invalidDictionarySize = 100000

invalid_dictionary = invalidDictionaryParallel(invalidDictionarySize, 24)
invalid_dictionary_json = json.dumps(invalid_dictionary)

input_dictionary = dict(valid_dictionary | invalid_dictionary)

inputDictionaryList = list(input_dictionary.items())

random.shuffle(inputDictionaryList)

inputDictionaryShuffled = dict(inputDictionaryList)


# ta tendo alguns kvp inválidos na saída do c++
# isso é provavelmente pq o dictionary ta misturado e o quoting character ta interefrindo isso
# pensar na solução dps da academia

with open("big_input_file.txt", "w") as f:
    for key, value in inputDictionaryShuffled.items():
        if shouldMessUp():
            rnans = randomNotAlnumString(random.randint(1, 100))
            invalidString = rnans + rnans
            f.write(invalidString)

        keyEncloseCharacter = '"' if shouldEnclose() and key not in invalid_dictionary else ''
        valueEncloseCharacter = '"' if shouldEnclose() and key not in invalid_dictionary else ''

        keyString = keyEncloseCharacter + key + keyEncloseCharacter
        valueString = valueEncloseCharacter + value + valueEncloseCharacter
        space_characters = spaceCharacters(random.randint(1, 10)) if shouldUnnecessarySpace() else ''

        kvp = space_characters + keyString + ":" + space_characters + valueString + ","
        f.write(kvp)

with open("big_output_file.json", "w") as f:
    f.write(expected_output_json)
