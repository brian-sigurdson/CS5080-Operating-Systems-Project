/*
 * File Name    : sssh.h
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
// include guard
#ifndef SSSH_H
#define SSSH_H

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include <string>
#include <vector>
#include "process.h"
#include "job.h"


//=================================
// the actual class
class SSSH
{
public:
    SSSH();
    ~SSSH();

    // check input
    bool isWaitNum();
    bool runWaitNum();
    bool isExit();
    bool isEmptyString();
    bool isChangeDirectory();
    void runChangeDirectory();

    // manage input
    void parseInput();
    bool runInput();
    std::vector<std::string> getLastCommand();
    void setCurrentCommand(std::vector<std::string> new_cmd);
    void clearCmdLineArgs();
    bool isPriorCommandRequest() const;

    // manage processes
    void addProcess(Process* theProcess);
    void printBGJobs();
    void printBGRunningJobs();
    void printBGFinishedJobs();
    void removeFinishedProcesses();

    // static members to increment and decrement sssh id#
    static int getNextSsshId();
    static void decSsshId();


private:
    // holds processes
    std::vector <Process*> myprocs;
//    Process* theProcess;

    // a vector holding vectors of string that represent indvidual commandline
    // input to facilitate testing, ie. a vector holding a vector of tests
//    std::vector <std::vector <std::string> > testjobs;

    // vector to hold parsed command line
    std::vector<std::string> cmd_line_args;
    std::vector<std::string> prior_cmd_line_args;

    // facilitate comparing string
    std::string toLower(std::string tmp);

    // current count of jobs
    static int currentSsshId;

};


#endif // SSSH_H
