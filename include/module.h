#ifndef __MODULE_H__
#define __MODULE_H__

//These are custom error strings.
#define MOD_ERR_DL -2
#define MOD_ERR_MSG -1

//Module isn't in error state.
#define MOD_ERR_OK 0

//core defined Error Strings.
#define MOD_ERR_STOP 1
#define MOD_ERR_CRIT 2
#define MOD_ERR_FATAL 3
#define MOD_ERR_EXISTS 4
#define MOD_ERR_NOFILE 5
#define MOD_ERR_IO 6
#define MOD_ERR_API 7
#define MOD_ERR_ABI 8
#define MOD_ERR_MEMORY 9
#define MOD_ERR_UNLOAD 10
#define MOD_ERR_SYNTAX 11
#define MOD_ERR_UNSAFE 12
#define MOD_ERR_NOMOD  13
#define MOD_ERR_SQL    14
#define MOD_ERR_DEPENDENCY 15


#define MOD_STD_PATH      MODULE_DIR
#define MOD_SOCKET_PATH   MODULE_DIR "/socketengine/"
#define MOD_PROTOCOL_PATH MODULE_DIR "/protocol/"
#define MOD_CORE_PATH     MODULE_DIR "/core/"
#define MOD_CONTRIB_PATH  MODULE_DIR "/contrib/"
#define MOD_DB_PATH       MODULE_DIR "/db/"
#define MOD_CLIENT_PATH   MODULE_DIR "/client/"


//Quick reference module errors... designed for ease of use.
#define MOD_CONT MOD_ERR_OK
#define MOD_STOP MOD_ERR_STOP
#define MOD_CRIT MOD_ERR_CRIT
#define MOD_FATAL MOD_ERR_FATAL
#define MOD_MEMORY MOD_ERR_MEMORY

enum ModTypes {
    MOD_TYPE_UNKNOWN, 
    MOD_TYPE_CORE,
    MOD_TYPE_SOCKET,
    MOD_TYPE_PROTOCOL,
    MOD_TYPE_3RD,
    MOD_TYPE_DB
};

char mod_err_msg[512];

#define module_dependency(mod) \
	if (!(module_find(mod))) \
		return MOD_ERR_DEPENDENCY;

#define throwModErr(msg, ...) \
		snprintf(mod_err_msg, sizeof(mod_err_msg), msg, __VA_ARGS__); \
		return MOD_ERR_MSG; \


/**
 * This just looks ugly but i wanted to take into account
 * for directories not having a / and having a /
 * So with this said :) this is what i came up with - Twitch
 */

#define format_filename(dest,filestr)  \
	char format_file_tmp[MAXPATH]; \
	snprintf(format_file_tmp, sizeof(format_file_tmp), "%s%s", (filestr), (strstr(filestr,".so"))? "" : ".so"); \
    snprintf((dest), sizeof(dest) - 1, "%s/%s",find_module_dir((format_file_tmp)), (format_file_tmp)); \
	memset(format_file_tmp,0,sizeof(format_file_tmp));

/**
  * Module API Stuff
  *   This logic kinda looks off at first glance however we need this to make sure
  *   that module API's match, note you can manually change the ABI version on a
  *   module however do that at your own risk, as the module ABI specifies which
  *   modules the core will work properly with.
  */

#define API_MAJOR  0
#define API_MINOR  6
#define API_PATCH  7

#define API_VERSION ((API_MAJOR * 1000) + (API_MINOR * 100))
#define API_VERSION_FULL ((API_MAJOR * 1000) + (API_MINOR * 100) + API_PATCH)

#define MODULE_API(maj,min,patch) ((maj * 1000) + (min * 100) + patch)

/* MAKE_ABI
 * @depreciated I realize this isn't the correct terminology, API is a better
 *              word for this, as its not the binary interface we are looking for
 *
 * XXX Remove this once its no longer in use
 */

#define MAKE_ABI(maj,min,patch) ((maj * 1000) + (min * 100) + patch)

