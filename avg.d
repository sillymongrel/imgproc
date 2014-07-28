import std.stdio;
import std.file;
import std.exception;
import std.conv;
import std.string;

void main(string[] args)
{
    enforce(args.length == 2, "Usage: avg <infile>");
    
    string dataFile = args[1];

    auto f= File(dataFile, "r");

    foreach(lineIn; f.byLine) {
        auto vals = lineIn.to!string.chomp.strip.split(" ");

        float sum = 0.0;
        foreach(val; vals) {
            sum += val.strip.to!float;
        }

        sum /= vals.length;
        writefln("avg:%s", sum);                        
    }

}
