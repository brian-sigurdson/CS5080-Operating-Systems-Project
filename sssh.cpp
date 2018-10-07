/*
 * File Name    : sssh.cpp
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
// included dependencies
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // execlp
#include <stdlib.h> // system
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <list>

#include "sssh.h"
#include "process.h"
#include "job.h"

//=================================
// forward declared dependencies

//=================================
// initalize static member variables
int SSSH::currentSsshId = 0;

//=============================================================================
// begin main()
//=============================================================================
int main(int argc, char* argv[])
{
    // the shell
    SSSH the_shell;

    bool stop = false;

    while(stop == false){

        // background jobs?
        the_shell.printBGJobs();

        // parse the input
        the_shell.parseInput();

        // check a few items before proceeding
        if(the_shell.isExit()){
            // exit
            std::cout << std::endl <<
                         "** Info:  Exiting Super Simple SHell."
                      << std::endl << std::endl;

            the_shell.clearCmdLineArgs();
            // use this, it works.
            // odd behavior with exit(EXIT_SUCCESS) and return
            // low priority bug
            stop = true;
            continue;
        }
        if(the_shell.isEmptyString()){
            // empty string command
            std::cout << std::endl <<
                         "** WARNING:  Empty string command found.  Please try again."
                      << std::endl << std::endl;

            // clear the command line argurments
            the_shell.clearCmdLineArgs();
            // start loop over and get input again
            continue;
        }
        if(the_shell.isWaitNum()){
            // wait num
            if(the_shell.runWaitNum()){
                std::cout << std::endl <<
                             "** Info:  wait num command processed."
                          << std::endl << std::endl;
            } else{
                std::cout << std::endl <<
                             "** Info:  wait num cannot be processed."
                          << std::endl << std::endl;
            }

            // clear the command line argurments
            the_shell.clearCmdLineArgs();
            // start loop over and get input again
            continue;
        }
        if(the_shell.isChangeDirectory()){
            // cd dir
            the_shell.runChangeDirectory();
            std::cout << std::endl <<
                         "** Info:  cd dir command processes."
                      << std::endl << std::endl;

            // clear the command line argurments
            the_shell.clearCmdLineArgs();
            // start loop over and get input again
            continue;
        }
        if(the_shell.isPriorCommandRequest()){
            // user requested !!
            // over-ride the current "!!" command with whatever was the prior

            the_shell.setCurrentCommand( the_shell.getLastCommand() );
            // now initiate a normal process
            if( the_shell.runInput() ){
                //
                std::cout << std::endl <<
                             "** Info:  !! command processes."
                             << std::endl << std::endl;
            }
                // clear the command line argurments
                the_shell.clearCmdLineArgs();
            // start loop over and get input again
            continue;
        }

        // proceed to running some jobs
        if(the_shell.runInput()){
            // success
            // now what?
            // clear the vector of the current commands
            the_shell.clearCmdLineArgs();
            // I don't think there is anything else to do at this point, but loop

        } else{
            // not success
            // anything todo here?
            // clear the vector of the current commands
            the_shell.clearCmdLineArgs();
            // i don't think so.
        }

    }

    return EXIT_SUCCESS;
}
//=============================================================================
// end main()
//=============================================================================

// =============================================================================
/*
 * Description
 * add a new process to the vector of processess
 *
 * Parameters
 * new process
 *
 * Return Value
 * void
 */
void SSSH::addProcess(Process* theProcess)
{
    // add a new process
    SSSH::myprocs.push_back(theProcess);
//    SSSH::theProcess = theProcess;
}

// =============================================================================
/*
 * Description
 * parse the input and store it in the cmd_line_args vector
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
void SSSH::parseInput()
{
    // reading the cmd line
    std::string word;
    std::string line;    

    // prompt
    std::cout << "sssh: ";
    // process input
    std::getline(std::cin, line);
    std::istringstream iss(line);
    while ( iss >> word) {
        cmd_line_args.push_back(word);
    }
}

// =============================================================================
/*
 * Description
 * run the input
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
bool SSSH::runInput()
{

    // create new process
    Process *p = new Process();

    // pass the args to process
    if( p->processInput(cmd_line_args) ){
        // success

        // if this is a background job
        if(p->isBGProcess()){
            // add it
            addProcess(p);
        }
        return true;
    } else{
        return false;
    }
}

// =============================================================================
/*
 * Description
 * get the last command
 *
 * Parameters
 * void
 *
 * Return Value
 * prior command
 */
