#include <sys/stat.h> // posix stat

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <vector>

using namespace std;

long getMillas ()
{
   static auto initialTime = ::chrono::system_clock::now();
   return ::chrono::duration_cast<chrono::milliseconds>(::chrono::system_clock::now() - initialTime).count();
}

// Note : we try to make a fprintf to the console (stdout) in all cases, DBG or not DBG,
//        but since we call WinExec we get no console .. so it is useless
//

#ifdef DBG

   #define DBG_DISK(msg) { \
            FILE * hand = fopen ("lanzaras_grabo.txt", "at"); \
            if (hand) { \
               fprintf (hand, "%09ld ", getMillas ()); \
               fprintf msg ; \
               fprintf (hand, "\n"); \
               fclose (hand); \
               hand = stdout; \
               fprintf msg ; \
         }}

#else

   #define DBG_DISK(msg) { FILE * hand = stdout; fprintf msg ; }

#endif


//
//    Lanzaras.exe is a utility to passthrough the windowes application association mechanism
//
//    Any extension can be associated to Lanzares.exe and then configure the rule in lanzaras."extension".rule file
//
//    In this configuration file the final executable is invoqued with specific parameters. Two special characters can
//    be used: '@' and '$'
//
//       @program
//           will search for the first path in the environmen variable PATH where the file "program" is found and
//           then it will be replaced by "path from env\program" including the double quotes!
//
//       $file
//           where $ represents the path where Lanzaras.exe is found, so it will replaced  by "path where lanzaras.exe\file"
//           including the double quotes!
//
//    If no rule is found lanzaras will use the default rule
//
//       @java.exe -jar $gastona.jar
//
//    this rule will work if java is installed and gastona.jar is found in the same directory of lanzaras.exe.
//
//


bool existsFileInPath (const string & path)
{
  struct stat buffer;
  return (stat (path.c_str(), & buffer) == 0);
}

vector<string> & split (const string & str, char sepa, vector<string> & arr)
{
    stringstream sstre(str);
    string toke;
    while (getline(sstre, toke, sepa))
        arr.push_back(toke);
    return arr;
}

vector<string> split (const string & str, char sepa)
{
    vector<string> arr;
    split(str, sepa, arr);
    return arr;
}

bool findFileInPaths (const string & fileName, string & fullPath)
{
   fullPath = string (getenv("PATH"));
   vector<string> arrPat = split (fullPath, ';');
   fullPath = "";

   for (int ii = 0; ii < arrPat.size (); ii ++)
   {
      if (arrPat[ii].length () == 0) continue;
      fullPath = arrPat[ii];
      fullPath += (arrPat[ii][arrPat[ii].length ()-1] != '\\') ? "\\": "";
      fullPath += fileName;
      if (existsFileInPath (fullPath))
      {
         // last arrangements (double quoting does not work in stat!!)
         fullPath = "\"";
         fullPath += arrPat[ii];
         fullPath += (arrPat[ii][arrPat[ii].length ()-1] != '\\') ? "\\": "";
         fullPath += fileName;
         fullPath += "\"";
         return true;
      }
   }
   fullPath = fileName;
   return false;
}

int main (int nn, char ** aa)
{
   DBG_DISK ((hand, "hey is me, nn = %d, aa[0] = \"%s\"\n", nn, aa[0]));
   for (int ii = 0; ii < nn; ii ++)
   {
      DBG_DISK ((hand, "%d) [%s]\n", ii, aa[ii]));
   }
   string LLAM = "@java.exe -jar $gastona.jar"; // Default for calling gast files

   // 1------- deduce path of lanzaras.exe
   //
   string pathLanzaras = (nn > 0) ? aa[0]: "";
   int pos = pathLanzaras.find_last_of('\\');

   pathLanzaras = (pos != string::npos) ? pathLanzaras.substr (0, pos): "";
   if (pathLanzaras.length () > 0)
      pathLanzaras += "\\";

   string param1 = (nn > 1) ? aa[1]: "";
   if (param1.length () < 5 || param1.substr (param1.length() - 5) != ".gast")
   {
      DBG_DISK ((hand, "search for specific rule!"));
      int pos = param1.length();
      while (pos > 0 && param1[pos-1] != '.') pos --;

      if (pos == 0)
      {
         DBG_DISK ((hand, "No extension, cannot associate any launcher!"));
         return 0;
      }
      DBG_DISK ((hand, "Reconocemos [%s]\n", param1.substr(pos).c_str ()));

      string ruleFile = pathLanzaras + "lanzaras.";
      ruleFile += param1.substr(pos);
      ruleFile += ".rule";

      if (!existsFileInPath (ruleFile))
      {
         printf ("File [%s] not found!\n", ruleFile.c_str ());
         return 0;
      }

      FILE * fi = fopen (ruleFile.c_str (), "rt");
      if (fi)
      {
         char buff[4024+1];
         LLAM = fgets (buff, 4024, fi);
         fclose (fi);
      }
   }

   DBG_DISK ((hand, "base call [%s]\n", LLAM.c_str ()));

   if (LLAM.length () == 0)
   {
      DBG_DISK ((hand, "Nothing to launch!\n"));
      return 0;
   }

   string CALL = "";

   // Replace @xxxx by the found path in enviroment + "\xxxx"
   // ONLY the first @ !!!
   if (LLAM[0] == '@')
   {
      int pos = 0;
      while (pos < LLAM.length() && LLAM[pos] != ' ') pos ++;
      findFileInPaths(LLAM.substr (1, pos-1), CALL);
      LLAM = LLAM.substr (pos);
   }

   // Replace $xxxx by pathLanzaras + "xxxx"
   //
   while (LLAM.length () > 0)
   {
      int pos = 0;
      while (pos < LLAM.length() && LLAM[pos] != '$') pos ++;
      if (pos >= LLAM.length ()) break;

      CALL += LLAM.substr(0, pos);
      if (pathLanzaras.length () > 0)
      {
         CALL += "\"";
         CALL += pathLanzaras;
      }
      int pos2 = pos + 1;
      while (pos2 < LLAM.length() && LLAM[pos2] != ' ') pos2 ++;

      CALL += LLAM.substr(pos + 1, pos2 - pos - 1);
      if (pathLanzaras.length () > 0)
      {
         CALL += "\"";
      }
      LLAM = LLAM.substr (pos2);
   }

   CALL += LLAM;

   DBG_DISK ((hand, "call before parameters [%s]\n", CALL.c_str ()));

   for (int ii = 1; ii < nn; ii ++)
   {
      CALL += " \"";
      CALL += aa[ii];
      CALL += "\"";
   }

   DBG_DISK ((hand, "final call [%s]\n", CALL.c_str ()));
   WinExec(CALL.c_str (), SW_HIDE);

   DBG_DISK ((hand, "when we were youngs...\n"));
   return 0;
}
