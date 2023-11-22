# Modified EKI syllabifier

Intended for use on the ELLE Linux server.

## Compiling the program

* Run the command `g++ syllabifier.cpp syllabifier_trie.cpp -o syllabifier.bin` in the project's root directory

## Running the program

* Make sure the files `syllabifier.exceptions` and `syllabifier.specialchars` are in the same directory as the
  compiled `syllabifier.bin`
* Run the command `./syllabifier.bin`
* Input the desired text to syllabify
    * If the terminal encoding is not set to `ISO-8859-13`, then the program will crash if any of the following
      characters are inserted: Šš, Žž, Õõ, Ää, Öö, Üü
    * The program expects a single string with non-alphanumeric characters removed (i.e. only letters, numbers and
      spaces are allowed)

## Running the Python script

This script intends to run the program smoothly on any terminal, converting the input encoding to `ISO-8859-13` and back
to the terminal's encoding once the syllabification is done.

* Create a file called `input.txt` in the same directory as the script and the compiled `syllabifier.bin`
    * The script expects a text file containing a single string with non-alphanumeric characters removed (i.e. only
      letters, numbers and spaces are allowed)
* Run the command `./syllabify_text_file.py`
    * To measure the programs performance, run `time ./syllabify_text_file.py`
* The result of the syllabification will appear in a file called `output.txt`
