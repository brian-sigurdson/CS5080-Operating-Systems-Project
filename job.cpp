/*
 * File Name    : job.cpp
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
#include "job.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <sys/wait.h>
#include <signal.h>


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
Job::Job()
{

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
Job::~Job()
{

}

// =============================================================================
/*
 * Description
 * constructor
 *
 * Parameters
 * a vector of command line arguments
 *
 * Return Value
 * nothing
 */
Job::Job(std::vector<std::string> cmd_line, int pid, bool bg_job, int sssh_id)
{
    // set print status
    has_printed = false;

    // set sssh id
    the_sssh_id = sssh_id;

    // set this job's process id
    the_pid = pid;

    // is this a background job
    is_bg_Job = bg_job;

    // the command is at position 0
    the_cmd = cmd_line.at(0);

    // rest are the command optons etc...
    size_t size = cmd_line.size();

    for(size_t i = 1; i < size; i++){
        cmd_line_args.push_back(cmd_line.at(i));
    }
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
Job::Job(Job &rhs)
{
    the_cmd = rhs.the_cmd;
    cmd_line_args = rhs.cmd_line_args;
    is_bg_Job = rhs.is_bg_Job;
    the_pid = rhs.the_pid;
    the_sssh_id = rhs.the_sssh_id;
    has_printed = rhs.has_printed;
}

// =============================================================================
/*
 * Description
 * get the original command line input
 *
 * Parameters
 * none
 *
 * Return Value
 * the unformatted command line input
 */
std::string Job::getJobInput() const
{
    return getCommand() + " " + getArgsAsString();
}

// =============================================================================
/*
 * Description
 * return the command for this job
 *
 * Parameters
 *
 *
 * Return Value
 * command as string
 */
std::string Job::getCommand() const
{
    return the_cmd;
}

// =============================================================================
/*
 * Description
 * return the arguments for this job's command
 *
 * Parameters
 *
 *
 * Return Value
 * arguments as string
 */
std::string Job::getArgsAsString() const
{
    std::string tmp;
    size_t size = cmd_line_args.size();

    for(size_t i = 0; i < size; i++){
        tmp = tmp + " " + cmd_line_args.at(i);
    }

    return tmp;
}

// =============================================================================
/*
 * Description
 * is this a background job
 *
 * Parameters
 *
 *
 * Return Value
 * bool
 */
bool Job::isBGJob() const
{
    return is_bg_Job;
}

// =============================================================================
/*
 * Description
 * returns the pid for this job
 *
 * Parameters
 *
 *
 * Return Value
 * int
 */
int Job::getpid() const
{
    return Job::the_pid;
}

// =============================================================================
/*
 * Description
 * returns the sssh id for this job
 *
 * Parameters
 *
 *
 * Return Value
 * int
 */
int Job::getSSSHID() const
{
    return Job::the_sssh_id;
}

// =============================================================================
/*
 * Description
 * has this job been printed already
 *
 * Parameters
 *
 *
 * Return Value
 * bool
 */
bool Job::hasPrinted() const
{
    return Job::has_printed;
}

// =============================================================================
/*
 * Description
 * set print status
 *
 * Parameters
 *
 *
 * Return Value
 * void
 */
void Job::setPrintStatus(bool status)
{
    Job::has_printed = status;
}

// =============================================================================
/*
 * Description
 * is this background job still running
 *
 * Parameters
 *
 * Return Value
 * true or false
 */
bool Job::isRunning() const
{
    // not sure this is working properly
    //return 0 == kill(Job::the_pid, 0);

    // should return zero if the_pid has not changed state yet
    // and the pid of the job if finished

//    int tmp = waitpid(the_pid, NULL, WNOHANG);
//    std::cout << "Job::isRunning() = " <<  tmp << std::endl;
//    return tmp == 0;

    // kill (above) didn't seem to work, this does
    return 0 == waitpid(the_pid, NULL, WNOHANG);
}

// =============================================================================
/*
 * Description
 * prints sssh# and command line input
 *
 * Parameters
 *
 * Return Value
 * void
 */
void Job::printPretty() const
{
    // use this
    std::cout << "sssh# = " << getSSSHID()
              << " : pid# = " << getpid()
              << " : user input = " << getJobInput()
              << std::endl;

//    // this is for testing
//    std::cout << "bg job? = " << isBGJob()
//              << " ; sssh# = " << getSSSHID()
//              << " ; pid = " << getpid()
//              << " ; the job = " << getJobInput()
//              << " ; running? = " << isRunning()
//              << std::endl;
}
