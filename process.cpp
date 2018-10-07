/*
 * File Name    : process.cpp
 * Descripton   : Develope a Super Simple SHell (sssh), per project specs.
 * Author       : Brian K. Sigurson
 * Course       : CS5080
 * Assignment   : Project 01
 * Due Date     : 2017-03-27
 *
 * Extra Credit : 1) Command History !!
 *              : 2) Environment variables
 *              : 3) Multiple Foreground Commands
 */

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include "process.h"
#include "sssh.h"
//#include "job.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

// =============================================================================
/*
 * Description
 * constructor
 *
 * Parameters
 * none
 *
 * Return Value
 * nothing
 */
Process::Process()
{
    theJob = NULL;

}

// =============================================================================
/*
 * Description
 * copy constructor
 *
 * Parameters
 * none
 *
 * Return Value
 * nothing
 */
Process::Process(Process &rhs)
{
    theJob = rhs.getJob();
    is_bg_Proc = rhs.isBGProcess();
    is_piped_Proc = rhs.isPipedProcess();
    is_empty = rhs.isEmpty();

}

// =============================================================================
/*
 * Description
 * copy constructor
 *
 * Parameters
 * none
 *
 * Return Value
 * nothing
 */
Process::Process(Process *rhs)
{
    // this instance is made of only primitives
    theJob = rhs->getJob();
    // these are all primitives
    is_bg_Proc = rhs->isBGProcess();
    is_piped_Proc = rhs->isPipedProcess();
    is_empty = rhs->isEmpty();

}

// =============================================================================
/*
 * Description
 * destructor
 *
 * Parameters
 * none
 *
 * Return Value
 * nothing
 */
Process::~Process()
{
//    // delete pointers to jobs
//    for(std::vector<Job*>::iterator iter = myJobs.begin();
//        iter != myJobs.end();
//        iter++){

//        delete *iter;
//    }

//    myJobs.clear();
}

// =============================================================================
/*
 * Description
 * takes command line input and decides what to do: fg, bg, piped, etc...
 *
 * Parameters
 * command line input
 *
 * Return Value
 * void
 */
bool Process::processInput(std::vector<std::string> cmd_line_args)
{
    /* for the moment this is just testing for the regular project criteria
     * I suppose this is the most natural place to hand the extra credit work also
     *
     * perhaps a quick test for the extra credit items, and if none to process,
     * then the code for the regular items run.
     */

    // check if it is a foreground, background, or pipped job

    // piped job?
    Process::is_piped_Proc = Process::isPiped(cmd_line_args);
    if( is_piped_Proc ){
        // piped job
        return( processPipedJobs(cmd_line_args) );
    }

    // multiple fg job?
    if( Process::isMultipleFG(cmd_line_args) ){
        // multiple fg job
        return( processMultipleFGJobs(cmd_line_args) );
    }

    // environmental variables?
    if( Process::isEV(cmd_line_args) ){
        // environmental var job
        return ( Process::processEV(cmd_line_args));
    }

    // for this project, at this point if you're not any of the above
    // scenarios, then you're simply a bg or fg job
    // bg job?
    is_bg_Proc = Process::isBG(cmd_line_args);

    // non-piped, job
    // both fg & bg jobs are processed by processJobs()
    return( processJobs(cmd_line_args) );

}

/*
 * Description
 * Process foreground jobs
 *
 * Parameters
 * command line input
 *
 * Return Value
 * void
 */
bool Process::processJobs(std::vector<std::string> cmd_line_args)
{
    // fork a child
    int result = -10;
    pid_t childId = fork();

    switch(childId){
    case -1:
        // bad, fork() failed
        return false;

    case 0:
        // child
        processChild(cmd_line_args, result, Process::is_bg_Proc);

        if(result != 0){
            // there was a problem
            std::cout << std::endl <<
                         "** WARNING:  Problem with command.  Please try again."
                      << std::endl << std::endl;
            //std::cout << "child: result = " << result << std::endl;
        }

        _exit(EXIT_SUCCESS);

        break;

    default:
        // parent
        int status = -10;

        // if this is not a bg process
        if( !is_bg_Proc){
            // wait for child
            waitpid(childId, &status, 0);

            if(status == -1){
                // error
                std::cout << std::endl <<
                             "** WARNING:  Problem with command.  Please try again."
                          << std::endl << std::endl;
            }
        //        std::cout << "parent:parent id = " << getpid() << std::endl;
        //        std::cout << "parent:child id = " << childId << std::endl;
        } else{
            // this is a bg job, so save it to the list

            // create a new job instance and store it in the vector
            // tell it its pid #, sssh #,and its fg/bg status
//            Process::myJobs.push_back(
            theJob = new Job(
                        cmd_line_args,
                        childId,
                        Process::is_bg_Proc,
                        SSSH::getNextSsshId()
                    );
        }
    }

    return true;
}

