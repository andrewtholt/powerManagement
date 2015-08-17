#!/usr/bin/ruby

require 'inifile'
require 'pp'

# read an existing file
file = IniFile.load('../Data/tst.ini')
data = file["Desktop Entry"]

#output one property
puts "here is one property:"
puts data["Name"]

# pretty print object
puts "here is the loaded file:"
pp file

# create a new ini file object
new_file = IniFile.new

# set properties
new_file["Desktop Entry"] = {
    "Type" => "Application",
    "Name" => 'test',
    "Exec" => 'command',
}

# pretty print object
puts "here is a object created with new:"
pp new_file

# set file path
new_file.filename = "/tmp/new_ini_file.ini"

# save file
new_file.write()
puts "the new object has been saved as a file to /tmp/new_ini_file.ini"
