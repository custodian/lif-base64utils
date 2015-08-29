// base64encoder.cpp : Defines the entry point for the application.
//
#include "targetver.h"

#include "tchar.h"
#define NOMINMAX
#include "windows.h"

#include <iostream>
#include <fstream>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using namespace boost::iostreams;

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
using namespace boost::archive::iterators;

using namespace std;

typedef 
    insert_linebreaks<
        base64_from_binary<
            transform_width<
                istreambuf_iterator<char>,
                6,
                8
            >
        > 
        ,50002
    > 
    t_b64_from_bin;

int APIENTRY _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow
)
{
    int argc = 0;
    TCHAR** argv = CommandLineToArgvW(lpCmdLine, &argc);

    if (argc < 2) {
        cout << "Usage: input output gzip";
        return -1;
    }

    fstream fsin;
    fsin.open(argv[0], ios::binary | ios::in);
    if (!fsin.is_open()) {
        cout << "Error opening input file: " << argv[0];
        return -1;
    }

    fstream fsout;
    fsout.open(argv[1], ios::binary | ios::out);
    if (!fsout.is_open()) {
        cout << "Error opening output file: " << argv[1];
        return -1;
    }

    filtering_stream<input> in_data;
    in_data.push(gzip_compressor(gzip::default_compression,64000));
    in_data.push(fsin);
        
    //string data((istreambuf_iterator<char>(in_data)), istreambuf_iterator<char>());

    copy(
        t_b64_from_bin(istreambuf_iterator<char>(in_data)),
        t_b64_from_bin(istreambuf_iterator<char>()),
        std::ostream_iterator<char>(fsout)
    );

    return 0;
}