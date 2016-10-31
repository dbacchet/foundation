#pragma once

#include <vector>
#include <string>
#include <sstream>


/// convert the given value to string
template<typename T>
inline std::string to_string( const T& val)
{  
    std::ostringstream ss; 
    ss << val;
    return ss.str();
}

/// convert string to number
template <typename T>
inline T to_number ( const std::string &text )
{
	std::istringstream ss(text);
	T result;
    // return ss >> result ? result : 0;
    ss >> result;
    return result;
}

/// strip delimiters from beginning and end
inline std::string strip( const std::string  &text, const char *delimiters=" " )
{
    std::string::size_type const first = text.find_first_not_of(delimiters);
    return ( first==std::string::npos ) ? "" : text.substr(first, text.find_last_not_of(delimiters)-first+1);
}

/// tokenize a string given a delimiter
inline std::vector<std::string> tokenize( const std::string  &text, char delimiter=' ' )
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(text);
    while ( getline(iss, token, delimiter) )
    {
        tokens.push_back(token);
    }
    return tokens;
}

/// convert string to vector
template <typename T>
inline std::vector<T> to_vector ( const std::string &text, char delimiter=' ' )
{
    std::vector<std::string> tmp = tokenize(text,delimiter);
    std::vector<T> out;
    for (size_t i=0; i<tmp.size(); i++)
        out.push_back(to_number<T>(tmp[i]));
    return out;
}

