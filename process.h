/*
 * File Name    : process.h
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
#ifndef PROCESS_H
#define PROCESS_H

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "job.h"
//#include <string>
#include <vector>

//=================================
// the actual class
class Process
{
public:
    Process();
    ~Process();
    Process(Process &rhs);
    Process(Process *rhs);

    // to process input
    bool processInput(std::vector<std::string> cmd_line_args);

    void printBGRunningJobs();
    void printBGFinishedJobs();

    bool isBGProcess();
    bool isPipedProcess();
    bool isEmpty();

    Job* getJob();

private:
//    std::vector <Job*> myJobs;
    Job* theJob;
    bool is_bg_Proc;
    bool is_piped_Proc;
    bool is_empty;

    // to process jobs
    bool processJobs(std::vector <std::string> cmd_line_args);
    bool processPipedJobs(std::vector <std::string> cmd_line_args);
    bool processEV(std::vector <std::string> cmd_line_args);
    void processChild(std::vector<std::string> cmd_line_args, int &result,
                      bool isBGProc) const;

    bool isPiped(std::vector <std::string> cmd_line_args) const;
    void setPiped(bool piped);
    bool isBG(std::vector <std::string> cmd_line_args) const;
    bool isEV(std::vector <std::string> cmd_line_args) const;
    std::string toString(std::vector<std::string> tmp_old);
    bool isMultipleFG(std::vector<std::string> cmd_line_args);
    bool processMultipleFGJobs(std::vector <std::string> cmd_line_args);

};

#endif // PROCESS_H
