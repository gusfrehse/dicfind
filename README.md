# dicfind
Find words that satisfies a certain pattern.
## Usage
```$ ./dicfind [options] <pattern> <path-to-dictionary>```

Where:
 - [options] are `-s` for simple output (will not show how many matches) and `-r` result only (will only show how many matches).
 - \<pattern\> is a string which only contains the firsts letter of the alphabet. Ex.: `abc` is a valid pattern `bcd` is not.
 - \<path-to-dictionary\> is the path to the dictionary file. ASCII and one word per line is assumed.
