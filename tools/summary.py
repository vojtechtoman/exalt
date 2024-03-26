#! /usr/bin/python

import sys
import os
import string
import fpformat


all_files = os.listdir('.')
files = {}
complete_files = {}
avg_bpc = {}


def add_files(suffix):
    l = len(suffix)
    for file in all_files:
        without_suffix = file[:-l] 
        if file[-l:] == suffix:
            size = os.stat(file)[6]
            if files.has_key(without_suffix):
                files[without_suffix] = files[without_suffix] + [[suffix, [size, 0]]]
            else:
                files[without_suffix] = [[suffix, [size, 0]]]




for suffix in sys.argv[1:]:
    add_files('.' + suffix)


for key in files.keys():
    if len(files[key]) != len(sys.argv[1:]):
        print 'Some files for', key, 'were not found!'
    else:
        complete_files[key] = files[key]


if len([complete_files.keys().sort()]) == 0:
    print 'No files found'
else:
    print string.ljust('', 10),

    # display the suffixes (taken from the last complete file)
    for rec in complete_files[complete_files.keys()[0]]:
        print string.ljust(' ' + rec[0], 18),
        avg_bpc[rec[0]] = 0;

    

    # calculate the best bpc for files
    for file in complete_files.keys():
        # find the best bpc
        smallest = complete_files[file][1][0]
        for suf in complete_files[file]:
            if suf[1][0] < smallest:
                smallest = suf[1][0]

        # mark up the item with best bpc
        for suf in complete_files[file]:
            if suf[1][0] == smallest:
                suf[1][1] = 1
            else:
                suf[1][1] = 0


    # display the info for each file
    for file in complete_files.keys():

        print '\n' + string.ljust(file, 10),
        
        base_size = complete_files[file][0][1][0]
        
        print string.ljust(`base_size`, 18),
        
        for rec in complete_files[file][1:]:
            avg = float(rec[1][0]*8)/base_size
            if rec[1][1] == 1:
                print string.ljust('*' + `rec[1][0]` + ' (' + fpformat.fix(avg, 3) + ')', 18),
            else:
                print string.ljust(' ' + `rec[1][0]` + ' (' + fpformat.fix(avg, 3) + ')', 18),

            avg_bpc[rec[0]] = avg_bpc[rec[0]] + avg


    print
    print
    print 'Average bpc:'

    for rec in avg_bpc.keys():
        print rec, avg_bpc[rec]/len(complete_files.keys())
