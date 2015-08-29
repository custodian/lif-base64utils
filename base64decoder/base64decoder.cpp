// base64decoder.cpp : Defines the entry point for the application.
//
#include "targetver.h"

#include "tchar.h"
#define NOMINMAX
#include "windows.h"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/assign/list_of.hpp>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using namespace boost::iostreams;

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
using namespace boost::archive::iterators;

using namespace std;

typedef
    transform_width<
        binary_from_base64<
            remove_whitespace<
                const char *
            >
        >
        ,8
        ,6
    >
    t_bin_from_b64;

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
        cout << "Usage: input output" << endl;
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

    string data((istreambuf_iterator<char>(fsin)), istreambuf_iterator<char>());
    boost::trim_right_if(data, boost::is_any_of("\r\n"));

    filtering_stream<output> out_data;    
    out_data.push(gzip_decompressor(gzip::default_window_bits,64000));        
    out_data.push(fsout);

    copy(
        t_bin_from_b64(data.c_str()),
        t_bin_from_b64(data.c_str() + data.size()),
        std::ostream_iterator<char>(out_data)
    );

    return 0;
}