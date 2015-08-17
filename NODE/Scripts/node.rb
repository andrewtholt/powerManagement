#!/usr/bin/ruby

require 'inifile'
require 'pp'
require 'open-uri'

def main
  roles = [ "base"]
  
  puts ARGV[0]
  
  if ARGV[0] == nil
    puts "Usage node.rb <hostname>"
    exit 
  end
  
  hostName=ARGV[0]
  
  whatAmI = IniFile.load('../Data/whatAmI.ini')
  cfg = whatAmI['config']
  
  r = cfg['roles']
  
  addThese = r.split(',')
  pp addThese
  
  roles = roles + addThese
  
  for file in roles
    fileName='../Data/' + file + '.ini'
    
    if File.file?( fileName )
      file = IniFile.load('../Data/' + file + '.ini')
      puts file
      
      for section in file
	data = file[section] 
	
	installed=FALSE
	if data["test"]
	  puts "Test " + data["test"]
	  if data['test'] == 'NONE' 
	    installed=FALSE
	  else
	    installed=system("which " + data["test"])
	  end
	end
	
	if installed
	  puts "Done"
	else
	  puts "apt-get -y install " + section
	end
	
	if data["action"] != "NONE"
	  puts data["action"]
	end
      end
    else
      puts "File " + fileName + " not found."
      download = open('http://192.168.0.15:8080/manage/Data/' + file + '.ini' )
      IO.copy_stream(download, fileName )
    end
    
  end
end

main