/* Description
* Process child
*
* Parameters
*
*
* Return Value
* void
*/
void Process::processChild(std::vector<std::string> cmd_line_args,
                           int &result, bool isBGProc) const
{
    //std::cout << "child:child id = " << getpid() << std::endl;

    // thanks to one of the comments on this page
    //http://stackoverflow.com/questions/26452771/if-i-have-a-vector-of-strings-how-i-can-use-the-strings-with-execvp-to-execute
    // I was able to make this work without a lot of code changes
    // I was having a terrible time trying to get my vector of strings
    // as an array of pointers to char arrays

    // needed for execvp()
    std::vector<char *> commandVector;

    // most processes will only have one job, so iterator overhead
    // probably not necessary
    size_t size = cmd_line_args.size();
    for(size_t cnt = 0; cnt < size; cnt++){

        // skip the & for bg processes
        if( (cnt == (size-1) && (isBGProc == true) )){
            // if were at the last element of a background command
            // skip it so the & isnt' included in the system call
            // token for linux shell, but not when using system calls
            continue;
        }
        commandVector.push_back( &cmd_line_args[cnt][0] );
    }
    // execvp expects null terminating
    commandVector.push_back(NULL);

    // pass the vector's internal array to execvp
    char **command = &commandVector[0];
    result = execvp(command[0], command);
}

/*
 * Description
 * test for pipes
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool Process::isPiped(std::vector <std::string> cmd_line_args) const
{
    for(std::vector<std::string>::iterator iter = cmd_line_args.begin();
        iter != cmd_line_args.end();
        iter++){
        if( *iter == "|"){
            return true;
        }
    }
    return false;
}

/*
 * Description
 * set if piped job
 *
 * Parameters
 * bool
 *
 * Return Value
 * bool
 */
void Process::setPiped(bool piped)
{
    Process::is_piped_Proc = piped;
}

/*
 * Description
 * test for environmental variables
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool Process::isEV(std::vector <std::string> cmd_line_args) const
{
    std::string tmp;

    for(std::vector<std::string>::iterator iter = cmd_line_args.begin();
        iter != cmd_line_args.end();
        iter++){

        tmp = *iter;

        if( tmp.c_str()[0] == '$'){
            return true;
        }
    }
    return false;
}

/*
 * Description
 * test for pipes
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool Process::processPipedJobs(std::vector <std::string> cmd_line_args)
{

    /* The use of system() is from
     *
	 *		The Linux Programming Interface - Michael Kerrisk
     *      Ch 27.6 Executing a Shell Command: system()
     *
     * Kerrisk suggests using system() over exec*() calls,
     * due to its ease in facilitating such programming, unless performance
     * is of critical concern, due to the additional overhead imposed by the
     * use of system().
     */

    // create space between cmd prompt and output
    std::cout << std::endl;

    std::string str = toString(cmd_line_args);
    int result = system( str.c_str() );

    if(result < 0){
        // alert if error
        std::cout << "Warning:  Error processing piped command" << std::endl;
        std::cout << str << std::endl;
    }

    return true;
}