std::vector<std::string> SSSH::getLastCommand()
{
    return SSSH::prior_cmd_line_args;
}

// =============================================================================
/*
 * Description
 * set the current command
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
void SSSH::setCurrentCommand(std::vector<std::string> new_cmd)
{
    SSSH::cmd_line_args = new_cmd;
}
// =============================================================================
/*
 * Description
 * constructor
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
SSSH::SSSH()
{
}

// =============================================================================
/*
 * Description
 * destructor
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
SSSH::~SSSH()
{
    // delete pointers to processes
//    for(std::vector<Process*>::iterator iter = myprocs.begin();
//        iter != myprocs.end();
//        iter++){

//        delete *iter;
//    }

//    myprocs.clear();
}

// =============================================================================
/*
 * Description
 * did user enter wait num?
 *
 * Parameters
 * dir
 *
 * Return Value
 * void
 */
bool SSSH::isWaitNum()
{
    if(cmd_line_args.size() == 0){
        return false;
    }else {
        // get the command and compare
        return SSSH::toLower(cmd_line_args.at(0)) == "wait";
    }
}

// =============================================================================
/*
 * Description
 * handle built in wait num
 *
 * Parameters
 * time to wait
 *
 * Return Value
 * void
 */
bool SSSH::runWaitNum()
{
    // the number should be the second value in the arguments list
    int num = -1;
    std::string test = cmd_line_args.at(1);

    if( isdigit( test[0]) ){
        num = atoi(test.c_str());

        // iterate through all of the processes checking if the indicated
        // sssh number exists and is running
        bool matchFound = false;
        Job *theJob;


        // check that the sssh # exists and is running
        for(size_t i = 0; i < myprocs.size(); i++){

            theJob = myprocs.at( i)->getJob();

            if(theJob != NULL){
                // test if we have a match
                while( (theJob->getSSSHID() == num) && (theJob->isRunning()) ){
                    // if we're here, then the sssh# matches and job running
                    matchFound = true;
                    std::cout << "Job# " << num << " is running." << std::endl;
                    sleep(1);
                }

                if(matchFound){
                    std::cout << "Job# " << num << " is finished." << std::endl;
                    break;
                }
            }
        }

        if( !matchFound ){
            std::cout << "Job# " << num << " is not found or job already finised." << std::endl;
        }

    }else{
        return false;
    }

    return true;

}

// =============================================================================
/*
 * Description
 * user enter empty string?
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
bool SSSH::isEmptyString()
{
    return cmd_line_args.size() == 0;
}

// =============================================================================
/*
 * Description
 * user enter exit?
 *
 * Parameters
 * void
 *
 * Return Value
 * void
 */
bool SSSH::isExit()
{
    if(cmd_line_args.size() == 0){
        return false;
    }else {
        // get the command and compare
        return SSSH::toLower(cmd_line_args.at(0)) == "exit";
    }
}

// =============================================================================
/*
 * Description
 * user enter cd dir?
 *
 * Parameters
 * void
 *
 * Return Value
 * bool
 */
bool SSSH::isChangeDirectory()
{
    // i thought cd dir might be implemented as a fg process, but didn't seem
    // to work

    if(cmd_line_args.size() == 0){
        return false;
    }else {
        // get the command and compare
        return SSSH::toLower(cmd_line_args.at(0)) == "cd";
    }
}

// =============================================================================
/*
 * Description
 * handle built in cd dir
 *
 * Parameters
 *
 *
 * Return Value
 * success failure
 */
void SSSH::runChangeDirectory()
{
    // user entered cd dir
    std::string dir = SSSH::cmd_line_args.at(1);
    std::string result;
    int test = chdir(dir.c_str());
    if( test < 0 ){
        //error
        result = "unsuccessful";
    }else{
        // success
        result = "successful";
    }

    std::cout << "Changing to " << dir << " was " << result << "." << std::endl;
}

// =============================================================================
/*
 * Description
 * helper function to make strings lowercase
 *
 * Parameters
 * string to transform
 *
 * Return Value
 * lower case string
 */
