// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $
// MODIFY IT!
#include "commands.h"
#include "debug.h"
#include <vector>

commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"quit"  , fn_exit  }, // added my own little "alias" that I use
}){}

// HELPER FUNCTIONS ====================================================
/**
 * Helper function to pop off the command from a wordvec given
 * @param  words  a wordvec that consists of whole line
 * @return        returns the first word of the command
 */
wordvec pop_command(wordvec words){
   wordvec tmp = words;
   tmp.erase(tmp.begin());
   DEBUGF('c', "commandless words: " << tmp);
   return tmp;
}


/**
 * Parses a path string into a vector of folder names
 * @param path wordvec of all the folder names in the path
 */
wordvec parse_path(string path){
   // wordvec to return
   wordvec ret;

   // temp string to manipulate
   string tmp = path;

   // iterate over the string to parse the slashes out
   while (tmp != ""){

      size_t pos = tmp.find("/");
      DEBUGF('c', "pos = " << pos);

      // if the first slash is root, add an empty string to signify it
      if (pos == 0){
         ret.push_back("");
         tmp.erase(0, 1);
         continue;
      }

      // if there are no slashes in the path, we are done
      if (pos >= tmp.size() - 1) {
         ret.push_back(tmp);
         break;
      };

      // otherwise pop off a directory, and erase it and continue
      string dir = tmp.substr (0, pos);
      ret.push_back(dir);
      tmp.erase(0, pos + 1);
      DEBUGF('c', "curr dir =  " << dir);
      DEBUGF('c', "new pos = " << tmp.find("/"));
   }
   // finally return the wordvec
   return ret;
}

// /**
//  * helper function that returns true if the
//  * @param  path wordvec of the directories, should start at root
//  * @return      [description]
//  */
// bool dir_exists(wordvec path){
//
// }
//
// /**
//  * [get_dir description]
//  * @param  path [description]
//  * @return      [description]
//  */
// bool get_dir(wordvec path){
//
// }

/**
 * Helper function that returns a wordvec of the complete path to the
 * root
 * @param  node  [the node that the path is from]
 * @param  root  [a poine to the root]
 * @return       [wordvec  that is the root]
 */
wordvec get_path_from_root(inode_ptr node, inode_ptr root){
   wordvec ret;
   inode_ptr curr = node;

   while(curr != root){
      ret.insert(ret.begin(), curr->get_name());
      curr = curr->get_parent();
   }
   ret.insert(ret.begin(), "");
   return ret;
}

/**
 * helper function that returns true if the dir has, false if it
 * does not
 * @param  path A string representing a path
 * @return      true if the directory has a child named the name given
 * false otherwise
 */
bool dir_has_child(inode_ptr dir, string name){
   // TODO
   //

   inode_ptr warning_ender = dir;
   string warning_stopper = name;

   return true;
}

/**
 * Helper function for fn_mkdir. Makes a directory of the given path
 * @param path the string describing the path
 * @param state the current inode state. Necesary to get current working
 * directory or the root.
 */
void make_directory(string path, inode_state& state){
   // an inode pointer that will point to the level of the path
   inode_ptr curr_level;

   // parse the directories into a word vector
   wordvec dirs = parse_path(path);

   DEBUGF ('c', "dirs is " << dirs);

   // getting the level which the path refers to using a bunch
   // of else if statements since C++ doesn't support switches
   // on strings

   // if the first character is "/" that means that we are
   // making a directory at the root level
   if (path.find("/") == 0){
      curr_level = state.get_root();
      DEBUGF ('d', "top level is root");
      // the first entry is an empty string to signify root.
      // Pop it off to prevent logic errors
      dirs.erase(dirs.begin());
      // note that this doesn't return here
   } else if (path.find("..") == 0){
      // TODO test
      curr_level = state.get_cwd()->get_parent();
      DEBUGF('d', "Making direcotry at parent level");

      wordvec full_path =
         get_path_from_root(curr_level, state.get_root());

      DEBUGF ('d', "get path from root: " << full_path);

      if (dirs.size() > 2) {
         cout << "error: multiple dirs not supported" << endl;
         return;
      }
      curr_level->make_directory(dirs.at(1));
      return;
   } else if (path.find(".") == 0){

      // works the same as the end event
      DEBUGF('d', "Making direcotry at current level (\".\")");
      curr_level = state.get_cwd();

      // if there are multiple directories in the path, escape while
      if (dirs.size() > 2) {
         cout << "error: multiple dirs not supported" << endl;
         return;
      }

      // make the directory and return
      curr_level->make_directory(dirs.at(1));
      return;
   } else {
      DEBUGF('d', "Making direcotry at current level (else)");
      curr_level = state.get_cwd();

      wordvec full_path =
         get_path_from_root(curr_level, state.get_root());

      DEBUGF ('d', "get path from root: " << full_path);

      if (dirs.size() > 1) {
         cout << "error: multiple dirs not supported" << endl;
         return;
      }

      DEBUGF ('d', "dirs size is: " << dirs.size());
      string dirname = dirs.front();
      DEBUGF ('d', "Dirname is: " << dirname);
      curr_level->make_directory(dirname);
   }

   // iterate through the paths and make the directories
   for(auto it = dirs.begin(); it < dirs.end(); it++){
      // check if exists
      // if (directory_exists(curr_level, path)){
      //    cout << "Error: path " << path << " already exists" << endl;
      //    return;
      // }


      // make the directory with inode's function
      inode_ptr new_dir = curr_level->make_directory(*it);
      DEBUGF('c', "Made directory: " << *it);
      curr_level = new_dir; // TODO test
   }
}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

