import datetime
import os

my_file = open("sample_data.txt", "a+")
my_file.write("%s %d\n" % (datetime.datetime.now(), 101))
my_file.close()