bool Process::processMultipleFGJobs(std::vector <std::string> cmd_line_args)
{

    /* The use of system() is from
     *
	 *		The Linux Programming Interface - Michael Kerrisk
     *      Ch 27.6 Executing a Shell Command: system()
     *
     * Kerrisk suggests using system() over exec*() calls,
     * due to its ease in facilitating such programming, unless performance
     * is of critical concern, due to the additional overhead imposed by the
     * use of system().
     */

    // create space between cmd prompt and output
    std::cout << std::endl;

    std::string str = toString(cmd_line_args);
    int result = system( str.c_str() );

    if(result < 0){
        // alert if error
        std::cout << "Warning:  Error processing multiple foreground command" << std::endl;
        std::cout << str << std::endl;
    }

    return true;
}
/*
 * Description
 * test for background job
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool Process::processEV(std::vector <std::string> cmd_line_args)
{

    /* The use of system() is from
     *
	 *		The Linux Programming Interface - Michael Kerrisk
     *      Ch 27.6 Executing a Shell Command: system()
     *
     * Kerrisk suggests using system() over exec*() calls,
     * due to its ease in facilitating such programming, unless performance
     * is of critical concern, due to the additional overhead imposed by the
     * use of system().
     */

    // create space between cmd prompt and output
    std::cout << std::endl;

    std::string str = toString(cmd_line_args);
    int result = system( str.c_str() );

    if(result < 0){
        // alert if error
        std::cout << "Warning:  Error processing command" << std::endl;
        std::cout << str << std::endl;
    }

    return true;
}

/*
 * Description
 * test for background job
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool Process::isBG(std::vector <std::string> cmd_line_args) const
{    
    if( !cmd_line_args.empty() ){
        return (cmd_line_args.back() == "&");
    }else{
        return false;
    }

}

// =============================================================================
/*
 * Description
 * print status of running background processes
 *
 * Parameters
 *
 * Return Value
 * void
 */
void Process::printBGRunningJobs()
{
    // get a copy
    Job *tmpJob = Process::theJob;

    if( tmpJob != NULL ){
        if( tmpJob->isBGJob() && tmpJob->isRunning() ){
            tmpJob->printPretty();
            tmpJob->setPrintStatus(true);
        }
    }
}

// =============================================================================
/*
 * Description
 * print status of running background processes
 *
 * Parameters
 *
 * Return Value
 * void
 */
void Process::printBGFinishedJobs()
{    
    // get a copy (copy constructor should work)
    Job *tmpJob = Process::theJob;

    if( tmpJob != NULL ){
        if( tmpJob->isBGJob() && !(tmpJob->isRunning()) ){

            tmpJob->printPretty();

            // decrement the count
            SSSH::decSsshId();

            delete theJob;
            theJob = NULL;
        }
    }

}

// =============================================================================
/*
 * Description
 * how many jobs doe this process contain
 *
 * Parameters
 *
 * Return Value
 * number of jobs the process contains
 */
bool Process::isEmpty()
{
    return Process::theJob == NULL;
}

// =============================================================================
/*
 * Description
 * is this a bg process
 *
 * Parameters
 *
 * Return Value
 * number of jobs the process contains
 */
bool Process::isBGProcess()
{
    return Process::is_bg_Proc;
}

// =============================================================================
/*
 * Description
 * is this a piped process
 *
 * Parameters
 *
 * Return Value
 * bool
 */
bool Process::isPipedProcess()
{
    return Process::is_piped_Proc;
}

// =============================================================================
/*
 * Description
 * return the job
 *
 * Parameters
 *
 * Return Value
 * the job
 */
Job* Process::getJob()
{
    return Process::theJob;
}

// =============================================================================
/*
 * Description
 * multiple fg jobs?
 *
 * Parameters
 *
 * Return Value
 * bool
 */
bool Process::isMultipleFG(std::vector<std::string> cmd_line_args)
{
    for(std::vector<std::string>::iterator iter = cmd_line_args.begin();
        iter != cmd_line_args.end();
        iter++){
        if( *iter == ";"){
            return true;
        }
    }
    return false;
}
// =============================================================================
/*
 * Description
 * change vector to string
 *
 * Parameters
 * vector of string
 *
 * Return Value
 * new string
 */
std::string Process::toString(std::vector<std::string> tmp_old)
{
    std::string tmp_new;

    for(size_t i = 0; i < tmp_old.size(); i++){
        if(i == 0){
            // eliminates an empty char in position 0
            tmp_new = tmp_old.at(i);
        }else{
            tmp_new = tmp_new + " " + tmp_old.at(i);
        }
    }

    return tmp_new;
}
