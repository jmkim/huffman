#include "huffman.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

#include <getopt.h>

using namespace algorithm;

struct Msg
{
    static void
    Version(std::ostream & out)
    { out << "Huffman encoding with RLE compressor\n"; }

    static void
    Usage(std::ostream & out, const std::string & this_file)
    { out << "Usage: " << this_file << " [OPTION]... [INPUT FILENAME]\n"; }

    static void
    Help(std::ostream & out, const std::string & this_file)
    {
        Version(out);
        Usage(out, this_file);
        out << "\nMandatory arguments to long options are mandatory for short options too.\n"
            << "\n"
            << "Options:\n"
            << "  -h,  --help                      print this help\n"
            << "  -c,  --compress                  compress with huffman coding (this is the default)\n"
            << "  -d,  --decompress                decompress with huffman decoding\n"
            << "  -o,  --output-file=FILENAME      specify the output path (default is stdout)\n";
    }

    static void
    InvalidOption(std::ostream & out, const std::string & this_file)
    {
        Usage(out, this_file);
        out << "\nTry `" << this_file << " --help' for more options.\n";
    }

    static void
    ArgumentNotProvided(std::ostream & out, const std::string & this_file)
    {
        out << this_file << ": missing argument\n";
        InvalidOption(out, this_file);
    }

    static void
    TooManyArguments(std::ostream & out, const std::string & this_file)
    {
        out << this_file << ": too many arguments\n";
        InvalidOption(out, this_file);
    }

    static void
    CannotOpenFile(std::ostream & out, const std::string & fin_path)
    { out << fin_path << ": " << strerror(ENOENT) << "\n"; }
    
    static void
    CompressFailed(std::ostream & out, const std::string & path)
    { out << "Cannot compress file " << path << ".\n"; }
    
    static void
    DecompressFailed(std::ostream & out, const std::string & path)
    { out << "Cannot decompress file " << path << ".\n"; }
};

int
main(const int argc, char * const argv[])
{
    int retval = 0;

    std::string fin_path;
    std::string fout_path;
    bool compress = true;

    {
        /** getopt(3) */

        int c;
        int digit_optind = 0;

        while(true)
        {
            int this_option_optind = optind ? optind : 1;
            int option_index = 0;
            static struct option options[] =
            {
                { "compress",       no_argument,        nullptr, 'c' },
                { "decompress",     no_argument,        nullptr, 'd' },
                { "help",           no_argument,        nullptr, 'h' },
                { "output-file",    required_argument,  nullptr, 'o' }
            };

            c = getopt_long(argc, argv, "cdho:", options, &option_index);

            if(c == -1)
                break;

            switch(c)
            {
            case 'c': /** --compress */
                compress = true;
                break;

            case 'd': /** --decompress */
                compress = false;
                break;

            case 'o': /** --output-file */
                fout_path = optarg;
                break;

            case 'h': /** --help */
                Msg::Help(std::cout, argv[0]);
                goto jump_exit;

            case '?': /** Invalid option */
                Msg::InvalidOption(std::cout, argv[0]);
                goto jump_exit;

            default:
                break;
            }
        }

        if(optind + 1 == argc) /** Only one argument except options */
            fin_path = argv[optind];
        else
        {
            if(argc > 2)
                Msg::TooManyArguments(std::cout, argv[0]);
            else
                Msg::ArgumentNotProvided(std::cout, argv[0]);

            goto jump_exit;
        }
    }

    if(compress)
    {
        /** Compression */

        std::ifstream fin(fin_path, std::ios::binary);
        if(! fin.is_open())
        {
            Msg::CannotOpenFile(std::cerr, fin_path);
            Msg::CompressFailed(std::cerr, fin_path);

            retval = ENOENT;
            goto jump_exit;
        }

        Huffman huffman;

        if(fout_path.empty())
            huffman.Compress(fin, std::cout);
        else
        {
            std::ofstream fout(fout_path, std::ios::binary);
            huffman.Compress(fin, fout);
            fout.close();
        }

        fin.close();
    }
    else
    {
        /** Decompression */

        std::ifstream fin(fin_path, std::ios::binary);
        if(! fin.is_open())
        {
            Msg::CannotOpenFile(std::cerr, fin_path);
            Msg::DecompressFailed(std::cerr, fin_path);

            retval = ENOENT;
            goto jump_exit;
        }

        Huffman huffman;

        if(fout_path.empty())
            huffman.Decompress(fin, std::cout);
        else
        {
            std::ofstream fout(fout_path, std::ios::binary);
            huffman.Decompress(fin, fout);
            fout.close();
        }

        fin.close();
    }

jump_exit:
    return retval;
}
