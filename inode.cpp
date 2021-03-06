// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type, string init_name,
   inode_ptr init_parent):
   inode_nr (next_inode_nr++), type (init_type), name (init_name),
   parent (init_parent)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}


size_t plain_file::size() const {
   size_t size = this->data.size();
   DEBUGF ('i', "size = " << size);
   return size;
}


int plain_file::get_size(){
   return data.size();
}


const wordvec& plain_file::readfile() const {
   // wordvec ret_data;
   //
   // for(auto it = this->data.begin(); it != this->data.end(); it++){
   //    ret.push_back
   // }
   //
   // DEBUGF ('i', ret_data);
   return this->data;
}

void plain_file::writefile (const wordvec& words) {
   this->data = words;
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

/**
 * the constructor for inode_state. Initializes the root and current
 * working directory to be appropriate for the start.
 */
inode_state::inode_state() {
   // set the root to be a pointer to the inode, and make the inode
   // for it
   this->root = make_shared<inode>(DIR_INODE, "", this->root);

   // setting the root's parent manually because we couldn't set it in
   // the constructor
   this->root->parent = this->root;

   inode_ptr root_parent = this->root;

   // get reference to the directory contents
   directory_ptr root_dir_ptr;
   root_dir_ptr = directory_ptr_of(this->root->get_contents());

   // set the dot and dotdot entries
   root_dir_ptr->set_dot(this->root);
   root_dir_ptr->set_dotdot(this->root);

   // set the current working directory to be the root
   this->cwd = root;

   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}
// ====================================================================
// MY FUNCTIONS =======================================================
// ====================================================================

// HELPER =============================================================


// INODE ==============================================================

void inode::list(){
   if (this->type != DIR_INODE){
      return;
   }
   directory_ptr dir_ptr = directory_ptr_of(this->get_contents());
   dir_ptr->list();
}

string digit_length(int number){
   string ret;
   if (number < 10){
      ret = "     ";
   } else if (number < 100){
      ret = "    ";
   } else if (number < 1000){
      ret = "   ";
   } else if (number < 10000){
      ret = "  ";
   } else if (number < 100000){
      ret = " ";
   } else{
      ret = "";
   }
   ret = to_string(number) + ret;
   return ret;
}

string inode::list_info(){
   string ret;

   string number = digit_length(this->inode_nr);
   string size = digit_length(this->get_size());

   ret = number + "   " + size + " " + this->name;
   return ret;

}

void inode::list_recursive(){
   if (this->type != DIR_INODE){
      return;
   }
   directory_ptr dir_ptr = directory_ptr_of(this->get_contents());

   dir_ptr->list_recursive();
   dir_ptr->list();
}

bool inode::has_child(string dir_name){
   if (this->type != DIR_INODE){
      throw runtime_error("inode is not a directory");
   }

   directory_ptr this_dir = directory_ptr_of(this->get_contents());

   // quick reference to make line length smaller
   wordvec dirents = this_dir->get_dir_list();

   // and again
   auto it = find(dirents.begin(), dirents.end(),dir_name);

   if( it != dirents.end()){
      DEBUGF ('i', "Directory " + dir_name + " found");
      return true;
   }
   DEBUGF ('i', "Directory " + dir_name + " NOT found");
   return false;
}

int inode::get_size(){
   if (this->type == DIR_INODE){
      directory_ptr this_dir = directory_ptr_of(this->get_contents());
      return this_dir->get_dir_list().size() - 2;
   }

   if (this->type == PLAIN_INODE){
      plain_file_ptr this_plain =
         plain_file_ptr_of(this->get_contents());
      return this_plain->get_size();
   }

   return 0;
}

inode_ptr inode::get_child(string dir_name){
   if (this->type != DIR_INODE){
      throw runtime_error("inode is not a directory");
   }

   directory_ptr this_dir = directory_ptr_of (this->get_contents());
   return this_dir->get_child(dir_name);
}

/**
 * A function that call's directory's mkdir function, as well as makes
 * sure the derectory does not already exist
 * @param directory_name [description]
 */
inode_ptr inode::make_directory(string& directory_name){
   directory_ptr dir_ptr = directory_ptr_of(this->get_contents());

   DEBUGF ('h', "inode's make directory called");

   if (dir_ptr->has(directory_name)){
      cout << "error: directory " << directory_name << " already exists"
      << endl;
      //return this;
      // TODO error handling
   }
   return dir_ptr->mkdir(directory_name);
}

inode_ptr inode::make_plain(string& file_name){
   directory_ptr this_dir = directory_ptr_of(this->get_contents());
   return this_dir->mkfile (file_name);
}

/**
 * A function that gets the directory list of this inode
 * @param  dir an inode pointer that is of type DIR_INODE
 * @return     a wordvec of all the names of the sub directories
 */
wordvec inode::get_dir_list(){

   if (this->get_type() != DIR_INODE){
      throw runtime_error("this inode is not a directory");
   }

   directory_ptr dir_ptr = directory_ptr_of(this->get_contents());

   return dir_ptr->get_dir_list();
}


/**
 * Return the contents of the inode
 * @return file_base_ptr refering to the contents
 */
file_base_ptr inode::get_contents(){
   return this->contents;
}

/**
 * returns the string of the whole path. Works by taking the inode and
 * getting parents the whole way up.
 * @param state the current inode state. Necessary to get the root.
 * @return a string of the current directory.
 */
string inode::get_path(inode_state& state){
   // make the reference for the first iterations
   inode_ptr curr_dir = this->get_parent();

   // string to return
   string ret = this->get_name();

   // itarating loop. Note: could be a for loop, decided that looked
   // ugly though
   while(curr_dir != state.get_root()){
      DEBUGF('i', "parent is not root! continueing");
      ret = curr_dir->get_name() + "/" + ret;
      DEBUGF('i', "curent path:" + ret);
      curr_dir = curr_dir->get_parent();
   }
   // adding the last / for the root directory
   ret = "/" + ret;
   DEBUGF('i', "Path is: " + ret);
   return ret; // finally returning it
}

/**
 * Simple getter for the name of the inode
 * @return the inode's name
 */
string inode::get_name(){
   DEBUGF ('i', "Getting the name of inode: " + this->name);
   return this->name;
}

/**
 * simple getter for the parent of the inode. Always returns an inode
 * of type directory
 * @return an inode pointer to the parent of the inode. The parent of
 * the root directory is itself
 */
inode_ptr inode::get_parent(){
   DEBUGF('i', "Getting the parent");
   return this->parent;
}

/**
 * getter for the type of the inode
 * @return inode_t of the inode type
 */
inode_t inode::get_type(){
   return this->type;
}

// INODE state ========================================================
/**
 * setter for the shell's prompt
 * @param new_prompt a string that will be the new prompt. Will
 * automatically have a space appended to it.
 */
void inode_state::set_prompt(const string& new_prompt){
   this->prompt = new_prompt;
};

/**
 * setter for the cwd. Effectively makes cwd public.
 * I'm lazy and decided working code is better than good code that's
 * late.
 * @param new_cwd inode pointer to the new cwd
 */
void inode_state::set_cwd(inode_ptr new_cwd){
   this->cwd = new_cwd;
}

/**
 * getter for the shell's prompt
 * @return a string for the prompt
 */
const string& inode_state::get_prompt(){
   DEBUGF ('i', "prompt = " + this->prompt);
   return this->prompt;
}

/**
 * Getter for the current working direcotry of the shell through the
 * state
 * @return inode pointer to the current working direcotry
 */
inode_ptr inode_state::get_cwd(){
   DEBUGF ('i', "getting current working directory");
   return this->cwd;
}

/**
 * returns the root of the file structure
 * @return an inode pointer pointing at the root of the file structure
 */
inode_ptr inode_state::get_root(){
   DEBUGF ('i', "getting root");
   return this->root;
}

/**
 * Simple getter for the path of the current inode. Really extra, but
 * there just in case.
 * @return a string that represents the current path
 */
string inode_state::get_path(){
   DEBUGF('i', "getting path from cwd");
   return this->cwd->get_path(*this);
}



// directory ==========================================================

/**
 * Makes a directory in the
 * @pre  directory name must not already exist in parent directory
 * @param  dirname the name of the new directory
 * @return          a reference to the new directorie's inode
 */
inode_ptr directory::mkdir(const string& name){

   DEBUGF('h', "mkdir called");
   // check if it has the name
   if (this->has(name)){
      cout << "Error: " + name + " already exists" << endl;
      auto pair = this->dirents.find(name);
      inode_ptr ret = pair->second;
      return ret;
   }

   // get a reference for the parent
   inode_ptr dir_parent = dirents.at(".");

   // make the new directory
   inode_ptr new_dir = make_shared<inode>(DIR_INODE, name, dir_parent);

   // get a reference to make the line length better
   directory_ptr new_directory;
   new_directory = directory_ptr_of(new_dir->get_contents());

   // set the "." entry
   new_directory->set_dot(new_dir);
   // set the ".." entry
   new_directory->set_dotdot(dir_parent);

   this->dirents[name] = new_dir;

   // return the finished directory
   return new_dir; // TODO ask why star!
}

inode_ptr directory::mkfile(const string& name){

   DEBUGF('f', "Making file: " + name);
   if (this->has(name)){
      cout << "Error: " + name + " already exists" << endl;
      auto pair = this->dirents.find(name);
      inode_ptr ret = pair->second;
      return ret;
   }

   // get a reference for the parent
   inode_ptr dir_parent = dirents.at(".");

   // make the new directory
   inode_ptr file = make_shared<inode>(PLAIN_INODE, name, dir_parent);

   this->dirents[name] = file;

   return file;

}
/**
 * Setter for the parent of a director
 * @param parent inode pointer pointing to the parent of the directory
 * Will always point at another directory except for the root which
 * will point at itself.
 */
void directory::set_dotdot(inode_ptr parent){
   if (dirents.find("..") == dirents.end()){
      // directory .. does not exist, insert it;
      pair<string, inode_ptr> dot_dot = std::make_pair("..", parent);
      dirents.insert(dot_dot);
   }else {
      dirents.at("..") =  parent;
   }
}

/**
 * Sets the directory's "dot". Will always point at itself.
 * @param dot inode pointer that refers to directory
 */
void directory::set_dot(inode_ptr dot){
   if (dirents.find(".") == dirents.end()){
      // if directory . does not exist, insert it in
      pair<string, inode_ptr> new_dot = std::make_pair(".", dot);
      dirents.insert(new_dot);
   }
   else {
      dirents.at(".") = dot;
   }
}

/**
 * Function that checks if the directory has a child named the argument
 * given
 * @param  name name of the direcotry
 * @return      true if it is exists, false otherwise.
 */
bool directory::has(const string& name){
   wordvec dirlist = this->get_dir_list();
   DEBUGF('h', "name: " + name);
   DEBUGF('h', "dirlist: " << dirlist);

   auto it = find(dirlist.begin(), dirlist.end(), name);

   if (it == dirlist.end()){
      DEBUGF('h', "not found!");
      // there is no directory of that name.
      return false;
   } else{
      DEBUGF('h', "Found!");
      return true;
   }

}

/**
 * a getter that returns the list of children of the directory
 * @return wordvec with all the names of the children of the directory
 */
wordvec directory::get_dir_list(){
   wordvec ret;
   for (auto it = this->dirents.begin(); it != this->dirents.end();
      it++ ){
      string curr_dir = it->first;
      DEBUGF('i', "gettind dir" << curr_dir);
      ret.push_back(curr_dir);
   }
   return ret;
}

inode_ptr directory::get_child(string child_name){
   if (this->dirents.find(".") == this->dirents.end()){
      throw runtime_error ("error: directory not found");
   }
   return dirents[child_name];
}

void directory::list(){
   auto entries = this->dirents;

   inode_ptr this_dir = this->dirents["."];
   inode_ptr this_parent = this->dirents[".."];

   cout << "inode_nr size   filename" << endl;

   for (auto it = entries.begin(); it != entries.end(); it++){
      // check if the current directory is the . or ..
      inode_ptr curr_dir = it->second;
      if (curr_dir == this_dir || curr_dir == this_parent){
         continue;
      }
      cout << curr_dir->list_info() << endl;
   }
}

void directory::list_recursive(){

   auto entries = this->dirents;

   inode_ptr this_dir = this->dirents["."];
   inode_ptr this_parent = this->dirents[".."];

   for (auto it = entries.begin(); it != entries.end(); it++){

      inode_ptr curr_dir = it->second;

      if (curr_dir == this_dir || curr_dir == this_parent){
         continue;
      }
      it->second->list_recursive();
   }
}

// PLAIN FILE ==========================================================
