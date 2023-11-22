#!/usr/bin/env python

import io
import os
import subprocess
import sys

input_filename = "input.txt"
output_filename = "output.txt"


def syllabify(text_to_syllabify):
    a = subprocess.Popen("./syllabifier.bin", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    stdo, stde = a.communicate(text_to_syllabify.encode("iso-8859-13"))

    return stdo.decode("iso-8859-13")


with io.open(input_filename, "r", encoding="utf-8") as input_file:
    text = input_file.read()

result = syllabify(text)

with io.open(output_filename, "w", encoding="utf-8") as output_file:
    output_file.write(result)
