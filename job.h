/*
 * File Name    : job.h
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
#ifndef JOB_H
#define JOB_H


//=================================
// forward declared dependencies


//=================================
// included dependencies
#include <string>
#include <vector>


//=================================
// the actual class
class Job
{
public:
    Job();
    ~Job();
    Job(Job &rhs);
    Job(std::vector<std::string> cmd_line, int pid, bool bg_job, int sssh_id);
    std::string getJobInput() const;
    std::string getCommand() const;
    std::string getArgsAsString() const;
    bool isBGJob() const;
    int getpid() const;
    int getSSSHID() const;
    bool hasPrinted() const;
    void setPrintStatus(bool status);
    bool isRunning() const;
    void printPretty() const;

private:
    std::string the_cmd;
    std::vector <std::string> cmd_line_args;
    bool is_bg_Job;
    int the_pid;
    int the_sssh_id;
    bool has_printed;
};

#endif // JOB_H
