# lanzaras

## Motivation

Associate applications to extensions in windows can be unfortunatelly very challenging.
Moreover in Windows 10 seems to be no way to do it using any command line argument but 
the association is only possible just with a single executable. This makes things like
launching jar files simply impossible. Also the set associations are not very easy to export
cleanly to another machine.
   
## One Windows executable for all extensions

You can associate any extension (e.g. myExt) to lanzaras and then write a file in the
same directory as lanzaras.exe called lanzaras.myExt.rule where you simply write the
command line to launch what you want to without giving the last parameter which is 
always the file that is being double clicked.
   
For example, suppose we want to associate the extension "gast" with the command
   
      java -jar gastona.jar "%1"
      
   1) we associate the extension gast with lanzaras.exe in windows using the standard
     dialog (search app etc)
     
   2) copy gastona.jar in the same directory where lanzaras.exe is located

   3) create in the same directory the lanzaras.gast.rule with the content
   
         @java.exe -jar $gastona.jar
         
we exclude the final parameter since this is always the file that we want to launch.

Here the characters @ and $ have following meaning 
   
       @program.exe
           will search for the first path in the environmen variable PATH where the file "program.exe"
           is found and then it will be replaced by "path from env\program" including the double quotes

       $file
           where $ represents the path where lanzaras.exe is located, so it will replaced  by "path where lanzaras.exe\file"
           including the double quotes

## Configure once the associations for all machines

If you have associated small tools all located in lanzaras directory and/or applications that you have in the 
path in all machines it is enough to share the directory lanzaras (e.g. in a dropbox directory) to share
all configured associations. Then for each extension you only have to tell Windows once that has to be launched
with lanzaras.exe

## Compiling lanzaras.cpp

Using gcc to compile with and without debug information 

      g++ lanzaras.cpp -D DBG -mwindows -std=c++11 -static -o lanzaras.exe      
      g++ lanzaras.cpp -mwindows -std=c++11 -static -o lanzaras.exe
      
when compiling with debug (-D DBG) the trace messages are appended to the hardcoded file lanzaras_grabo.txt

