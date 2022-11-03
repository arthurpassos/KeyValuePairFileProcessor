# KeyValuePairFileProcessor

### Ideas to improve performance:

#### Both approaches:

1. Maintain separate implementations for different algorithm options to avoid checking the very same option in a loop. An example would be the optional enclosing/ quote character. Instead of checking if the character is set in every iteration, instantiate a different implementation depending on the presence of it. Will likely increase code duplication, but will probably improve performance and simplify the algorithm.
2. Check if RVO applies where necessary and scan code for unnecessary copies.

#### Lazy escaping:

1. store an extra bool flag alongside the view to indicate whether that particular string needs escaping

#### Inline escaping:
1. pre-allocate memory based on an arbitrary value for keys and values. Something like 64 characters

### Ideas to improve code
1. Pick a single implementation and maintain only one, this avoids unnecessary abstractions and keeps it simpler.
2. Maybe implement some fashion of a state design pattern? State pattern implies a class for each state, maybe something along those lines?
