#!/usr/bin/expect

proc install { cmd pkg } {
  puts "Install $pkg"
  send "which $cmd ; echo $?\r\n"
 
  expect {
    "1" { 
	  send "apt-get install $pkg\r\n"
	}
    "0" { }
    }
  expect "# "
}

proc mkfolder { name } {
  send "mkdir -p $name\r\n"
  
  expect "# "
}

puts $argc

if { $argc != 2 } {
  puts "Need a host name"
  exit 1
}

set hostname [ lindex $::argv 0 ]
set password [ lindex $::argv 1 ]

puts $hostname
puts $password

spawn ssh $hostname

expect {
  "(yes/no)?" { send "yes\r\n" }
}

expect {
  "word: " { send "$password\r\n" }
}

expect " $"
send "sudo -i\r\n"

expect {
  "word for " { send "$password\r\n" }
}

expect "# "

install "tsp" "task-spooler"
install "ruby" "ruby"
install "wget" "wget"

set data "/var/manage/Data"
mkfolder "$data"

send "cd $data\r\n"
expect "# "

send "wget http://192.168.0.15:8080/manage/Data/whatAmI.ini -O ./whatAmI.ini\r\n"
expect "# "



interact