std::string SSSH::toLower(std::string tmp)
{
    // get the command
    tmp = cmd_line_args.at(0);

    // to lower
    std::transform( tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

    return tmp;
}

// =============================================================================
/*
 * Description
 * keep a copy of the last command
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void SSSH::clearCmdLineArgs()
{
    // test if the current command was "!!" which is to run the prior command
    // if so, then don't save it to prior command, just discard it

    if( SSSH::isPriorCommandRequest()){
        // then just discard it and keep prior command
        // this allows the multiple use of !!,albeit the same command
        cmd_line_args.clear();
    }else{

        SSSH::prior_cmd_line_args.clear();
        SSSH::prior_cmd_line_args = cmd_line_args;
        SSSH::cmd_line_args.clear();
    }
}

// =============================================================================
/*
 * Description
 * get the next sssh id#
 *
 * Parameters
 *
 *
 * Return Value
 * the next value
 */
int SSSH::getNextSsshId()
{
    // increment and return
    currentSsshId += 1;
    return currentSsshId;
}

// =============================================================================
/*
 * Description
 * decrement sssh #
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void SSSH::decSsshId()
{
    // decrement the current sssh id #
    if(currentSsshId > 0){
        currentSsshId--;
    }
}

// =============================================================================
/*
 * Description
 * print the background jobs
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void SSSH::printBGJobs()
{
    // print fg
    std::cout << std::endl;
    printBGRunningJobs();

    std::cout << std::endl;

    // print bg
    printBGFinishedJobs();
    std::cout << std::endl;

    // remove finised processes
    removeFinishedProcesses();
}

// =============================================================================
/*
 * Description
 * print the background jobs still running
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void SSSH::printBGRunningJobs()
{
    // are there any jobs?
    if( myprocs.empty() ){
        std::cout << "No background jobs running." << std::endl;
    } else{
        // header
        std::cout << "Running BG Jobs" << std::endl;
        std::cout << "---------------" << std::endl;

        Process* theProc;
        std::vector<Process*>::iterator itr;
        for(itr = myprocs.begin(); itr != myprocs.end(); itr++){

            theProc = (*itr);

            // if not null and not empty
            if( (theProc != NULL) && !(theProc->isEmpty() ) ){
                // okay to print
                theProc->printBGRunningJobs();
            }

            theProc = NULL;
        }
    }
}

// =============================================================================
/*
 * Description
 * print the background jobs that have finised running
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void SSSH::printBGFinishedJobs()
{
    // are there any jobs?
    if( myprocs.empty() ){
//        std::cout << "No background jobs running." << std::endl;
    } else{
        // header
        std::cout << "Finished BG Jobs" << std::endl;
        std::cout << "---------------" << std::endl;

        Process *theProc;
        std::vector<Process*>::iterator itr;
        for(itr = myprocs.begin(); itr != myprocs.end(); itr++){

            //copy const shouldmake thrProc a ref not a pointer
            theProc = (*itr);

            // if not null and not empty
            if( (theProc != NULL) && !(theProc->isEmpty() ) ){
                // okay to print
                theProc->printBGFinishedJobs();
            }

            theProc = NULL;
        }
    }
}
// =============================================================================
/*
 * Description
 * remove finished processes
 *
 * Parameters
 *
 *
 * Return Value
 * bool
 */
void SSSH::removeFinishedProcesses()
{
    // a process should remove finished jobs, so any process with size
    // of zero should also be removed

    // vectors causing problems bc i'm removing random positions and i think
    // the iterator pointer is being invalidated.

    // move to linked list data structure
    // won't rewrite everthing, due to time constraints, but
    // this should do as this is the only place causing problems

    std::vector<Process*> myCopy;
    Process *theProc;

    // copy desirable processes to the list
    for(size_t i = 0; i < myprocs.size(); i++){

        theProc = myprocs.at(i);
        // if the process is not null and not empty, keep it
        if( theProc != NULL && !(theProc->isEmpty()) ){
            // store a reference in the list
            myCopy.push_back( theProc );
            theProc = NULL;
        }
    }

    // clear the vector s/b okay b/c it should call the destructor on each
    // process which does nothing
    myprocs.clear();

    // if not empty
    if( !myCopy.empty() ){
        // put the kept process back in vector
        std::vector<Process*>::iterator itr;
        for(size_t i = 0; i < myCopy.size(); i++){
            myprocs.push_back(myCopy.at(i));
        }
    }

    // clear the copy
//    myCopy.clear();

}

/*
 * Description
 * test for command !!
 *
 * Parameters
 * command line input
 *
 * Return Value
 * bool
 */
bool SSSH::isPriorCommandRequest() const
{
    if( !cmd_line_args.empty() ){

        std::string tmp =cmd_line_args.at(0);

        if( tmp == "!!"){
            return true;
        } else{
            return false;
        }

    } else{
        return false;
    }

}