/**
 * The contents of each file is copied to stdout. An error is reported
 * if no files are specified, a file does not exist, or there is no
 * directory.
 */
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if no arguments are given,  print an error and return
   if (words.size() == 1){
      cout << "Error: no arguments given to cat" << endl;
      return;
   }

   // iterate through the commands and print out the contents
   // this essentially hands off the code to the inode state
   /*for (int i = 1; i < words.size(); i++){
         String path = words.at(i);
         cout << state.readfile(path) << endl;
   }*/

}

void fn_cd (inode_state& state, const wordvec& words){
   // Error handling
   switch (pop_command(words).size()){
      case 0:
         cout << "Error, cd needs at least one argument" << endl;
         return;
      case 2:
         cout << "Error, cd needs can only take one argument" << endl;
         return;
      default: break;
   }

   // pop off the command to manipulate the string
   string path = pop_command(words).at(0);

   // parse the path with helper function
   wordvec dir_list = parse_path(path);

   // set the current directory to the parsed path by handing the
   // function off to inode.cpp
   state.set_cwd(dir_list);

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/**
 * Prints out the words given to the arguemnt
 * @param state unused inode state
 * @param words the command given to the
 */
void fn_echo (inode_state& state, const wordvec& words){
   // Debug stuff (Unused)
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // use helper function to delete unecessary words
   wordvec tmp = pop_command(words);

   // print it to standard out
   cout << tmp << endl;
}

/**
 * Exits the program with the error
 * @param state the state of the current inode
 * @param words the command given
 */
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if exit is the only word on the line, exit just like that
   if (words.size() == 1){
      DEBUGF('c', "EXITING! No arguments given");
      throw ysh_exit_exn();
   }

   /*
   otherwise we are given an exit status to set and should interpret
   it properly. A try/catch block is used here to make sure it's valid
    */
   try {
      // convert the argument is a string, interperet it as an int
      int exit_code = stoi(words.at(1));
      exit_status::set(exit_code);
   } catch (std::invalid_argument& e){
      // if invalid argument is given, exit with status 127
      exit_status::set(127);
   }
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){


   wordvec dir_list = state.get_cwd()->get_dir_list();

   for(auto it = dir_list.begin(); it != dir_list.end(); it++){
      cout << *it << endl;
   }

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){

   // if there are no arguments return an error.
   if (words.size() == 1){
      cout << "yshell: missing operand" << endl;
      return;
   }

   // pop off the command
   wordvec dirs = pop_command(words);

   // iterate over directories given and hand off the work to the
   // helper functions
   for (auto it = dirs.begin(); it != dirs.end(); it++){
      // actually make the directory with the helper function
      make_directory(*it, state);
   }

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   // allocate new string
   string new_prompt;

   // set the string to be the correct one, starts at 1 since the first
   // word is "prompt"
   for ( unsigned i = 1; i != words.size(); i++ ) {
      new_prompt = new_prompt + words.at(i) + " ";
   }
   DEBUGF('c', "New prompt is: " << new_prompt);
   // use the inode state to set the prompt
   state.set_prompt(new_prompt);

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   // hand off all heavy lifting to inode.cpp beacuse that's where the
   // real logic should take place
   cout << state.get_path() << endl;

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
