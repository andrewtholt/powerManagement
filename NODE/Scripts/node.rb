#!/usr/bin/ruby

require 'inifile'
require 'pp'

def main
  roles = [ "base", "dev" ]
  
  puts ARGV[0]
  
  if ARGV[0] == nil
    puts "Usage node.rb <hostname>"
    exit
  end
  
  hostName=ARGV[0]
  
  for file in roles
    fileName='../Data/' + file + '.ini'
    
    if File.file?( fileName )
      file = IniFile.load('../Data/' + file + '.ini')
      puts file
      
      for section in file
	data = file[section]
	puts "apt-get -y install " + section
	
	if data["action"] != "NONE"
	  puts data["action"]
	  
	  #     pkgList = file[section]
	  #     p = pkgList["PKGS"]
	  #     
	  #     for n in p.split(",")
	  #       puts "apt-get -y install " + n
	end
      end
    else
      puts "File " + fileName + " not found."
    end
    
  end
end

main