/**
  *   Module Register
  *   	This struct allows the core to pull in the necessary information
  *   	and place it into the module structure.
  *
  *  @param name - The module name -> This is just for personal reference
  *                          as the core relies on absolute paths to find and remove
  *                         modules.
  *  @param version - The modules version in free form
  *  @param abi      - The Binary interface version, this is the version of the API
  *                            used to interact with this module.
  *  @param load     - The function called by the core when loading the module
  *  @param unload  - The function used when unloading a module from the core.
  *
  */



#define MODREGISTER(name,version,author,abi,load,unload) \
static module_info_t modinfo = {          \
  name, version, author, abi, \
  load, unload \
} \


// XXX - Deprecation (Name doesnt make sense, for clarity,
// we want to put it at the buttom of the file)
#define MODHEADER(name,version,author,abi,load,unload) \
static module_info_t modinfo = {          \
  name, version, author, abi, \
  load, unload \
} \



/****
 * Necessary defines for systems without them
 */

#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#ifndef RTLD_LAZY
#define RTLD_LAZY RTLD_NOW
#endif
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif
#ifndef RTLD_LOCAL
#define RTLD_LOCAL 0
#endif

/***
 * Module Actions
 */

#define MOD_ACT_LOAD 0
#define MOD_ACT_UNLOAD 1
#define MOD_ACT_RELOAD 2

/***
 * Module Load Order
 */

 enum ModQuePriority { 
    MOD_QUEUE_PRIO_NONE = -1,
    MOD_QUEUE_PRIO_ALL  =  0,
    MOD_QUEUE_PRIO_PRE  =  1,
    MOD_QUEUE_PRIO_POST =  2,
    MOD_QUEUE_PRIO_STD  =  3
 };


/************************************************/

/**
 * ModInfo - This structure holds module specific information, this is information
 * 		 that is not held to be true at all times, since the modules interact with it
 *                 given that it is only used for basic checks :)
 *
 *  @param name - The module name -> This is just for personal reference
 *                          as the core relies on absolute paths to find and remove
 *                         modules.
 *  @param version - The modules version in free form
 *  @param abi      - The Binary interface version, this is the version of the API
 *                            used to interact with this module.
 *  @param mod_register   - The function called by the core when loading the module,
 *				    and takes place in the registration phase of the load.
 *
 *	NOTE: this is a required function - Modules will fail to load  if this is not specified.
 *
 *  @param mod_unregister  - The function used when unloading a module from the core,
 *				      takes place before the dlclose function is called, in the un-register phase
 *
 *	NOTE: This function is not required modules, however will not unload if this is not specified.
 *
 */


typedef struct module_info_t_ {
     char *name;        //name of module (Modules are allowed to specify there own name)

     char *version;     //Version information in a string - This has no impact on weather or

     char *author;      //Who wrote the module

     long api;          //BINARY INTERFACE version

     int    (*mod_register)	(void);	    /* Register function;   */

	   void	  (*mod_unregister)	(void);		/* Unregister function.	*/

} module_info_t;


/**
 *     Module structures (Module)
 *       This holds primary module information, such as the handle, pathway, and time the module was loaded.
 *	     this information is to be held true and trusted at all times, as the MODULE system is the only thing
 *	     that is allowed to touch this information :)
 */

typedef struct module_ {

    time_t age;            	//Time we were loaded

	  char name[MAXPATH]; 			 	//name of the module for the core purposes.
    char *file;

    void* handle;

    int type;               //Module types


    module_info_t *mi;

} module_t;

// XXX- Deprecation
#define Module module_t


/***********************************/
/**
 * Module Linked List's
 */


dlink_list  modules;

/************************************************************/


int load_protocol();
int load_modules();


/************************************************************/

void 	modules_init ();
void 	modules_purge();

int     module_open   (char *, int);
Module* module_find   (char *);
int     module_exists (char *);
void    module_free   (Module *);



int 	  module_close  (char *);
int    _module_close (Module *);

#define module_loaddir(x, y)  module_loaddir_ext((x), (y), MOD_TYPE_UNKNOWN)


int		 module_loaddir_ext(char *, int, int);

char *  mod_type_string(int); 
char *  find_module_dir(char *module);
char *  create_mod_temp(char *);


/************************************************************/
/**
 * Event Hooks
 */



#endif /*__MODULE_H__*/


